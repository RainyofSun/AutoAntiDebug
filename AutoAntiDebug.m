//
//  AutoAntiDebug.m
//  TEstDemo
//
//  Created by EGLS_BMAC on 2020/8/11.
//  Copyright © 2020 EGLS_BMAC. All rights reserved.
//

#import "AutoAntiDebug.h"
#include "AntiCrack.hpp"

@implementation AutoAntiDebug

+ (void)load {
    // 检测是否被调试
    bool isTracing = detectDebug_sysctl();
    isTracing ? [AutoAntiDebug exitApp] : nil;
}

// exit 退出
+ (void)exitApp {
    NSLog(@"检测到调试");
//    asm_exit();
//    ptrace_exit();
    syscall_exit();
}

@end
