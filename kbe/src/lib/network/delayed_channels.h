// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef DELAYED_CHANNELS_H
#define DELAYED_CHANNELS_H

#include "common/tasks.h"
#include "common/smartpointer.h"
#include "common/singleton.h"

namespace KBEngine{
namespace Network
{
class Channel;
class Address;
class EventDispatcher;
class NetworkInterface;

// 延迟通道管理类
class DelayedChannels : public Task
{
public:
	void init(EventDispatcher & dispatcher, NetworkInterface* pNetworkInterface);
	void fini(EventDispatcher & dispatcher);

	// 添加一个通道到管理器中
	void add(Channel & channel);

	void sendIfDelayed(Channel & channel);

private:
	// 处理任务
	virtual bool process();

	typedef std::set<Address> ChannelAddrs;
	ChannelAddrs channeladdrs_;

	// 网络接口指针
	NetworkInterface* pNetworkInterface_;
};

}
}
#endif // DELAYED_CHANNELS_H
