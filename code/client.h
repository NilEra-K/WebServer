/* 线程模块头文件 */
#ifndef _CLIENT_H_
#define _CLIENT_H_

// 定义一个结构体, 用来存储线程过程函数的参数
// 对于一个客户端线程来说, 必须知道本地资源的存储路径和通信套接字
// 本地资源套接字用于检测本地资源是否存在
// 通信套接字用于表示和哪个客户端进行通信
typedef struct clientArgs{
    const char* home;   // 资源在本地的存储路径
    int conn;           // 通信套接字
} CA;


/**
 * 线程过程函数
 * 线程负责完成和客户端的通信
 * 每个线程都要做成分离线程, 因为无法确定服务器和客户端需要通信多久, 因此不能因为回收线程阻塞主线程
 * client()
 * Params:
 * arg: 
*/
void* client(void* arg);

#endif // _CLIENT_H_

