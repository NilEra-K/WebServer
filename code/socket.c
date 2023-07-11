/* 套接字模块的实现 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "socket.h"

static int s_sock = -1;     // 侦听套接字
                            // 这里定义时加了 static 关键字 
                            // 因为其为一个全局变量, static 关键字可以保证只在当前文件中使用
                            // 防止其他文件使用 extern 使用该变量

// 实现: 初始化套接字
int initSocket(short port){
    printf("%d.%ld > 创建套接字\n", getpid(), syscall(SYS_gettid));
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(s_sock == -1){
        perror("Socket");
        return -1;
    }

    // 设置套接字 
    // 如果服务器因为某些异常原因终止, 需要重启, 可能因为端口还未释放, 导致无法绑定(bind 函数报错), 使重启失败
    // 这种情况可以选择等待 1~2 分钟, 等待端口释放
    // 也可以选择对套接字进行设置, 允许端口被多个套接字绑定, 也就是端口复用
    printf("%d.%ld > 设置套接字\n", getpid(), syscall(SYS_gettid));
    int on = 1;     // 输出型参数, 在使用 setsockopt() 函数时需要传入该值, 传入 optval 位置
                    // 输出包含新选项值的缓冲区
    if(setsockopt(s_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        perror("Setsockopt");
        return -1;
    }

    // 组织地址结构
    printf("%d.%ld > 组织地址结构\n", getpid(), syscall(SYS_gettid));
    struct sockaddr_in ser;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(port);
    ser.sin_addr.s_addr = INADDR_ANY;

    // 绑定地址结构
    printf("%d.%ld > 绑定地址结构\n", getpid(), syscall(SYS_gettid));
    if(bind(s_sock, (struct sockaddr*)&ser, sizeof(ser)) == -1){
        perror("Bind");
        return -1;
    }

    // 启动侦听
    printf("%d.%ld > 启动侦听\n", getpid(), syscall(SYS_gettid));
    if(listen(s_sock, 1024) == -1){
        perror("Listen");
        return -1;
    }
    return 0;
}

// 实现: 接收客户端的连接请求
int acceptClient(void){
    // 等待客户机的连接
    printf("%d.%ld > 等待客户机的连接\n", getpid(), syscall(SYS_gettid));
    struct sockaddr_in cli;         // 用来输出客户端的地址结构
    socklen_t len = sizeof(cli);    // 用来输出地址结构大小
    int conn = accept(s_sock, (struct sockaddr*)&cli, &len);   // 用于后续通信的套接字
    if(conn == -1){
        perror("Accept");
        return -1;
    }

    // 接收客户机连接
    printf("%d.%ld > 接收到客户机 %s: %hu 的连接\n", getpid(), syscall(SYS_gettid), 
                                                  inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
    return conn;
}

// 接收 HTTP 请求
char* recvRequest(int conn){
    // 因为我们所接收的请求无法确定大小, 所以我们很难分配合适大小的存储区
    // 因此我们希望动态分配存储区
    char* req = NULL;   // 记录动态分配的存储区的首地址
    ssize_t len = 0;    // 记录已经接收的总字节数
    for(;;){
        char buf[1024] = {};
        ssize_t size = recv(conn, buf, sizeof(buf) - 1, 0);
        if(size == -1){
            perror("Recv");
            free(req);  // 如果 recv() 函数出错, 那么之前分配的内存失去意义, 这里进行释放
            return NULL;
        }
        if (size == 0){
            printf("%d.%ld > 客户机关闭连接\n", getpid(), syscall(SYS_gettid));
        }

        // realloc() 函数可以用来动态分配存储区
        // 这里是将 req 的存储区大小存储为 len + size + 1
        // 如果缓冲区存满, 结果就是 0 + 1024 + 1, req 的存储区大小变为 1025
        // 再一次存满, 结果就是 1025 + 1024 + 1, req 的存储区大小变为 2050
        // ... 依次类推
        req = realloc(req, len + size + 1); // 扩大存储区

        // memcpy() 函数可以用来进行内存拷贝
        // 这里就是将缓冲区 buf 里的数据拷贝到指定地址
        // 第一个参数是拷贝首地址
        // 第二个参数是拷贝的缓冲区
        // 第三个参数是拷贝的大小
        memcpy(req + len, buf, size + 1);   // 拷贝此次接收数据到存储区中
        len = len + size;                   // 总长度类加

        // 如果请求中有两个 "\r\n" 则表示接收完成
        // 此时退出循环
        if(strstr(req, "\r\n\r\n")){
            break;
        }
    }
    return req;
}

// 实现: 发送响应头
int sendRespondHead(int conn, const char* head){
    if(send(conn, head, strlen(head), 0) == -1){
        perror("Send");
        return -1;
    }
    return 0;
}

// 实现: 发送响应体
int sendRespondBody(int conn, const char* path){
    // 打开文件
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        perror("Open");
        return -1;
    }

    // 循环读取并发送文件
    // 这里的 while 循环内的循环条件
    // 首先调用 read() 函数读取文件内容到缓冲区, 定义一个 len 保存 read() 函数的返回值
    // 然后判断 len 是否大于 0, 当 len 大于 0 的时候, 即说明读到了数据
    // 当 len 等于 0 时, 正常读完文件
    // 当 len 等于 -1 时, 读取出错
    char buf[1024] = {};
    ssize_t len;
    while((len = read(fd, buf, sizeof(buf))) > 0){
        if(send(conn, buf, len, 0) == -1){
            perror("Send");
            return 0;
        }
    }
    if(len == -1){
        perror("read");
        return -1;
    }
    close(fd);
    return 0;
}

// 实现: 销毁套接字
void deinitSocket(void){
    close(s_sock);
}


