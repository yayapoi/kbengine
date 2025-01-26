// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_WEBSOCKET_PACKET_READER_H
#define KBE_WEBSOCKET_PACKET_READER_H

#include "network/packet_reader.h"

namespace KBEngine{
namespace Network
{

/**
 * @class WebSocketPacketReader
 * 
 * @brief WebSocket 数据包读取器类
 * 
 * 该类继承自 PacketReader，专门用于处理 WebSocket 协议的数据包。
 * 它负责从通道中读取数据包，并解析成消息，然后调用消息处理器进行处理。
 */
class WebSocketPacketReader : public PacketReader
{
public:
	WebSocketPacketReader(Channel* pChannel);
	virtual ~WebSocketPacketReader();

	virtual void reset();
	
	/**
     * 处理消息
     * 
     * 该方法从数据包中解析出消息，并调用消息处理器进行处理。
     * 
     * @param pMsgHandlers 消息处理器指针
     * @param pPacket 数据包指针
     */
	virtual void processMessages(KBEngine::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);

	virtual PacketReader::PACKET_READER_TYPE type() const { return PACKET_READER_TYPE_WEBSOCKET; } // 返回包读取器的类型，这里返回的是 PACKET_READER_TYPE_WEBSOCKET

protected:
};


}
}
#endif 
