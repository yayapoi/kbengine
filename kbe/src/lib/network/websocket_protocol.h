// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_WEBSOCKET_PROTOCOL_H
#define KBE_WEBSOCKET_PROTOCOL_H

#include "common/common.h"
#include "helper/debug_helper.h"

namespace KBEngine{

class MemoryStream;

namespace Network
{
	class Channel;
	class Packet;

namespace websocket{


/*	WebSocket FRC6544
WebSocketProtocol 类定义了 WebSocket 协议中不同类型的帧，并提供了一些静态方法用于帧的解析和生成。
*/
class WebSocketProtocol
{
public:
	enum FrameType  //  帧类型
	{
		// 下一帧与结束
		NEXT_FRAME = 0x0,
		END_FRAME = 0x80,

		ERROR_FRAME = 0xFF00, //  错误帧
		INCOMPLETE_FRAME = 0xFE00, //  未完成的帧

		OPENING_FRAME = 0x3300, //  打开帧
		CLOSING_FRAME = 0x3400, //  关闭帧

		// 未完成的帧
		INCOMPLETE_TEXT_FRAME = 0x01, //  未完成的文本帧
		INCOMPLETE_BINARY_FRAME = 0x02, //  未完成的二进制帧

		// 文本帧与二进制帧 END_FRAME + *_FRAME
		TEXT_FRAME = 0x81, //  文本帧
		BINARY_FRAME = 0x82, //  二进制帧

		// END_FRAME + *_FRAME
		PING_FRAME = 0x89, //  PING帧
		PONG_FRAME = 0x8A, //  PONG帧

		// 关闭连接
		CLOSE_FRAME = 0x08
	};

	/**
		是否是websocket协议
	*/
	static bool isWebSocketProtocol(MemoryStream* s);
	
	/**
		websocket协议握手
	*/
	static bool handshake(Network::Channel* pChannel, MemoryStream* s);

	/**
		帧解析相关
	*/
	static int makeFrame(FrameType frame_type, Packet* pInPacket, Packet* pOutPacket);
	static int getFrame(Packet* pPacket, uint8& msg_opcode, uint8& msg_fin, uint8& msg_masked, uint32& msg_mask, 
		int32& msg_length_field, uint64& msg_payload_length, FrameType& frameType); //  从包中获取帧信息

	static bool decodingDatas(Packet* pPacket, uint8 msg_masked, uint32 msg_mask); //  解码数据

	static std::string getFrameTypeName(FrameType frame_type); //  获取帧类型名称

};

}
}
}

#endif // KBE_WEBSOCKET_PROTOCOL_H

