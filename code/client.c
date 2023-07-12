/* 线程模块的实现 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>   // 获取文件元数据
#include "http.h"
#include "socket.h"
#include "resource.h"
#include "client.h"

// 线程过程函数实现
void* client(void* arg){
    // 定义 CA*类型结构体
    CA* ca = (CA*)arg;
    printf("%d.%ld > 客户机线程处理开始\n", getpid(), syscall(SYS_gettid));
    for(;;){
        // 接收 HTTP请求
        printf("%d.%ld > 接收 HTTP请求\n", getpid(), syscall(SYS_gettid));
        char* req = recvRequest(ca->conn);
        if(req == NULL){
            break;
        }
        printf("%d.%ld > 请求报文:\n%s\n", getpid(), syscall(SYS_gettid), req);

        // 解析 HTTP请求
        printf("%d.%ld > 解析 HTTP请求\n", getpid(), syscall(SYS_gettid));
        HTTP_REQUEST hreq = {}; // 用于输出请求中的关键参数
        if(resolutionRequest(req, &hreq) == -1){
            free(req);          // 解析失败, 释放存储区
                                // 解析失败后, 避免资源占据存储区, 造成资源的浪费
            break;
        }
        free(req);  // 解析成功, 也要释放存储区
                    // 解析成功后, req 请求变量就没有用处了
                    // 所需要的变量已经存储进入了 hreq 结构体
        req = NULL; // 好的编程习惯, 存储区释放后置空

        // 路径处理
        // 请求路径: /c/error.h
        // 存储路径: /home/nilera/WebServer/home
        // 最终路径: /home/nilera/WebServer/home/c/error.h
        char root[PATH_MAX + 1];    // 存储路径 []
        char path[PATH_MAX + 1];    // 真实路径 []
        strcpy(root, ca->home);     // 将请求路径存储到 root 中
                                    // 存储路径 [/home/nilera/WebServer/home]
        // 如果路径最后存在 "/" , 去掉路径中最后的 "/"
        if(root[strlen(root) - 1] == '/'){
            root[strlen(root) - 1] = '\0';
        }
        // 拼接文件的真实路径
        strcpy(path, root);         // 真实路径 [/home/nilera/WebServer/home]
        strcat(path, hreq.path);    // 真实路径 [/home/nilera/WebServer/home/c/error.h]

        // 如果请求路径为 "/" , 应将首页文件响应给对方
        if(strcmp(hreq.path, "/") == 0){
            // 如果请求路径为 "/" 拼接后为 "/home/nilera/WebServer/home/"
            // 我们要响应首页, 则对应路径为 "/home/nilera/WebServer/home/index.html"
            strcat(path, "index.html");
        }

        // 构造响应时所需要的响应体
        // 这里赋初始值是大多数情况下应该给出的响应, 通过后续的判断可以再进行更改
        HTTP_RESPOND hres = {"HTTP/1.1", 200, "OK", "text/html"};
        // 搜索资源
        if(searchResource(path) == -1){
            hres.status = 404;
            strcpy(hres.desc, "NOT FOUND");
            // 当无法找到资源时, 对应路径为 "/home/nilera/WebServer/home/404.html"
            strcpy(path, root);         // 这里是将 root 中的内容复制到 path 中
                                        // path [...] -> [/home/nilera/WebServer/home]
            strcat(path, "/404.html");  // path [/home/nilera/WebServer/home/404.html]
        } else if(identifyResourceType(path, hres.type) == -1) {
            hres.status = 404;
            strcpy(hres.desc, "NOT FOUND");
            strcpy(path, root);
            strcat(path, "/404.html");
        }
        // 确定文件长度, 有两种方法, 一个是通过获取文件元数据, 一个是通过获取读写位置
        // 这里选用获取文件元数据
        struct stat st;
        if(stat(path, &st) == -1){
            perror("Stat");
            break;
        }
        hres.length = st.st_size;

        // 获取连接状态
        if(strlen(hreq.connection)){    // 判断 hreq.connection 长度是否为 0
                                        // 如果为 0, 说明没有该字段
            strcpy(hres.connection, hreq.connection);
        } else if(strcasecmp(hreq.protocol, "HTTP/1.0") == 0) { // 协议版本为 HTTP/1.0 传入 "Close"
            strcpy(hres.connection, "Close");
        } else {
            strcpy(hres.connection, "Keep-alive");
        }

        // 构造响应
        printf("%d.%ld > 构造响应\n", getpid(), syscall(SYS_gettid));
        char head[1024];
        if(constructRespond(&hres, head) == -1){
            break;
        }
        printf("%d.%ld > 响应报文:\n%s\n", getpid(), syscall(SYS_gettid), head);

        // 发送响应头
        if(sendRespondHead(ca->conn, head) == -1){
            break;
        }

        // 发送响应体
        if(sendRespondBody(ca->conn, path) == -1){
            break;
        }

        // 如果连接状态为 Close 则退循环结束线程
        if(strcasecmp(hres.connection, "Close") == -1){
            break;
        }
    }
    // 后处理
    close(ca->conn);    // 关闭通信套接字
    free(ca);           // 释放 ca 所占存储区
    printf("%d.%ld > 客户机线程处理结束\n", getpid(), syscall(SYS_gettid));
    return NULL;
}


