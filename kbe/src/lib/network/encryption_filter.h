// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_ENCRYPTION_FILTER_H
#define KBE_ENCRYPTION_FILTER_H

#include "network/packet_filter.h"

#ifdef USE_OPENSSL
#include "common/blowfish.h"
#endif

namespace KBEngine { 
namespace Network
{

// ������ܹ������ࡣ�����˼��ܺͽ��ܵĻ����ӿڡ����о���ļ��ܹ������඼Ӧ�̳��Ը��ࡣ
class EncryptionFilter : public PacketFilter
{
public:
	virtual ~EncryptionFilter() {}

	virtual void encrypt(Packet * pInPacket, Packet * pOutPacket) = 0; //  ���ܺ������������Packet���ܺ����
	virtual void decrypt(Packet * pInPacket, Packet * pOutPacket) = 0; //  ���ܺ������������Packet���ܺ����
};


// ���� Blowfish �����㷨�Ĺ�����
class BlowfishFilter : public EncryptionFilter, public KBEBlowfish
{
public:
	virtual ~BlowfishFilter();
	BlowfishFilter(const Key & key);	// ʹ��ָ������Կ��ʼ��
	BlowfishFilter();

	virtual Reason send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg); //  ���ͺ�������Packet���͵�ָ����Channel

	virtual Reason recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket); //  ���պ�������ָ����Channel����Packet

	void encrypt(Packet * pInPacket, Packet * pOutPacket); //  ���ܺ������������Packet���ܺ����
	void decrypt(Packet * pInPacket, Packet * pOutPacket); //  ���ܺ������������Packet���ܺ����
private:
	// ��ǰ��������ݰ�ָ��
	Packet * pPacket_;
	// ���ݰ�����
	Network::PacketLength packetLen_;
	// ����С
	uint8 padSize_;
};

typedef SmartPointer<BlowfishFilter> BlowfishFilterPtr;

// �������ʹ�����Ӧ�ļ��ܹ�����ʵ����
inline EncryptionFilter* createEncryptionFilter(int8 type, const std::string& datas)
{
	EncryptionFilter* pEncryptionFilter = NULL;
	switch(type)
	{
	case 1:
		pEncryptionFilter = new BlowfishFilter(datas);
		break;
	default:
		break;
	}

	return pEncryptionFilter;
}

}
}

#endif // KBE_ENCRYPTION_FILTER_H
