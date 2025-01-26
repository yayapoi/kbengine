// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_EVENT_DISPATCHER_H
#define KBE_EVENT_DISPATCHER_H

#include <map>
#include "common/tasks.h"
#include "common/timer.h"
#include "network/interfaces.h"
#include "network/common.h"

namespace KBEngine { 
namespace Network
{
class DispatcherCoupling;
class ErrorReporter;
class EventPoller;

class EventDispatcher
{
public:
	enum EVENT_DISPATCHER_STATUS
	{
		EVENT_DISPATCHER_STATUS_RUNNING = 0, //  �¼���������������
		EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING = 1, // �ȴ��жϴ���״̬
		EVENT_DISPATCHER_STATUS_BREAK_PROCESSING = 2 // �жϴ���״̬
	};

	EventDispatcher();
	virtual ~EventDispatcher();
	
	int  processOnce(bool shouldIdle = false); //  ����һ���¼������shouldIdleΪtrue����������״̬
	void processUntilBreak(); //  ����ֱ�������ж�

	// ����Ƿ����жϴ���״̬
	INLINE bool hasBreakProcessing() const;
	// ����Ƿ�ȴ��жϴ���
	INLINE bool waitingBreakProcessing() const;

	void breakProcessing(bool breakState = true); //  �����жϴ���״̬
	INLINE void setWaitBreakProcessing(); //  ���õȴ��жϴ���
	
	void addTask(Task * pTask); //  �������
	bool cancelTask(Task * pTask); //  ȡ������
	
	INLINE double maxWait() const; //  ��ȡ���ȴ�ʱ��
	INLINE void maxWait(double seconds); //  �������ȴ�ʱ��

	bool registerReadFileDescriptor(int fd, InputNotificationHandler * handler); //  ע����ļ�������
	bool deregisterReadFileDescriptor(int fd); //  ע�����ļ�������
	bool registerWriteFileDescriptor(int fd, OutputNotificationHandler * handler); //  ע��д�ļ�������
	bool deregisterWriteFileDescriptor(int fd); //  ע��д�ļ�������

	// ��Ӷ�ʱ��
	INLINE TimerHandle addTimer(int64 microseconds,
					TimerHandler * handler, void* arg = NULL);

	uint64 getSpareTime() const; //  ��ȡʣ��?����?ʱ��
	void clearSpareTime(); //  ���ʣ��?����?ʱ��

	// ��ȡ���󱨸���
	ErrorReporter & errorReporter()	{ return *pErrorReporter_; }

	// �����¼���ѯ��
	INLINE EventPoller* createPoller();
	// ��ȡ�¼���ѯ��
	EventPoller* pPoller(){ return pPoller_; }

	int processNetwork(bool shouldIdle); //  ���������¼�
private:
	TimerHandle addTimerCommon(int64 microseconds,
		TimerHandler * handler,
		void * arg,
		bool recurrent);

	void processTasks(); //  ��������
	void processTimers(); //  ����ʱ��
	void processStats(); //  ����ͳ����Ϣ
	
	double calculateWait() const; //  ����ȴ�ʱ��
	
protected:
	int8 breakProcessing_;	// �жϴ���״̬

	double maxWait_; //  ���ȴ�ʱ��
	uint32 numTimerCalls_; //  ��ʱ�����ô���
	
	// Statistics
	TimeStamp		accSpareTime_; //  ��¼�ۻ��Ŀ���ʱ��
	TimeStamp		oldSpareTime_; //  ��¼��һ�εĿ���ʱ��
	TimeStamp totSpareTime_;                               // �ܿ���ʱ��
    TimeStamp lastStatisticsGathered_;                     // �ϴ��ռ�ͳ����Ϣ��ʱ��

    Tasks* pTasks_;                                        // �����б�
    ErrorReporter * pErrorReporter_;                       // ���󱨸���
    Timers64* pTimers_;                                    // ��ʱ���б�
    EventPoller* pPoller_;                                 // �¼���ѯ��
};


}
}

#ifdef CODE_INLINE
#include "event_dispatcher.inl"
#endif
#endif // KBE_EVENT_DISPATCHER_H
