// 主模块
#include <stdio.h>
#include <stdlib.h>
#include "server.h"

// 完成代码后使用如下命令进行编译
// gcc main.c *.o -o webserver -lpthread 
// sudo ./webserver

int main(int argc, char* argv[]){
    // ./a.out port home
    //         ^    ^
    //     端口号    资源主路径
    // ./a.out 8080 ../home

    // 初始化服务器
    // atoi() 函数, 将一个字符串转化为一个整数
    if(initServer(argc < 2 ? 80 : atoi(argv[1]) == -1)){
        return -1;
    }

    // 运行服务
    if(runServer(argc < 3 ? "../home" : argv[2]) == -1){
        return -1;
    }

    // 销毁服务器
    deinitServer();
    return 0;
}

