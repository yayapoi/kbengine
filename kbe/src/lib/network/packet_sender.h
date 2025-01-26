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
 * @brief ���ݰ���������
 * 
 * ���ฺ�������ݰ��ķ��Ͳ��������̳��� OutputNotificationHandler �� PoolObject��
 * �ṩ��TCP��UDP���ݰ��ķ��͹��ܣ���֧�ֶ���ع�����������ܡ�
 */
class PacketSender : public OutputNotificationHandler, public PoolObject
{
public:
	enum PACKET_SENDER_TYPE
	{
		TCP_PACKET_SENDER = 0, // TCP���ݰ�������
		UDP_PACKET_SENDER = 1
	};

public:
	PacketSender();
	PacketSender(EndPoint & endpoint, NetworkInterface & networkInterface);
	virtual ~PacketSender();

	// ��ȡ�¼�������
	EventDispatcher& dispatcher();

	//�������ʱ�Ĵ���
	void onReclaimObject()
	{
		pEndpoint_ = NULL;
		pChannel_ = NULL;
		pNetworkInterface_ = NULL;
	}

	// ���ö˵����
	void pEndPoint(EndPoint* pEndpoint) {
		pChannel_ = NULL;
		pEndpoint_ = pEndpoint; 
	}

	EndPoint* pEndPoint() const { 
		return pEndpoint_; 
	}

	// ��ȡ����ӿڶ���
	NetworkInterface* pNetworkInterface() const
	{
		return pNetworkInterface_;
	}

	void pNetworkInterface(NetworkInterface* v)
	{
		pNetworkInterface_ = v;
	}

	// ��ȡ���ݰ�����������
	virtual PACKET_SENDER_TYPE type() const
	{
		return TCP_PACKET_SENDER;
	}

	// �������֪ͨ
	virtual int handleOutputNotification(int fd);

	/**
     * �������ݰ�
     * 
     * @param pChannel ͨ������ָ��
     * @param pPacket ���ݰ�����ָ��
     * @param userarg �û�����
     * @return ������
     */
	virtual Reason processPacket(Channel* pChannel, Packet * pPacket, int userarg);

	/**
     * ������˺�����ݰ�
     * 
     * @param pChannel ͨ������ָ��
     * @param pPacket ���ݰ�����ָ��
     * @param userarg �û�����
     * @return ������
     */
	virtual Reason processFilterPacket(Channel* pChannel, Packet * pPacket, int userarg) = 0;

	 /**
     * ����׽��ִ���
     * 
     * @param pEndpoint �˵����ָ��
     * @return ����ԭ��
     */
	static Reason checkSocketErrors(const EndPoint * pEndpoint);

	virtual Channel* getChannel();

	 /**
     * �����Ͳ���
     * 
     * @param pChannel ͨ������ָ��
     * @param userarg �û�����
     * @return �Ƿ�ɹ�
     */
	virtual bool processSend(Channel* pChannel, int userarg) = 0;

protected:
	EndPoint* pEndpoint_;
	Channel* pChannel_;
	// ����ӿڶ���ָ��
	NetworkInterface* pNetworkInterface_;
};

}
}

#ifdef CODE_INLINE
#include "packet_sender.inl"
#endif
#endif // KBE_NETWORKPACKET_SENDER_H
