// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_FIXED_NETWORK_MESSAGES_H
#define KBE_FIXED_NETWORK_MESSAGES_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "network/common.h"

namespace KBEngine { 
namespace Network
{
/*
	用来对中间协议(前端与后端之间)进行强制约定。
	没有使用到kbe整套协议自动绑定机制的前端可以使用此处来强制约定协议。
*/
class FixedMessages : public Singleton<FixedMessages>
{
public:

	// 固定的协议数据结构
	struct MSGInfo
	{
		MessageID msgid; //  消息ID
		std::string msgname; //  消息名称
		//std::wstring descr; //  消息描述
	};

public:
	FixedMessages();
	~FixedMessages();

	bool loadConfig(std::string fileName, bool notFoundError = true); //  加载配置文件

	FixedMessages::MSGInfo* isFixed(const char* msgName); //  根据消息名称判断是否为固定消息
	bool isFixed(MessageID msgid); //  根据消息ID判断是否为固定消息

public:
	typedef KBEUnordered_map<std::string, MSGInfo> MSGINFO_MAP; //  定义一个字符串到MSGInfo的映射类型

private:
	MSGINFO_MAP _infomap; //  存储消息信息的映射
	bool _loaded;	// 配置是否已加载
};

}
}
#endif // KBE_FIXED_NETWORK_MESSAGES_H
