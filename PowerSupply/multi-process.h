// Multi-process function lib
// Downloaded from gist.github.com/shonenada/7007688

#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

// 定义最大子进程数量
#define MAX_PROCESS_NUM 100

pid_t createProcess(void(*ptr) ());
int addInProcessList(pid_t pid);
void waitAllProcess();
int isOutsideProcess(pid_t pid);
int isOutsideAllProcess();
int processError();
void killProcess();
void killChildren();

void childProcess();
void parentForkInfo(pid_t pid);
int tprintf(const char* fmt, ...);

// 初始化进程计数器i
int processCount = 0;
// 初始化子进程数组
pid_t processList[MAX_PROCESS_NUM];

// 将子进程添加到子进程数组中
int addInProcessList(pid_t pid){
    if (processCount < MAX_PROCESS_NUM){
        processList[processCount] = pid;
        processCount = processCount + 1;
        return 1;
    }
    return -1;
}

// wait 所有子进程
void waitAllProcess(){
    int i;
    for(i=0;i<processCount; ++i){
        waitpid(processList[i], NULL, 0);
    }
}

// 检测子进程是否出错。
int processError(){
    int i;
    for(i=0;i<processCount; ++i){
        if(processList[i] == -1){
            return 0;
        }
    }
    return 1;
}

// 创建子进程
pid_t createProcess(void(*ptr) ()){
    pid_t pid;
    // isOutsideAllProcess() 满足的时候，表明当前处于父进程。
    if (isOutsideAllProcess()){
        // 创建子进程
        pid = fork();
        // 将所创建的子进程存入 pid_t 数组中
        if (addInProcessList(pid) != -1){
            if(pid == 0){
                // 处于所创建的子进程执行状态下，并执行子进程程序。
                (*ptr)();
            }
        }else{
            printf("Full processes!");
            killChildren();
            exit(1);
        }
    }
    return pid;
}

// kill 进程
void killProcess(pid_t pid){
    if(pid == 0){
        printf("Process %d exit!", getpid());
        exit(1);
    }
}

// kill 所有子进程
void killChildren(){
    int i;
    for(i=0;i<processCount; ++i){
        killProcess(processList[i]);
    }
}

// 判断当前状态是否处于某子进程外
int isOutsideProcess(pid_t pid){
    return (int) (pid != -1 && pid != 0);
}

// 判断当前状态是否处于所有子进程外，即当前为父进程状态。
int isOutsideAllProcess(){
    int i;
    for (i=0;i<processCount;++i){
        if (isOutsideProcess(processList[i]) == 0){
            return 0;
        }
    }
    return 1;
}

// 子进程执行
void childProcess(){
    int i;
    sleep(1);
    for (i=0; i<5; ++i){
        tprintf("Hello from Child Process %d. %d times\n", getpid(), i+1);
        sleep(1);
    }
}

// 输出父进程信息
void parentForkInfo(pid_t pid){
    tprintf("Parent forked one child process -- %d.\n", pid);
    tprintf("Parent is waiting for children to exit.\n");
}

int tprintf(const char* fmt, ...){
    va_list args;
    struct tm * tstruct;
    time_t tsec = time(NULL);
    tstruct = localtime (&tsec);
    printf("%02d:%02d:%02d: %5d| ", tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec, getpid());
    va_start(args, fmt);
    return vprintf(fmt, args);
}
