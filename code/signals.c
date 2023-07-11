/* 信号模块实现 */
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include "signals.h"

// 初始化信号, 忽略大部分信号
int initSignals(void){
    printf("%d.%ld > 忽略大部分信号\n", getpid(), syscall(SYS_gettid));
    for(int signum = SIGHUP; signum <= SIGRTMAX; signum++){
        if(signum != SIGINT && signum != SIGTERM){  // 如果非 2号或 15号信号, 就进行忽略
            signal(signum, SIG_IGN);
        }
    }
    return 0;
}
