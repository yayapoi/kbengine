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

// 抽象加密过滤器类。定义了加密和解密的基本接口。所有具体的加密过滤器类都应继承自该类。
class EncryptionFilter : public PacketFilter
{
public:
	virtual ~EncryptionFilter() {}

	virtual void encrypt(Packet * pInPacket, Packet * pOutPacket) = 0; //  加密函数，将输入的Packet加密后输出
	virtual void decrypt(Packet * pInPacket, Packet * pOutPacket) = 0; //  解密函数，将输入的Packet解密后输出
};


// 用于 Blowfish 加密算法的过滤器
class BlowfishFilter : public EncryptionFilter, public KBEBlowfish
{
public:
	virtual ~BlowfishFilter();
	BlowfishFilter(const Key & key);	// 使用指定的密钥初始化
	BlowfishFilter();

	virtual Reason send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg); //  发送函数，将Packet发送到指定的Channel

	virtual Reason recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket); //  接收函数，从指定的Channel接收Packet

	void encrypt(Packet * pInPacket, Packet * pOutPacket); //  加密函数，将输入的Packet加密后输出
	void decrypt(Packet * pInPacket, Packet * pOutPacket); //  解密函数，将输入的Packet解密后输出
private:
	// 当前处理的数据包指针
	Packet * pPacket_;
	// 数据包长度
	Network::PacketLength packetLen_;
	// 填充大小
	uint8 padSize_;
};

typedef SmartPointer<BlowfishFilter> BlowfishFilterPtr;

// 根据类型创建相应的加密过滤器实例。
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
