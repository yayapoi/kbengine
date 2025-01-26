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
 * @brief ���ݰ���
 * 
 * ����̳��� MemoryStream �� RefCountable�����ڱ�ʾ����ͨ���е����ݰ���
 * ���ṩ�����ݰ��Ļ����������緢�͡����ա����õȣ���֧�� TCP �� UDP ���ִ���Э�顣
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
     * �������ʱ�Ĵ���
     * 
     * �÷����ڶ��󱻻���ʱ���ã������������ݰ�״̬��
     */
	virtual void onReclaimObject()
	{
		MemoryStream::onReclaimObject();
		resetPacket();
	}
	
	/**
     * ��ȡ����ռ�õ��ֽ���
     * 
     * @return ռ�õ��ֽ���
     */
	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(msgID_) + sizeof(isTCPPacket_) + sizeof(encrypted_) + sizeof(pBundle_)
		 + sizeof(sentSize);

		return MemoryStream::getPoolObjectBytes() + bytes;
	}

	/**
     * ��ȡ������ Bundle ����
     * 
     * @return ������ Bundle ����ָ��
     */
	Bundle* pBundle() const{ return pBundle_; }
	void pBundle(Bundle* v){ pBundle_ = v; }

	/**
     * �Ӷ˵�������ݰ�
     * 
     * @param ep �˵��������
     * @param pAddr ��ַ����ָ�루��ѡ��
     * @return ���ս��
     */
	virtual int recvFromEndPoint(EndPoint & ep, Address* pAddr = NULL) = 0;
    virtual bool empty() const { return length() == 0; }

	/**
     * �������ݰ�״̬
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
     * ��ȡ���ݰ��Ƿ����
     * 
     * @return ������ݰ��Ѽ����򷵻� true�����򷵻� false
     */
	bool encrypted() const { return encrypted_; }

	void encrypted(bool v) { encrypted_ = v; }

protected:
	MessageID msgID_; //  ��ϢID
	bool isTCPPacket_; //  �Ƿ�ΪTCP��
	bool encrypted_; //  �Ƿ����
	Bundle* pBundle_; //  ���ݰ�ָ��

public:
	uint32 sentSize; //  ���ʹ�С

};

typedef SmartPointer<Packet> PacketPtr;
}
}

#endif // KBE_SOCKETPACKET_H
