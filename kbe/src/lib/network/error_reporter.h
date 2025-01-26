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

// ���󱨸�ͼ����ṹ��
struct ErrorReportAndCount {
    uint64 lastReportStamps; // �ϴα����ʱ���
    uint64 lastRaisedStamps; // �ϴδ��������ʱ���
    uint count;              // �������
};

// ��ַ�ʹ����ַ������������
typedef std::pair< Address, std::string > AddressAndErrorString;

// ���󱨸�ͼ�����ӳ������
typedef std::map< AddressAndErrorString, ErrorReportAndCount > ErrorsAndCounts;

// ���󱨸��࣬�̳��� TimerHandler
class ErrorReporter : public TimerHandler {
public:
    // ���캯��������һ�� EventDispatcher ����
    ErrorReporter(EventDispatcher & dispatcher);
    // ��������
    ~ErrorReporter();

    // �����쳣����ѡ��ַ��ǰ׺�ͺ�׺
    void reportException(Reason reason, const Address & addr = Address::NONE,
        const char * prefix = NULL, const char* suffix = NULL);

    // �����������쳣����ѡ���Ƿ񱨸������ֵ���쳣
    void reportPendingExceptions(bool reportBelowThreshold = false);

private:
    // ��̬��������С�����������룩
    static const uint ERROR_REPORT_MIN_PERIOD_MS;
    // ��̬�������������������������ڣ����룩
    static const uint ERROR_REPORT_COUNT_MAX_LIFETIME_MS;

    // ���������쳣����ѡǰ׺�ͺ�׺
    void reportException(const NetworkException & ne, 
        const char * prefix = NULL, const char* suffix = NULL);

    // ������󣬽��յ�ַ�͸�ʽ���ַ���
    void reportError(const Address & address, const char* format, ...);

    // ��ӱ��棬���յ�ַ�ʹ����ַ���
    void addReport(const Address & address, const std::string & error);

    // ����ַ�ʹ����ַ���ת��Ϊ�ַ���
    static std::string addressErrorToString(
            const Address & address,
            const std::string & errorString);

    // ����ַ�������ַ���������ͼ����Լ���ǰʱ���ת��Ϊ�ַ���
    static std::string addressErrorToString(
            const Address & address,
            const std::string & errorString,
            const ErrorReportAndCount & reportAndCount,
            const uint64 & now);

    // ����ʱ�¼����麯��
    virtual void handleTimeout(TimerHandle handle, void * arg);

    // �������ƶ�ʱ�����
    TimerHandle reportLimitTimerHandle_;
    // ���󱨸�ͼ�����ӳ��
    ErrorsAndCounts errorsAndCounts_;
};


}
}

#ifdef CODE_INLINE
#include "error_reporter.inl"
#endif
#endif // KBE_NETWORKERROR_REPORTER_H
