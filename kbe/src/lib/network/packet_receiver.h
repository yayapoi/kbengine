// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORKPACKET_RECEIVER_H
#define KBE_NETWORKPACKET_RECEIVER_H

#include "common/common.h"
#include "common/objectpool.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "network/tcp_packet.h"

namespace KBEngine { 
namespace Network
{
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

class PacketReceiver : public InputNotificationHandler, public PoolObject
{
public:
	enum RecvState
	{
		RECV_STATE_INTERRUPT = -1, //  中断接收
		RECV_STATE_BREAK = 0, //  结束接收
		RECV_STATE_CONTINUE = 1 //  继续接收
	};

	enum PACKET_RECEIVER_TYPE
	{
		TCP_PACKET_RECEIVER = 0, //  TCP接收器
		UDP_PACKET_RECEIVER = 1 //  UDP接收器
	};

public:
	PacketReceiver();
	PacketReceiver(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~PacketReceiver();

	virtual Reason processPacket(Channel* pChannel, Packet * pPacket); //  虚函数，用于处理接收到的数据包
	virtual Reason processFilteredPacket(Channel* pChannel, Packet * pPacket) = 0; //  虚函数，用于处理过滤后的数据包
	EventDispatcher& dispatcher(); //  获取事件调度器

	// 回收对象时调用
	void onReclaimObject()
	{
		pEndpoint_ = NULL;
		pChannel_ = NULL;
	}

	virtual PacketReceiver::PACKET_RECEIVER_TYPE type() const
	{
		return TCP_PACKET_RECEIVER;
	}

	// 设置端点
	void pEndPoint(EndPoint* pEndpoint) { 
		pEndpoint_ = pEndpoint; 
		pChannel_ = NULL;
	}

	EndPoint* pEndPoint() const { 
		return pEndpoint_; 
	}

	virtual int handleInputNotification(int fd); //  处理输入通知

	virtual Channel* getChannel();

protected:
	virtual bool processRecv(bool expectingPacket) = 0; //  处理接收数据
	virtual RecvState checkSocketErrors(int len, bool expectingPacket) = 0; //  检查Socket错误

protected:
	EndPoint* pEndpoint_;
	Channel* pChannel_;
	NetworkInterface* pNetworkInterface_; //  网络接口指针
};

}
}

#ifdef CODE_INLINE
#include "packet_receiver.inl"
#endif
#endif // KBE_NETWORKPACKET_RECEIVER_H
