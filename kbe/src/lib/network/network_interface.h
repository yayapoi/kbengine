// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORK_INTERFACE_H
#define KBE_NETWORK_INTERFACE_H

#include "common/memorystream.h"
#include "network/common.h"
#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/endpoint.h"

namespace KBEngine { 
namespace Network
{
class Address;
class Bundle;
class Channel;
class ChannelTimeOutHandler;
class ChannelDeregisterHandler;
class DelayedChannels;
class ListenerReceiver;
class Packet;
class EventDispatcher;
class MessageHandlers;

//一个网络接口类，用于处理网络事件和通道管理。它继承自 TimerHandler，并包含多个成员变量和成员函数，用于配置和管理网络监听器、通道以及定时器
class NetworkInterface : public TimerHandler
{
public:
	typedef std::map<Address, Channel *>	ChannelMap;
	
	// 构造函数，初始化网络接口
	NetworkInterface(EventDispatcher * pDispatcher,
		int32 extlisteningTcpPort_min = -1, int32 extlisteningTcpPort_max = -1, int32 extlisteningUdpPort_min = -1, int32 extlisteningUdpPort_max = -1, const char * extlisteningInterface = "",
		uint32 extrbuffer = 0, uint32 extwbuffer = 0, 
		int32 intlisteningPort_min = 0, int32 intlisteningPort_max = 0, const char * intlisteningInterface = "",
		uint32 intrbuffer = 0, uint32 intwbuffer = 0);

	~NetworkInterface();

	INLINE const Address & extTcpAddr() const; //  返回外部TCP地址
	INLINE const Address & extUdpAddr() const; //  返回外部UDP地址
	INLINE const Address & intTcpAddr() const; //  返回内部TCP地址

	bool initialize(const char* pEndPointName, uint16 listeningPort_min, uint16 listeningPort_max,
		const char * listeningInterface, EndPoint* pEP, ListenerReceiver* pLR, uint32 rbuffer = 0, uint32 wbuffer = 0);

	bool registerChannel(Channel* pChannel);
	bool deregisterChannel(Channel* pChannel); //  注销Channel对象
	bool deregisterAllChannels(); //  注销所有Channel对象
	Channel * findChannel(const Address & addr); //  根据地址查找Channel对象
	Channel * findChannel(int fd); //  根据文件描述符查找Channel对象

	// 获取通道超时处理器
	ChannelTimeOutHandler * pChannelTimeOutHandler() const
		{ return pChannelTimeOutHandler_; }
	// 设置通道超时处理器
	void pChannelTimeOutHandler(ChannelTimeOutHandler * pHandler)
		{ pChannelTimeOutHandler_ = pHandler; }
		
	// 获取通道注销处理器
	ChannelDeregisterHandler * pChannelDeregisterHandler() const
		{ return pChannelDeregisterHandler_; }
	// 设置通道注销处理器
	void pChannelDeregisterHandler(ChannelDeregisterHandler * pHandler)
		{ pChannelDeregisterHandler_ = pHandler; }

	// 获取事件分发器
	EventDispatcher & dispatcher()		{ return *pDispatcher_; }

	/* 外部网点和内部网点 */
	EndPoint & extEndpoint()				{ return extTcpEndpoint_; }
	EndPoint & intEndpoint()				{ return intTcpEndpoint_; }

	// 获取字符串表示
	const char * c_str() const { return extTcpEndpoint_.c_str(); }
	
	// 获取通道映射
	const ChannelMap& channels(void) { return channelMap_; }
		
	/** 发送相关,发送数据（如果延迟）*/
	void sendIfDelayed(Channel & channel);
	// 发送相关,延迟发送数据
	void delayedSend(Channel & channel);
	
	// 检查网络接口是否正常
	bool good() const{ return (!pExtListenerReceiver_ || extTcpEndpoint_.good()) && (intTcpEndpoint_.good()); }

	// 处理通道超时
	void onChannelTimeOut(Channel * pChannel);
	
	/* 
		处理所有channels  
	*/
	void processChannels(KBEngine::Network::MessageHandlers* pMsgHandlers);

	// 获取外部通道数量
	INLINE int32 numExtChannels() const;

private:
	// 处理超时事件
	virtual void handleTimeout(TimerHandle handle, void * arg);

	void closeSocket();

private:
	EndPoint								extTcpEndpoint_, extUdpEndpoint_, intTcpEndpoint_;	// 外部网点和内部网点

	ChannelMap								channelMap_;	// 通道映射

	EventDispatcher *						pDispatcher_;	// 事件分发器指针
	
	ListenerReceiver *						pExtListenerReceiver_;	// 外部TCP监听接收器
	ListenerReceiver *						pExtUdpListenerReceiver_;	// 外部UDP监听接收器
	ListenerReceiver *						pIntListenerReceiver_;	// 内部监听接收器
	
	DelayedChannels * 						pDelayedChannels_;	// 延迟通道指针
	
	ChannelTimeOutHandler *					pChannelTimeOutHandler_;	// 超时的通道可被这个句柄捕捉， 例如告知上层client断开
	ChannelDeregisterHandler *				pChannelDeregisterHandler_;	// 通道注销处理器指针

	int32									numExtChannels_;	// 外部通道数量
};

}
}

#ifdef CODE_INLINE
#include "network_interface.inl"
#endif
#endif // KBE_NETWORK_INTERFACE_H
