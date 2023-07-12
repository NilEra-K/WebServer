/* 服务器模块头文件 */
#ifndef _SERVER_H_
#define _SERVER_H_

/**
 * 初始化服务器
 * initServer()
 * Params:
 * port 端口号, 调用初始化套接字函数时需要传入 port 参数
 * 因此这里需要传入 port
*/
int initServer(short port);

/**
 * 运行服务器
 * int runServer()
 * Params:
 * home 资源本地路径
*/
int runServer(const char* home);

/**
 * 销毁服务器
 * void deinitServer(void)
 * Params:
 * void 空
*/
void deinitServer(void);

#endif // _SERVER_H_
