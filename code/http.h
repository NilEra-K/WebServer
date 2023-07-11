/* HTTP 模块头文件 */
#ifndef _HTTP_H_
#define _HTTP_H_
#include <linux/limits.h>       // 使用 PATH_MAX 系统定义宏
#include <sys/types.h>

// 定义一个结构体: 用来表示解析得到的数据
typedef struct httpRequest{
    char method[32];            // 请求方法
    char path[PATH_MAX + 1];    // 资源路径
                                // PATH_MAX 系统定义宏, 表示系统规定的路径长度最大值
    char protocol[32];          // 协议版本
    char connection[32];        // 连接状态
} HTTP_REQUEST;

// 定义一个结构体: 用来表示构造响应头需要的数据
typedef struct httpRespond
{
    char protocal[32];          // 协议版本
    int status;                 // 状态码
    char desc[128];             // 状态描述
    char type[64];              // 类型
    off_t length;               // 长度, 常使用 off_t 类型表示文件的长度
    char connection[32];        // 连接状态
} HTTP_RESPOND;

/**
 * HTTP 请求的解析
 * resolutionRequest()
 * Parmas:
 * request: 请求信息
 * herq: 输出解析得到的数据, 是一个输出型参数
*/
int resolutionRequest(const char* request, HTTP_REQUEST* hreq);

/**
 * HTTP 响应的构造
 * constructRespond()
 * Params:
 * hres: 传入响应头所需要的数据
 * head: 输出响应头
*/
int constructRespond(const HTTP_RESPOND* hres, char* head);

#endif  // _HTTP_H_

