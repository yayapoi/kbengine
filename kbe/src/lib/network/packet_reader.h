// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_PACKET_READER_H
#define KBE_PACKET_READER_H

#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "network/common.h"

namespace KBEngine{
namespace Network
{
class Channel;
class MessageHandlers;

/**
 * @brief PacketReader �����ڴ���������Ķ�ȡ�ͽ�����
 * 
 * PacketReader �ฺ�������ͨ���ж�ȡ���ݰ������������Ϊ��Ϣ����֧�ֶ������͵����紫��Э�飬
 * ���� Socket��WebSocket �� KCP��
 */
class PacketReader
{
public:
	enum PACKET_READER_TYPE //  ����PacketReader����
	{
		PACKET_READER_TYPE_SOCKET = 0, //  SOCKET����
		PACKET_READER_TYPE_WEBSOCKET = 1, //  WEBSOCKET����
		PACKET_READER_TYPE_KCP = 2 //  KCP����
	};

	PacketReader(Channel* pChannel);
	virtual ~PacketReader();

	virtual void reset();
	
	/**
     * @brief ������յ�����Ϣ��
     * 
     * @param pMsgHandlers ָ�� MessageHandlers �����ָ�룬���ڴ�����Ϣ��
     * @param pPacket ָ�� Packet �����ָ�룬�������յ������ݰ���
     */
	virtual void processMessages(KBEngine::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);
	
	/**
     * @brief ��ȡ��ǰ��Ϣ�� ID��
     * 
     * @return ��ǰ��Ϣ�� ID��
     */
	Network::MessageID	currMsgID() const{return currMsgID_;}
	/**
     * @brief ��ȡ��ǰ��Ϣ�ĳ��ȡ�
     * 
     * @return ��ǰ��Ϣ�ĳ��ȡ�
     */
	Network::MessageLength	currMsgLen() const{return currMsgLen_;}
	
	/**
     * @brief ���õ�ǰ��Ϣ�� ID��
     * 
     * @param id ��ǰ��Ϣ�� ID��
     */
	void currMsgID(Network::MessageID id){currMsgID_ = id;}
	void currMsgLen(Network::MessageLength len){currMsgLen_ = len;}

	virtual PacketReader::PACKET_READER_TYPE type()const { return PACKET_READER_TYPE_SOCKET; }


protected:
	enum FragmentDataTypes
	{
		FRAGMENT_DATA_UNKNOW, //  δ֪
		FRAGMENT_DATA_MESSAGE_ID, //  ��ϢID
		FRAGMENT_DATA_MESSAGE_LENGTH, //  ��Ϣ����
		FRAGMENT_DATA_MESSAGE_LENGTH1,	// ��Ϣ����Ƭ�Σ���չ��
		FRAGMENT_DATA_MESSAGE_BODY	// ��Ϣ��Ƭ��
	};
	
	/**
     * @brief д��Ƭ�����ݵ��ڴ����С�
     * 
     * @param fragmentDatasFlag Ƭ���������͡�
     * @param pPacket ָ�� Packet �����ָ�룬����Ƭ�����ݡ�
     * @param datasize Ƭ�����ݵĴ�С��
     */
	virtual void writeFragmentMessage(FragmentDataTypes fragmentDatasFlag, Packet* pPacket, uint32 datasize);
	/**
     * @brief �ϲ�Ƭ�����ݵ���������Ϣ��
     * 
     * @param pPacket ָ�� Packet �����ָ�룬����Ƭ�����ݡ�
     */
	virtual void mergeFragmentMessage(Packet* pPacket);

protected:
	uint8*						pFragmentDatas_;	// ָ��Ƭ�����ݵ�ָ�롣
	uint32						pFragmentDatasWpos_;	// Ƭ�����ݵ�дλ��
	uint32						pFragmentDatasRemain_;	// ʣ���Ƭ�����ݴ�С��
	FragmentDataTypes			fragmentDatasFlag_;	// ��ǰƬ���������͡�
	MemoryStream*				pFragmentStream_;	// ָ���ڴ��������ָ�룬���ڴ洢Ƭ������

	Network::MessageID			currMsgID_; //  ��ǰ��ϢID
	Network::MessageLength1		currMsgLen_; //  ��ǰ��Ϣ����
	
	Channel*					pChannel_;
};


}
}
#endif 
