// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORKERROR_REPORTER_H
#define KBE_NETWORKERROR_REPORTER_H

#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/network_exception.h"

namespace KBEngine { 
namespace Network
{
class EventDispatcher;
class Address;

// 错误报告和计数结构体
struct ErrorReportAndCount {
    uint64 lastReportStamps; // 上次报告的时间戳
    uint64 lastRaisedStamps; // 上次触发错误的时间戳
    uint count;              // 错误计数
};

// 地址和错误字符串的配对类型
typedef std::pair< Address, std::string > AddressAndErrorString;

// 错误报告和计数的映射类型
typedef std::map< AddressAndErrorString, ErrorReportAndCount > ErrorsAndCounts;

// 错误报告类，继承自 TimerHandler
class ErrorReporter : public TimerHandler {
public:
    // 构造函数，接收一个 EventDispatcher 引用
    ErrorReporter(EventDispatcher & dispatcher);
    // 析构函数
    ~ErrorReporter();

    // 报告异常，可选地址、前缀和后缀
    void reportException(Reason reason, const Address & addr = Address::NONE,
        const char * prefix = NULL, const char* suffix = NULL);

    // 报告待处理的异常，可选择是否报告低于阈值的异常
    void reportPendingExceptions(bool reportBelowThreshold = false);

private:
    // 静态常量：最小报告间隔（毫秒）
    static const uint ERROR_REPORT_MIN_PERIOD_MS;
    // 静态常量：错误计数的最大生命周期（毫秒）
    static const uint ERROR_REPORT_COUNT_MAX_LIFETIME_MS;

    // 报告网络异常，可选前缀和后缀
    void reportException(const NetworkException & ne, 
        const char * prefix = NULL, const char* suffix = NULL);

    // 报告错误，接收地址和格式化字符串
    void reportError(const Address & address, const char* format, ...);

    // 添加报告，接收地址和错误字符串
    void addReport(const Address & address, const std::string & error);

    // 将地址和错误字符串转换为字符串
    static std::string addressErrorToString(
            const Address & address,
            const std::string & errorString);

    // 将地址、错误字符串、报告和计数以及当前时间戳转换为字符串
    static std::string addressErrorToString(
            const Address & address,
            const std::string & errorString,
            const ErrorReportAndCount & reportAndCount,
            const uint64 & now);

    // 处理超时事件的虚函数
    virtual void handleTimeout(TimerHandle handle, void * arg);

    // 报告限制定时器句柄
    TimerHandle reportLimitTimerHandle_;
    // 错误报告和计数的映射
    ErrorsAndCounts errorsAndCounts_;
};


}
}

#ifdef CODE_INLINE
#include "error_reporter.inl"
#endif
#endif // KBE_NETWORKERROR_REPORTER_H
