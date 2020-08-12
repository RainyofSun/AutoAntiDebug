//
//  AntiCrack.hpp
//  VideoDemo
//
//  Created by EGLS_BMAC on 2020/8/12.
//  Copyright © 2020 EGLS_BMAC. All rights reserved.
//

#ifndef AntiCrack_hpp
#define AntiCrack_hpp

#include <stdio.h>
#include <string.h>
#include <sys/sysctl.h>
#include <dispatch/dispatch.h>
#include <CoreFoundation/CFBundle.h>

typedef int (*ptrace_ptr_t)(int _request, pid_t _pid, caddr_t _addr, int _data);

/**
 * 汇编调用Exit 退出
 */
static __attribute__ ((always_inline)) void asm_exit() {
#ifdef __arm64__
    __asm__ (
             "mov x0, #0\n"
             "mov w16, #1\n"
             "svc #0x80\n"
             
             "mov x1, #0\n"
             "mov sp, x1\n"
             "mov x29, x1\n"
             "mov x30, x1\n"
             "ret"
             );
#endif
}

/**
* 汇编调用Ptrace 退出
*/
static __attribute__ ((always_inline)) void ptrace_asm_exit() {
#ifdef __arm64__
    __asm__ volatile (
                    "mov x0,#31\n"
                    "mov x1,#0\n"
                    "mov x2,#0\n"
                    "mov x3,#0\n"
                    "mov w16,#26\n" //26是ptrace
                    "svc #0x80" //0x80触发中断去找w16执行
                    );
#endif
}

/**
 * 调用系统Ptrace方法退出
 */
static void ptrace_exit() {
    void* handle = dlopen(0, RTLD_GLOBAL | RTLD_NOW);
    ptrace_ptr_t ptrace_ptr = dlsym(handle, "ptrace");
    ptrace_ptr(31, 0, 0, 0);
    dlclose(handle);
}

/**
 * 调用syscall 退出
 */
static void syscall_exit() {
    syscall(26,31,0,0);
}

/**
 * 根据BoundleId进行反调试
 */
static void detectDebug_boundleId(NSString *bundleId) {
    CFStringRef mainBundleId = CFBundleGetIdentifier(CFBundleGetMainBundle());
    CFStringRef temp = (__bridge CFStringRef)bundleId;
    if (CFStringCompare(mainBundleId, temp, 0) != 0) {
        printf("检测到bundleID有篡改\n");
        asm_exit();
    }
}

/**
 * 调用sysctl反调试
 */
static bool detectDebug_sysctl(void) __attribute__ ((always_inline));
bool detectDebug_sysctl(void) {
    size_t size = sizeof(struct kinfo_proc);
    struct kinfo_proc info;
    int ret, name[4];
    
    memset(&info, 0, sizeof(struct kinfo_proc));
    
    name[0] = CTL_KERN;
    name[1] = KERN_PROC;
    name[2] = KERN_PROC_PID;
    name[3] = getpid();
    
#if 0
    if ((ret = (sysctl(name, 4, &info, &size, NULL, 0)))) {
        return ret;
    }
#else
#ifdef __arm64__
    __asm__ volatile(
                    "mov x0, %[name_ptr]\n"
                    "mov x1, #4\n"
                    "mov x2, %[info_ptr]\n"
                    "mov x3, %[size_ptr]\n"
                    "mov x4, #0\n"
                    "mov x5, #0\n"
                    "mov w16, #202\n"
                    "svc #0x80"
                    :
                    : [name_ptr] "r"(name), [info_ptr] "r"(&info),
                    [size_ptr] "r"(&size)
                    );
#endif
#endif
    return (info.kp_proc.p_flag & P_TRACED) ? 1 : 0;
}

#endif /* AntiCrack_hpp */
