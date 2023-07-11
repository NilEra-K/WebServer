/* HTTP 模块的实现 */
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#define __USE_GNU
#include <string.h>
#include <time.h>
#include "http.h"

// 实现: HTTP 请求的解析
int resolutionRequest(const char* request, HTTP_REQUEST* hreq){
    /* =====================================================
        GET /cpp/locale/setlocale.html HTTP/1.1
        Host: 192.168.xxx.xxx
        Connection: keep-alive
        User-Agent: Mozilla/5.0
        Accept: text/html
        Referer: http://192.168.xxx.xxx/cpp/locale.html
    ===================================================== */

    // sscanf() 从一个字符串里获得输入
    sscanf(request, "%s%s%s", hreq->method, hreq->path, hreq->protocol);

    // strstr() 从一个字符串中找字符串, 如果找到则返回查找字符串子一个字符的地址
    // strcasestr() 功能与 strstr() 相同, 但是其查找字符串时大小写不敏感, 即查找 "AbC" 同 查找 "abc"
    // strcasestr() 使用时需要在 string.h 头文件前 定义 __USE_GNU 宏 #define __USE_GNU
    char* connection = strcasestr(request, "Connection");
    if(connection){
        // connection 相当于从 Connection 字段开始后面的一个大字符串
        // %*s 是一种虚读的方法, 表示会读如两个字段, 即 "Connection:" 和 "keep-alive"
        // %*s 则表示忽略 "Connection:", 不进行存储
        sscanf(connection, "%*s%s", hreq->connection);
    }
    printf("%d.%ld > [%s] [%s] [%s] [%s]\n", getpid(), syscall(SYS_gettid), hreq->method, hreq->path, hreq->protocol, hreq->connection);

    // 判断请求方法
    // strcasecmp() 同理, 是非大小写敏感的字符串比较方法
    // 如果 hreq->method 和 "GET" 相等, 则返回 0
    if(strcasecmp(hreq->method, "GET")){
        printf("%d.%ld > 无效的方法\n", getpid(), syscall(SYS_gettid));
        return -1;
    }

    // 判断协议版本
    if(strcasecmp(hreq->protocol, "HTTP/1.0") && strcasecmp(hreq->protocol, "HTTP/1.1")){
        printf("%d.%ld > 无效的协议\n", getpid(), syscall(SYS_gettid));
        return -1;
    }
    return 0;
}

// HTTP 响应的构造
int constructRespond(const HTTP_RESPOND* hres, char* head){
    /* =====================================================
        HTTP/1.1 200 OK
        Server: NilEra-WebServer 4.0
        Date: Sat 03 Dec 2022 02:06:24
        Content-Type: text/html
        Content-Length: 32212
        Connection: keep-alive
    ===================================================== */
    char dateTime[32];          // 定义一个存储区用于存储 Date
    time_t now = time(NULL);    // 定义一个 time_t 类型的变量用于获取当前时间
    // strftime() 用于格式化输出时间字符串
    // 该函数需要传入四个参数, 包括 存储区, 存储区大小, 输出时间格式, tm 类型时间结构体
    // gmtime() 用于将 time_t 类型的时间转化为 tm 类型时间结构体
    // 可以使用 man 手册查看具体函数以及参数
    strftime(dateTime, sizeof(dateTime), "%a %d %b %Y %T", gmtime(&now));
    sprintf(head, "%s %d %s\r\n"
                  "Server: NilEra-WebServer 1.0\r\n"
                  "Date: %s\r\n"
                  "Content-Type: %s\r\n"
                  "Content-Length: %ld\r\n"
                  "Connrction: %s\r\n\r\n", 
                  hres->protocal, hres->status, hres->desc, 
                  dateTime, hres->type, hres->length, hres->connection);
    return 0;
}


