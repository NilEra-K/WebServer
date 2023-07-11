/* 通信模块头文件 */
#ifndef _SOCKET_H_
#define _SOCKET_H_

/**
 * 初始化套接字
 * 需要完成: 创建套接字, 组织地址结构, 绑定, 监听
 * initSocket()
 * Params:
 * port 开放的端口号
*/
int initSocket(short port);

/**
 * 接收客户端的连接请求
 * acceptClient()
 * Params:
 * void 参数为空, 这里考虑到可以把套接字设置为全局变量, 所有函数都可以使用, 所以可以不传入参数
*/
int acceptClient(void);

/**
 * 接收 HTTP请求
 * recvRequest()
 * Params:
 * conn 传入一个套接字, 用于确定具体和谁通信
 * 返回值为 char* , 返回接收请求的首地址
*/
char* recvRequest(int conn);

// 发送 HTTP响应, 可以分为 发送响应头 和 发送响应体 两个部分
/**
 * 发送响应头
 * 响应头是在 HTTP 模块中构造的部分
 * sendRespondHead()
 * Parmas:
 * conn 代表客户端对应的套接字, 将信息发送给客户端
 * head 响应头存储区的地址
*/
int sendRespondHead(int conn, const char* head);

/**
 * 发送响应体
 * 响应体是具体的文件内容
 * sendRespondBody()
 * Params:
 * conn 代表客户端对应的套接字, 将信息发送给客户端
*/
int sendRespondBody(int conn, const char* path);

/**
 * 销毁套接字
 * deinitSocket()
 * Params:
 * void 空
*/
void deinitSocket(void);

#endif // _SOCKET_H_

