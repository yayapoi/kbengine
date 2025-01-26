// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORK_STATS_H
#define KBE_NETWORK_STATS_H

#include "network/interfaces.h"
#include "common/common.h"
#include "common/singleton.h"

namespace KBEngine { 
namespace Network
{

class MessageHandler;

/*
	记录network流量等信息
	是一个单例类，用于跟踪网络消息的发送和接收统计信息。它允许用户记录消息的发送和接收次数以及大小，并通知注册的处理器。
*/
class NetworkStats : public Singleton<NetworkStats>
{
public:
	// 操作类型枚举，用于区分发送和接收操作
	enum S_OP{
		SEND,	// 发送操作
		RECV	// 接收操作
	};

	// 统计信息结构体
	struct Stats
	{
		Stats()
		{
			name = "";
			send_count = 0;
			send_size = 0;
			recv_size = 0;
			recv_count = 0;
		}

		std::string name;	// 消息名称
		uint32 send_size; //  发送的总字节数
		uint32 send_count; //  发送次数
		uint32 recv_size; //  接收的总字节数
		uint32 recv_count; //  接收次数
	};

	typedef KBEUnordered_map<std::string, Stats> STATS; //  定义一个unordered_map，用于存储消息名称和对应的统计信息

	NetworkStats();
	~NetworkStats();

	void trackMessage(S_OP op, const MessageHandler& msgHandler, uint32 size);	// 跟踪消息的发送或接收

	NetworkStats::STATS& stats(){ return stats_; }	// 获取统计信息

	void addHandler(NetworkStatsHandler* pHandler); //  添加处理器
	void removeHandler(NetworkStatsHandler* pHandler); //  移除处理器

private:
	STATS stats_; //  统计信息

	std::vector<NetworkStatsHandler*> handlers_; //  处理器列表
};

}
}
#endif // KBE_NETWORK_STATS_H
