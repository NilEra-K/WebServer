/* 服务器模块的实现 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/resource.h>
#include "socket.h"
#include "signals.h"
#include "client.h"
#include "server.h"

// 初始化最大文件描述符数
// 我们希望服务器能尽可能多的打开文件, 因此我们希望能够最大化文件描述符数
// static 表示当前函数作用于只限于当前文件
// 其没有在头文件中声明, 说明其并不是一个"希望被其他文件调用"的函数
static int initMaxFiles(void){
    // 资源限制
    struct rlimit rl;
    if(getrlimit(RLIMIT_NOFILE, &rl) == -1){
        perror("Getrlimit");
        return -1;
    }
    // 如果当前进程的文件描述符数小于系统极限文件描述符数
    // 将当前进程的文件描述符数设置成系统极限文件描述符数
    if(rl.rlim_cur < rl.rlim_max){
        rl.rlim_cur = rl.rlim_max;
        if(setrlimit(RLIMIT_NOFILE, &rl) == -1){    // 使设置生效
            perror("Setrlimit");
            return -1;
        }
    }
    return 0;
}

// 实现: 初始化服务器
int initServer(short port){
    // 初始化最大文件描述符数
    if(initMaxFiles() == -1){
        return -1;
    }
    // 初始化信号
    if(initSignals() == -1){
        return -1;
    }
    // 初始化套接字
    if(initSocket(port) == -1){
        return -1;
    }
    return 0;
}

// 实现: 运行服务器
int runServer(const char* home){
    for(;;){
        // 接收客户端的连接请求
        int conn = acceptClient();
        if(conn == -1){
            return -1;
        }
        // 开线程, 具有分离属性
        pthread_t tid;              // 线程 ID
        pthread_attr_t attr;        // 线程属性, 可以传递给 pthread_create() 函数的第二个形参
        pthread_attr_init(&attr);   // 初始化线程属性变量
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        CA* ca = malloc(sizeof(CA));// 动态分配一块 CA 类型的存储区
        ca->conn = conn;
        ca->home = home;
        int error = pthread_create(&tid, &attr, client, ca);    // 这里是跨文件的分配和释放
        if(error){
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            return -1;
        }
    }
    return 0;
}

// 实现: 销毁服务器
void deinitServer(void){
    // 销毁套接字
    deinitSocket();
}



