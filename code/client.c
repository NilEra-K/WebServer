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
        HTTP_RESPOND hres = {"HTTP/1.1", "200", "OK", "text/html"};
        // 搜索资源
        if(searchResource(path) == -1){
            hres.status = 404;
            strcpy(hres.desc, "NOT FOUND");
            // 当无法找到资源时, 对应路径为 "/home/nilera/WebServer/home/404.html"
            strcpy(path, root);         // 这里是将 root 中的内容复制到 path 中
                                        // path [...] -> [/home/nilera/WebServer/home]
            strcat(path, "404.html");   // path [/home/nilera/WebServer/home/404.html]
        } else if(identifyResourceType(path, hres.type) == -1) {
            hres.status = 404;
            strcpy(hres.desc, "NOT FOUND");
            strcpy(path, root);
            strcat(path, "404.html");
        }
    }
    return NULL;
}


