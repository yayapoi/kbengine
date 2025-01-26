// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_BUNDLE_BROADCAST_H
#define KBE_BUNDLE_BROADCAST_H

#include "common/common.h"
#include "helper/debug_helper.h"
#include "network/common.h"
#include "network/interfaces.h"
#include "message_handler.h"
#include "network/bundle.h"
#include "network/endpoint.h"

namespace KBEngine { 
namespace Network
{
class NetworkInterface;

/*
	���Է���Ĵ�����:��������ڹ㲥ĳЩ��Ϣ�� �������ռ������Ϣ��
	ͨ�� UDP Э����й㲥ͨ�ţ����ṩ�˷��ͺͽ��չ㲥��Ϣ�Ĺ���
*/
class BundleBroadcast : public Bundle
{
public:
	BundleBroadcast(NetworkInterface & networkInterface, uint16 bindPort = KBE_PORT_BROADCAST_DISCOVERY, 
		uint32 recvWindowSize = PACKET_MAX_SIZE_UDP);
	virtual ~BundleBroadcast();

	EventDispatcher& dispatcher(); //  �����¼�������
	
	bool broadcast(uint16 port = 0); //  �㲥��Ϣ
	bool receive(MessageArgs* recvArgs, sockaddr_in* psin = NULL, int32 timeout = 100000, bool showerr = true); //  ������Ϣ

	// ��ȡ�����˵�
	Network::EndPoint& epListen() { 
		return epListen_; 
	}

	void close();

	bool good() const { 
		return epListen_.good() && good_; 
	}

	// �������Դ���
	void itry(int8 i){ 
		itry_ = i; 
	}

	void addBroadCastAddress(std::string addr); //  ��ӹ㲥��ַ

protected:
	// �����˵�
    Network::EndPoint epListen_;
    // �㲥�˵�
    Network::EndPoint epBroadcast_;
	NetworkInterface & networkInterface_; //  ����ӿ�
	uint32 recvWindowSize_; //  ���մ��ڴ�С
	bool good_; //  �����Ƿ�����
	int8 itry_; //  ���Դ���
	std::vector< std::string > machine_addresses_; //  ������ַ�б�
};

}
}

#ifdef CODE_INLINE
#include "bundle_broadcast.inl"
#endif
#endif // KBE_BUNDLE_BROADCAST_H
