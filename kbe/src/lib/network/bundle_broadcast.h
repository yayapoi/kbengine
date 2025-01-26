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
	可以方便的处理如:向局域网内广播某些信息， 并处理收集相关信息。
	通过 UDP 协议进行广播通信，并提供了发送和接收广播消息的功能
*/
class BundleBroadcast : public Bundle
{
public:
	BundleBroadcast(NetworkInterface & networkInterface, uint16 bindPort = KBE_PORT_BROADCAST_DISCOVERY, 
		uint32 recvWindowSize = PACKET_MAX_SIZE_UDP);
	virtual ~BundleBroadcast();

	EventDispatcher& dispatcher(); //  返回事件调度器
	
	bool broadcast(uint16 port = 0); //  广播信息
	bool receive(MessageArgs* recvArgs, sockaddr_in* psin = NULL, int32 timeout = 100000, bool showerr = true); //  接收信息

	// 获取监听端点
	Network::EndPoint& epListen() { 
		return epListen_; 
	}

	void close();

	bool good() const { 
		return epListen_.good() && good_; 
	}

	// 设置重试次数
	void itry(int8 i){ 
		itry_ = i; 
	}

	void addBroadCastAddress(std::string addr); //  添加广播地址

protected:
	// 监听端点
    Network::EndPoint epListen_;
    // 广播端点
    Network::EndPoint epBroadcast_;
	NetworkInterface & networkInterface_; //  网络接口
	uint32 recvWindowSize_; //  接收窗口大小
	bool good_; //  连接是否正常
	int8 itry_; //  尝试次数
	std::vector< std::string > machine_addresses_; //  机器地址列表
};

}
}

#ifdef CODE_INLINE
#include "bundle_broadcast.inl"
#endif
#endif // KBE_BUNDLE_BROADCAST_H
