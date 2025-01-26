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

//һ������ӿ��࣬���ڴ��������¼���ͨ���������̳��� TimerHandler�������������Ա�����ͳ�Ա�������������ú͹��������������ͨ���Լ���ʱ��
class NetworkInterface : public TimerHandler
{
public:
	typedef std::map<Address, Channel *>	ChannelMap;
	
	// ���캯������ʼ������ӿ�
	NetworkInterface(EventDispatcher * pDispatcher,
		int32 extlisteningTcpPort_min = -1, int32 extlisteningTcpPort_max = -1, int32 extlisteningUdpPort_min = -1, int32 extlisteningUdpPort_max = -1, const char * extlisteningInterface = "",
		uint32 extrbuffer = 0, uint32 extwbuffer = 0, 
		int32 intlisteningPort_min = 0, int32 intlisteningPort_max = 0, const char * intlisteningInterface = "",
		uint32 intrbuffer = 0, uint32 intwbuffer = 0);

	~NetworkInterface();

	INLINE const Address & extTcpAddr() const; //  �����ⲿTCP��ַ
	INLINE const Address & extUdpAddr() const; //  �����ⲿUDP��ַ
	INLINE const Address & intTcpAddr() const; //  �����ڲ�TCP��ַ

	bool initialize(const char* pEndPointName, uint16 listeningPort_min, uint16 listeningPort_max,
		const char * listeningInterface, EndPoint* pEP, ListenerReceiver* pLR, uint32 rbuffer = 0, uint32 wbuffer = 0);

	bool registerChannel(Channel* pChannel);
	bool deregisterChannel(Channel* pChannel); //  ע��Channel����
	bool deregisterAllChannels(); //  ע������Channel����
	Channel * findChannel(const Address & addr); //  ���ݵ�ַ����Channel����
	Channel * findChannel(int fd); //  �����ļ�����������Channel����

	// ��ȡͨ����ʱ������
	ChannelTimeOutHandler * pChannelTimeOutHandler() const
		{ return pChannelTimeOutHandler_; }
	// ����ͨ����ʱ������
	void pChannelTimeOutHandler(ChannelTimeOutHandler * pHandler)
		{ pChannelTimeOutHandler_ = pHandler; }
		
	// ��ȡͨ��ע��������
	ChannelDeregisterHandler * pChannelDeregisterHandler() const
		{ return pChannelDeregisterHandler_; }
	// ����ͨ��ע��������
	void pChannelDeregisterHandler(ChannelDeregisterHandler * pHandler)
		{ pChannelDeregisterHandler_ = pHandler; }

	// ��ȡ�¼��ַ���
	EventDispatcher & dispatcher()		{ return *pDispatcher_; }

	/* �ⲿ������ڲ����� */
	EndPoint & extEndpoint()				{ return extTcpEndpoint_; }
	EndPoint & intEndpoint()				{ return intTcpEndpoint_; }

	// ��ȡ�ַ�����ʾ
	const char * c_str() const { return extTcpEndpoint_.c_str(); }
	
	// ��ȡͨ��ӳ��
	const ChannelMap& channels(void) { return channelMap_; }
		
	/** �������,�������ݣ�����ӳ٣�*/
	void sendIfDelayed(Channel & channel);
	// �������,�ӳٷ�������
	void delayedSend(Channel & channel);
	
	// �������ӿ��Ƿ�����
	bool good() const{ return (!pExtListenerReceiver_ || extTcpEndpoint_.good()) && (intTcpEndpoint_.good()); }

	// ����ͨ����ʱ
	void onChannelTimeOut(Channel * pChannel);
	
	/* 
		��������channels  
	*/
	void processChannels(KBEngine::Network::MessageHandlers* pMsgHandlers);

	// ��ȡ�ⲿͨ������
	INLINE int32 numExtChannels() const;

private:
	// ����ʱ�¼�
	virtual void handleTimeout(TimerHandle handle, void * arg);

	void closeSocket();

private:
	EndPoint								extTcpEndpoint_, extUdpEndpoint_, intTcpEndpoint_;	// �ⲿ������ڲ�����

	ChannelMap								channelMap_;	// ͨ��ӳ��

	EventDispatcher *						pDispatcher_;	// �¼��ַ���ָ��
	
	ListenerReceiver *						pExtListenerReceiver_;	// �ⲿTCP����������
	ListenerReceiver *						pExtUdpListenerReceiver_;	// �ⲿUDP����������
	ListenerReceiver *						pIntListenerReceiver_;	// �ڲ�����������
	
	DelayedChannels * 						pDelayedChannels_;	// �ӳ�ͨ��ָ��
	
	ChannelTimeOutHandler *					pChannelTimeOutHandler_;	// ��ʱ��ͨ���ɱ���������׽�� �����֪�ϲ�client�Ͽ�
	ChannelDeregisterHandler *				pChannelDeregisterHandler_;	// ͨ��ע��������ָ��

	int32									numExtChannels_;	// �ⲿͨ������
};

}
}

#ifdef CODE_INLINE
#include "network_interface.inl"
#endif
#endif // KBE_NETWORK_INTERFACE_H
