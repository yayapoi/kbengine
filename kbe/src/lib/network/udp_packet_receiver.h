// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORKUDPPACKET_RECEIVER_H
#define KBE_NETWORKUDPPACKET_RECEIVER_H

#include "common/common.h"
#include "common/timer.h"
#include "common/objectpool.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "network/udp_packet.h"
#include "network/packet_receiver.h"

namespace KBEngine { 
namespace Network
{
class Socket;
class Channel;
class Address;
class NetworkInterface;
class EventDispatcher;

// 用于处理 UDP 数据包接收的类，继承自 PacketReceiver 类。它提供了一些静态方法来创建和管理 UDP 数据包接收对象池，以及一些虚函数用于处理接收到的 UDP 数据包。
class UDPPacketReceiver : public PacketReceiver
{
public:
	typedef KBEShared_ptr< SmartPoolObject< UDPPacketReceiver > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<UDPPacketReceiver>& ObjPool();
	static UDPPacketReceiver* createPoolObject(const std::string& logPoint); //  创建一个对象池中的对象
	static void reclaimPoolObject(UDPPacketReceiver* obj); //  回收对象池中的对象
	static void destroyObjPool(); //  销毁对象池

	UDPPacketReceiver():PacketReceiver(){}
	UDPPacketReceiver(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~UDPPacketReceiver();

	Reason processFilteredPacket(Channel* pChannel, Packet * pPacket); //  处理过滤后的数据包
	
	// 获取接收器类型
	virtual PacketReceiver::PACKET_RECEIVER_TYPE type() const
	{
		return UDP_PACKET_RECEIVER;
	}

	// 获取协议子类型
	virtual ProtocolSubType protocolSubType() const {
		return SUB_PROTOCOL_UDP;
	}

	virtual bool processRecv(UDPPacket* pReceiveWindow); //  处理接收到的数据包
	virtual bool processRecv(bool expectingPacket);	// 处理接收到的数据包（带期望标志）

	virtual Channel* findChannel(const Address& addr);	// 查找通道

protected:
	PacketReceiver::RecvState checkSocketErrors(int len, bool expectingPacket); //  检查套接字错误

protected:

};

}
}

#ifdef CODE_INLINE
#include "udp_packet_receiver.inl"
#endif
#endif // KBE_NETWORKUDPPACKET_RECEIVER_H
