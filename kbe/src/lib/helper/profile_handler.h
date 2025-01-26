// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_PROFILE_HANDLER_H
#define KBE_PROFILE_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "helper/eventhistory_stats.h"
#include "network/interfaces.h"

namespace KBEngine { 
namespace Network
{
class NetworkInterface;
class Address;
class MessageHandler;
}

class MemoryStream;

// 定时器处理器，用于处理网络接口的性能分析。
class ProfileHandler : public TimerHandler
{
public:
	ProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~ProfileHandler();
	
	// 超时处理函数，需要子类实现
	virtual void timeout() = 0;
	// 发送数据流，需要子类实现
	virtual void sendStream(MemoryStream* s) = 0;

	static KBEUnordered_map<std::string, KBEShared_ptr< ProfileHandler > > profiles;

protected:
	/**
     * @brief 处理超时事件
     * @param handle 定时器句柄
     * @param arg 用户自定义参数
     */
    virtual void handleTimeout(TimerHandle handle, void * arg);

    /**
     * @brief 网络接口引用
     */
    Network::NetworkInterface& networkInterface_;

    /**
     * @brief 报告限制定时器句柄
     */
    TimerHandle reportLimitTimerHandle_;
    
    /**
     * @brief 处理器名称
     */
    std::string name_;
    
    /**
     * @brief 地址
     */
    Network::Address addr_;

    /**
     * @brief 定时器间隔时间（毫秒）
     */
    uint32 timinglen_;
};

// 成为任务的定时性能分析处理器，继承自 Task 和 ProfileHandler。
class CProfileHandler : public Task, 
						public ProfileHandler
{
public:
	CProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~CProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);
	bool process();

private:
	struct ProfileVal
	{
		// 名称
		std::string		name;

		// startd后的时间.
		TimeStamp		lastTime;
		TimeStamp		diff_lastTime;

		// count_次的总时间
		TimeStamp		sumTime;
		TimeStamp		diff_sumTime;

		// 记录最后一次内部时间片
		TimeStamp		lastIntTime;
		TimeStamp		diff_lastIntTime;

		// count_次内部总时间
		TimeStamp		sumIntTime;
		TimeStamp		diff_sumIntTime;

		uint32			count; //  计数器
		uint32			diff_count; //  差值计数器
	};

	// 此ProfileVal只在计时器开始时记录default.profiles的初始值
	// 在结束时取出差值得到结果
	typedef KBEUnordered_map<std::string,  ProfileVal> PROFILEVALS;
	PROFILEVALS profileVals_;
};

class EventProfileHandler : public ProfileHandler
{
public:
	EventProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~EventProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);

	// 触发事件时的处理函数
	void onTriggerEvent(const EventHistoryStats& eventHistory, const EventHistoryStats::Stats& stats, 
		uint32 size);

	// 触发事件的静态函数
	static void triggerEvent(const EventHistoryStats& eventHistory, const EventHistoryStats::Stats& stats, 
		uint32 size);

private:
	//存储性能分析值的结构体
	struct ProfileVal
	{
		ProfileVal()
		{
			name = "";
			size = 0;
			count = 0;
		}

		// 名称
		std::string		name;

		uint32			size;
		uint32			count;
	};

	// 此ProfileVal只在计时器开始时记录default.profiles的初始值
	// 在结束时取出差值得到结果
	typedef KBEUnordered_map<std::string,  ProfileVal> PROFILEVALS;

	typedef KBEUnordered_map< std::string,  PROFILEVALS > PROFILEVALMAP;
	PROFILEVALMAP profileMaps_;
	
	static std::vector<EventProfileHandler*> eventProfileHandlers_; //  事件处理程序列表
	int removeHandle_;
};

// 网络统计处理器
class NetworkProfileHandler : public ProfileHandler, public Network::NetworkStatsHandler
{
public:
	NetworkProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~NetworkProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);

	virtual void onSendMessage(const Network::MessageHandler& msgHandler, int size); //  发送消息时的处理函数
	virtual void onRecvMessage(const Network::MessageHandler& msgHandler, int size); //  接收消息时的处理函数

private:
	struct ProfileVal
	{
		ProfileVal()
		{
			name = ""; //  初始化name为空字符串
			send_size = 0; //  初始化发送大小为0
			send_count = 0; //  初始化发送次数为0
			send_avgsize = 0; //  初始化平均发送大小为0
			total_send_size = 0; //  初始化总发送大小为0
			total_send_count = 0; //  初始化总发送次数为0

			recv_size = 0;
			recv_count = 0;
			recv_avgsize = 0;
			total_recv_size = 0;
			total_recv_count = 0;
		}

		// 名称
		std::string		name;

		uint32			send_size; //  发送数据大小
		uint32			send_avgsize; //  发送数据平均大小
		uint32			send_count; //  发送数据次数

		uint32			total_send_size; //  总发送数据大小
		uint32			total_send_count; //  总发送数据次数

		uint32			recv_size; //  接收数据大小
		uint32			recv_count; //  接收数据次数
		uint32			recv_avgsize; //  接收数据平均大小

		uint32			total_recv_size; //  总接收数据大小
		uint32			total_recv_count; //  总接收数据次数
	};

	typedef KBEUnordered_map<std::string,  ProfileVal> PROFILEVALS;
	PROFILEVALS profileVals_;
};

}

#endif
