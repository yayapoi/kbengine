// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORKPACKET_SENDER_H
#define KBE_NETWORKPACKET_SENDER_H

#include "common/common.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"

namespace KBEngine { 
namespace Network
{
class Packet;
class EndPoint;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

/**
 * @class PacketSender
 * 
 * @brief 数据包发送器类
 * 
 * 该类负责处理数据包的发送操作。它继承自 OutputNotificationHandler 和 PoolObject，
 * 提供了TCP和UDP数据包的发送功能，并支持对象池管理以提高性能。
 */
class PacketSender : public OutputNotificationHandler, public PoolObject
{
public:
	enum PACKET_SENDER_TYPE
	{
		TCP_PACKET_SENDER = 0, // TCP数据包发送器
		UDP_PACKET_SENDER = 1
	};

public:
	PacketSender();
	PacketSender(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~PacketSender();

	// 获取事件调度器
	EventDispatcher& dispatcher();

	//对象回收时的处理
	void onReclaimObject()
	{
		pEndpoint_ = NULL;
		pChannel_ = NULL;
		pNetworkInterface_ = NULL;
	}

	// 设置端点对象
	void pEndPoint(EndPoint* pEndpoint) {
		pChannel_ = NULL;
		pEndpoint_ = pEndpoint; 
	}

	EndPoint* pEndPoint() const { 
		return pEndpoint_; 
	}

	// 获取网络接口对象
	NetworkInterface* pNetworkInterface() const
	{
		return pNetworkInterface_;
	}

	void pNetworkInterface(NetworkInterface* v)
	{
		pNetworkInterface_ = v;
	}

	// 获取数据包发送器类型
	virtual PACKET_SENDER_TYPE type() const
	{
		return TCP_PACKET_SENDER;
	}

	// 处理输出通知
	virtual int handleOutputNotification(int fd);

	/**
     * 处理数据包
     * 
     * @param pChannel 通道对象指针
     * @param pPacket 数据包对象指针
     * @param userarg 用户参数
     * @return 处理结果
     */
	virtual Reason processPacket(Channel* pChannel, Packet * pPacket, int userarg);

	/**
     * 处理过滤后的数据包
     * 
     * @param pChannel 通道对象指针
     * @param pPacket 数据包对象指针
     * @param userarg 用户参数
     * @return 处理结果
     */
	virtual Reason processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg) = 0;

	 /**
     * 检查套接字错误
     * 
     * @param pEndpoint 端点对象指针
     * @return 错误原因
     */
	static Reason checkSocketErrors(const EndPoint * pEndpoint);

	virtual Channel* getChannel();

	 /**
     * 处理发送操作
     * 
     * @param pChannel 通道对象指针
     * @param userarg 用户参数
     * @return 是否成功
     */
	virtual bool processSend(Channel* pChannel, int userarg) = 0;

protected:
	EndPoint* pEndpoint_;
	Channel* pChannel_;
	// 网络接口对象指针
	NetworkInterface* pNetworkInterface_;
};

}
}

#ifdef CODE_INLINE
#include "packet_sender.inl"
#endif
#endif // KBE_NETWORKPACKET_SENDER_H
