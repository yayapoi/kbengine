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
		EVENT_DISPATCHER_STATUS_RUNNING = 0, //  事件调度器正在运行
		EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING = 1, // 等待中断处理状态
		EVENT_DISPATCHER_STATUS_BREAK_PROCESSING = 2 // 中断处理状态
	};

	EventDispatcher();
	virtual ~EventDispatcher();
	
	int  processOnce(bool shouldIdle = false); //  处理一次事件，如果shouldIdle为true，则进入空闲状态
	void processUntilBreak(); //  处理直到遇到中断

	// 检查是否处于中断处理状态
	INLINE bool hasBreakProcessing() const;
	// 检查是否等待中断处理
	INLINE bool waitingBreakProcessing() const;

	void breakProcessing(bool breakState = true); //  设置中断处理状态
	INLINE void setWaitBreakProcessing(); //  设置等待中断处理
	
	void addTask(Task * pTask); //  添加任务
	bool cancelTask(Task * pTask); //  取消任务
	
	INLINE double maxWait() const; //  获取最大等待时间
	INLINE void maxWait(double seconds); //  设置最大等待时间

	bool registerReadFileDescriptor(int fd, InputNotificationHandler * handler); //  注册读文件描述符
	bool deregisterReadFileDescriptor(int fd); //  注销读文件描述符
	bool registerWriteFileDescriptor(int fd, OutputNotificationHandler * handler); //  注册写文件描述符
	bool deregisterWriteFileDescriptor(int fd); //  注销写文件描述符

	// 添加定时器
	INLINE TimerHandle addTimer(int64 microseconds,
					TimerHandler * handler, void* arg = NULL);

	uint64 getSpareTime() const; //  获取剩余?空闲?时间
	void clearSpareTime(); //  清除剩余?空闲?时间

	// 获取错误报告器
	ErrorReporter & errorReporter()	{ return *pErrorReporter_; }

	// 创建事件轮询器
	INLINE EventPoller* createPoller();
	// 获取事件轮询器
	EventPoller* pPoller(){ return pPoller_; }

	int processNetwork(bool shouldIdle); //  处理网络事件
private:
	TimerHandle addTimerCommon(int64 microseconds,
		TimerHandler * handler,
		void * arg,
		bool recurrent);

	void processTasks(); //  处理任务
	void processTimers(); //  处理定时器
	void processStats(); //  处理统计信息
	
	double calculateWait() const; //  计算等待时间
	
protected:
	int8 breakProcessing_;	// 中断处理状态

	double maxWait_; //  最大等待时间
	uint32 numTimerCalls_; //  定时器调用次数
	
	// Statistics
	TimeStamp		accSpareTime_; //  记录累积的空闲时间
	TimeStamp		oldSpareTime_; //  记录上一次的空闲时间
	TimeStamp totSpareTime_;                               // 总空闲时间
    TimeStamp lastStatisticsGathered_;                     // 上次收集统计信息的时间

    Tasks* pTasks_;                                        // 任务列表
    ErrorReporter * pErrorReporter_;                       // 错误报告器
    Timers64* pTimers_;                                    // 定时器列表
    EventPoller* pPoller_;                                 // 事件轮询器
};


}
}

#ifdef CODE_INLINE
#include "event_dispatcher.inl"
#endif
#endif // KBE_EVENT_DISPATCHER_H
