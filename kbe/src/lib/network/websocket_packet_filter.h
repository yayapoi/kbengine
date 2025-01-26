// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_WEBSOCKET_PACKET_FILTER_H
#define KBE_WEBSOCKET_PACKET_FILTER_H

#include "network/packet_filter.h"
#include "network/websocket_protocol.h"

namespace KBEngine { 
namespace Network
{
class TCPPacket;

/**
 * @class WebSocketPacketFilter
 * 
 * @brief WebSocket 数据包过滤器类
 * 
 * 该类继承自 PacketFilter，专门用于处理 WebSocket 协议的数据包过滤。
 * 它负责在数据包发送和接收时进行必要的处理，包括分片管理、消息类型检查等。
 */
class WebSocketPacketFilter : public PacketFilter
{
public:
	WebSocketPacketFilter(Channel* pChannel);
	virtual ~WebSocketPacketFilter();

 	/**
     * 发送数据包
     * 
     * 该方法负责将数据包通过通道发送出去，并返回发送结果。
     * 
     * @param pChannel 通道指针
     * @param sender 数据包发送器
     * @param pPacket 待发送的数据包指针
     * @param userarg 用户自定义参数
     * @return 发送结果
     */
	virtual Reason send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg);

	/**
     * 接收数据包
     * 
     * 该方法负责从通道中接收数据包，并返回接收结果。
     * 
     * @param pChannel 通道指针
     * @param receiver 数据包接收器
     * @param pPacket 接收到的数据包指针
     * @return 接收结果
     */
	virtual Reason recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket);

protected:
	void reset();
	
	/**
     * 处理 Ping 消息
     * 
     * 该方法用于处理接收到的 Ping 消息。
     * 
     * @param pChannel 通道指针
     * @param pPacket 接收到的 Ping 消息数据包指针
     * @return 处理结果
     */
	Reason onPing(Channel * pChannel, Packet* pPacket);

protected:
	enum FragmentDataTypes //  定义数据包类型
	{
		FRAGMENT_MESSAGE_HREAD,	// 分片消息头
		FRAGMENT_MESSAGE_DATAS	// 分片消息数据
	};

	int32										pFragmentDatasRemain_; //  剩余的片段数据
	FragmentDataTypes							fragmentDatasFlag_; //  片段数据类型

	uint8										msg_opcode_; //  消息操作码
	uint8										msg_fin_; //  消息是否结束
	uint8										msg_masked_; //  消息是否被掩码
	uint32										msg_mask_; //  消息掩码
	int32										msg_length_field_; //  消息长度字段
	uint64										msg_payload_length_; //  消息负载长度
	websocket::WebSocketProtocol::FrameType		msg_frameType_; //  消息帧类型

	Channel*									pChannel_;

	TCPPacket*									pTCPPacket_;
};


}
}

#endif // KBE_WEBSOCKET_PACKET_FILTER_H
