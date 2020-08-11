//
//  AutoAntiDebug.m
//  TEstDemo
//
//  Created by EGLS_BMAC on 2020/8/11.
//  Copyright © 2020 EGLS_BMAC. All rights reserved.
//

#import "AutoAntiDebug.h"
#import <sys/sysctl.h>
#import <sys/types.h>
#import <dlfcn.h>

typedef int (*ptrace_ptr_t)(int _request, pid_t _pid, caddr_t _addr, int _data);

__attribute__((__always_inline)) bool checkTracing() {
    size_t size = sizeof(struct kinfo_proc);
    struct kinfo_proc proc;
    memset(&proc, 0, size);
    
    int name[4];
    name[0] = CTL_KERN;
    name[1] = KERN_PROC;
    name[2] = KERN_PROC_PID;
    name[3] = getpid();
    
    sysctl(name, 4, &proc, &size, NULL, 0);
    
    return proc.kp_proc.p_flag & P_TRACED;
}

@implementation AutoAntiDebug

+ (void)load {
    // 检测是否被调试
    bool isTracing = checkTracing();
    isTracing ? [AutoAntiDebug ptraceApp] : nil;
}

// exit 退出
+ (void)exitApp {
    NSLog(@"检测到调试");
    exit(-1);
}

// ptrace 退出
+ (void)ptraceApp {
    NSLog(@"检测到调试");
    void* handle = dlopen(0, RTLD_GLOBAL | RTLD_NOW);
    ptrace_ptr_t ptrace_ptr = dlsym(handle, "ptrace");
    ptrace_ptr(31, 0, 0, 0);
    dlclose(handle);
}

@end
