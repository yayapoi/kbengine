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
 * @brief PacketReader 类用于处理网络包的读取和解析。
 * 
 * PacketReader 类负责从网络通道中读取数据包，并将其解析为消息。它支持多种类型的网络传输协议，
 * 包括 Socket、WebSocket 和 KCP。
 */
class PacketReader
{
public:
	enum PACKET_READER_TYPE //  定义PacketReader类型
	{
		PACKET_READER_TYPE_SOCKET = 0, //  SOCKET类型
		PACKET_READER_TYPE_WEBSOCKET = 1, //  WEBSOCKET类型
		PACKET_READER_TYPE_KCP = 2 //  KCP类型
	};

	PacketReader(Channel* pChannel);
	virtual ~PacketReader();

	virtual void reset();
	
	/**
     * @brief 处理接收到的消息。
     * 
     * @param pMsgHandlers 指向 MessageHandlers 对象的指针，用于处理消息。
     * @param pPacket 指向 Packet 对象的指针，包含接收到的数据包。
     */
	virtual void processMessages(KBEngine::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);
	
	/**
     * @brief 获取当前消息的 ID。
     * 
     * @return 当前消息的 ID。
     */
	Network::MessageID	currMsgID() const{return currMsgID_;}
	/**
     * @brief 获取当前消息的长度。
     * 
     * @return 当前消息的长度。
     */
	Network::MessageLength	currMsgLen() const{return currMsgLen_;}
	
	/**
     * @brief 设置当前消息的 ID。
     * 
     * @param id 当前消息的 ID。
     */
	void currMsgID(Network::MessageID id){currMsgID_ = id;}
	void currMsgLen(Network::MessageLength len){currMsgLen_ = len;}

	virtual PacketReader::PACKET_READER_TYPE type()const { return PACKET_READER_TYPE_SOCKET; }


protected:
	enum FragmentDataTypes
	{
		FRAGMENT_DATA_UNKNOW, //  未知
		FRAGMENT_DATA_MESSAGE_ID, //  消息ID
		FRAGMENT_DATA_MESSAGE_LENGTH, //  消息长度
		FRAGMENT_DATA_MESSAGE_LENGTH1,	// 消息长度片段（扩展）
		FRAGMENT_DATA_MESSAGE_BODY	// 消息体片段
	};
	
	/**
     * @brief 写入片段数据到内存流中。
     * 
     * @param fragmentDatasFlag 片段数据类型。
     * @param pPacket 指向 Packet 对象的指针，包含片段数据。
     * @param datasize 片段数据的大小。
     */
	virtual void writeFragmentMessage(FragmentDataTypes fragmentDatasFlag, Packet* pPacket, uint32 datasize);
	/**
     * @brief 合并片段数据到完整的消息。
     * 
     * @param pPacket 指向 Packet 对象的指针，包含片段数据。
     */
	virtual void mergeFragmentMessage(Packet* pPacket);

protected:
	uint8*						pFragmentDatas_;	// 指向片段数据的指针。
	uint32						pFragmentDatasWpos_;	// 片段数据的写位置
	uint32						pFragmentDatasRemain_;	// 剩余的片段数据大小。
	FragmentDataTypes			fragmentDatasFlag_;	// 当前片段数据类型。
	MemoryStream*				pFragmentStream_;	// 指向内存流对象的指针，用于存储片段数据

	Network::MessageID			currMsgID_; //  当前消息ID
	Network::MessageLength1		currMsgLen_; //  当前消息长度
	
	Channel*					pChannel_;
};


}
}
#endif 
