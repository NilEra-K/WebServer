/* 资源模块的实现 */
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include "resource.h"
#include "mime.h"

// 实现: 判断资源是否存在
// 可以用 open 函数
// 但是我们可以使用 access 判断一个资源的权限
// R_OK 表示文件是否有读权限, F_OK 表示文件是否存在, ...
// 这里给 R_OK 判断文件是否有可读权限, 因为存在并不一定可读
// 不可读的文件只判断存在也没什么必要
int searchResource(const char* path){
    return access(path, R_OK);
}

// 实现: 识别资源类型
// 这里需要搭配 mime.h 头文件使用
int identifyType(const char* path, char* type){
    // Path: /home/c/error.html
    // 确定拓展名
    // strrchr() 函数可以用于在一个大字符串中寻找小字符串, 并且为逆序查找
    // 成功返回对应字符的首地址
    // 失败返回空(NULL)
    char* suffix = strrchr(path, '.');
    if(suffix ==  NULL){
        printf("%d.%ld > 无法获取拓展名\n", getpid(), syscall(SYS_gettid));
        return -1;
    }
    for(int i = 0; i < sizeof(s_mime) / sizeof(s_mime[0]); i++){
        if(strcasecmp(suffix, s_mime[i].suffix) == 0){
            strcpy(type, s_mime[i].type);
            return 0;
        }
    }
    printf("%d.%ld > 不可识别的资源类型\n", getpid(), syscall(SYS_gettid));
    return -1;
}



