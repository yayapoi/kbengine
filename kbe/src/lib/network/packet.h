/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 kbegine Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
*/
#ifndef KBE_SOCKETPACKET_H
#define KBE_SOCKETPACKET_H
	
// common include
#include "common/memorystream.h"
#include "common/common.h"
#include "common/objectpool.h"
#include "common/smartpointer.h"	
#include "network/common.h"
	
namespace KBEngine{
namespace Network
{
class EndPoint;
class Address;
class Bundle;

/**
 * @class Packet
 * 
 * @brief 数据包类
 * 
 * 该类继承自 MemoryStream 和 RefCountable，用于表示网络通信中的数据包。
 * 它提供了数据包的基本操作，如发送、接收、重置等，并支持 TCP 和 UDP 两种传输协议。
 */
class Packet : public MemoryStream, public RefCountable
{
public:
	Packet(MessageID msgID = 0, bool isTCPPacket = true, size_t res = 200):
	MemoryStream(res),
	msgID_(msgID),
	isTCPPacket_(isTCPPacket),
	encrypted_(false),
	pBundle_(NULL),
	sentSize(0)
	{
	};
	
	virtual ~Packet(void)
	{
	};
	
	/**
     * 对象回收时的处理
     * 
     * 该方法在对象被回收时调用，用于重置数据包状态。
     */
	virtual void onReclaimObject()
	{
		MemoryStream::onReclaimObject();
		resetPacket();
	}
	
	/**
     * 获取对象占用的字节数
     * 
     * @return 占用的字节数
     */
	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(msgID_) + sizeof(isTCPPacket_) + sizeof(encrypted_) + sizeof(pBundle_)
		 + sizeof(sentSize);

		return MemoryStream::getPoolObjectBytes() + bytes;
	}

	/**
     * 获取关联的 Bundle 对象
     * 
     * @return 关联的 Bundle 对象指针
     */
	Bundle* pBundle() const{ return pBundle_; }
	void pBundle(Bundle* v){ pBundle_ = v; }

	/**
     * 从端点接收数据包
     * 
     * @param ep 端点对象引用
     * @param pAddr 地址对象指针（可选）
     * @return 接收结果
     */
	virtual int recvFromEndPoint(EndPoint & ep, Address* pAddr = NULL) = 0;
    virtual bool empty() const { return length() == 0; }

	/**
     * 重置数据包状态
     */
	void resetPacket(void)
	{
		wpos(0);
		rpos(0);
		encrypted_ = false;
		sentSize = 0;
		msgID_ = 0;
		pBundle_ = NULL;
		// memset(data(), 0, size());
	};
	
	inline void messageID(MessageID msgID) { 
		msgID_ = msgID; 
	}

	inline MessageID messageID() const { return msgID_; }

	void isTCPPacket(bool v) { isTCPPacket_ = v; }
	bool isTCPPacket() const { return isTCPPacket_; }

	/**
     * 获取数据包是否加密
     * 
     * @return 如果数据包已加密则返回 true，否则返回 false
     */
	bool encrypted() const { return encrypted_; }

	void encrypted(bool v) { encrypted_ = v; }

protected:
	MessageID msgID_; //  消息ID
	bool isTCPPacket_; //  是否为TCP包
	bool encrypted_; //  是否加密
	Bundle* pBundle_; //  数据包指针

public:
	uint32 sentSize; //  发送大小

};

typedef SmartPointer<Packet> PacketPtr;
}
}

#endif // KBE_SOCKETPACKET_H
