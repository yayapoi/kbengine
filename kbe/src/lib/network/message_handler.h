// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_MESSAGE_HANDLER_H
#define KBE_MESSAGE_HANDLER_H

#include "common/memorystream.h"
#include "common/smartpointer.h"
#include "helper/debug_helper.h"
#include "network/common.h"

namespace KBEngine{

class KBE_MD5;

namespace Network
{

class Channel;
class MessageHandlers;

/** 一个消息的参数抽象类 */
class MessageArgs
{
public:
	enum MESSAGE_ARGS_TYPE
	{
		MESSAGE_ARGS_TYPE_VARIABLE = -1,		// 可变参数长度
		MESSAGE_ARGS_TYPE_FIXED = 0				// 固定参数长度
	};

	MessageArgs():strArgsTypes(){};
	virtual ~MessageArgs(){};
	// 从内存流中创建消息参数
	virtual void createFromStream(MemoryStream& s) = 0;
	// 将消息参数添加到内存流中
	virtual void addToStream(MemoryStream& s) = 0;
	virtual int32 dataSize(void) = 0;
	virtual MessageArgs::MESSAGE_ARGS_TYPE type(void){ return MESSAGE_ARGS_TYPE_FIXED; }

	std::vector<std::string> strArgsTypes; //  存储消息参数类型的字符串
};

struct ExposedMessageInfo //  暴露的消息信息结构体
{
	std::string name; //  消息名称
	Network::MessageID id; //  消息ID
	int16 msgLen; // 消息长度，对外消息不会超过1500
	int8 argsType; //  参数类型
	std::vector<uint8> argsTypes; //  参数类型列表
};

// 消息处理类
class MessageHandler
{
public:
	MessageHandler();
	virtual ~MessageHandler();

	std::string name;	//  消息处理器名称
	MessageID msgID;	// 消息ID
	MessageArgs* pArgs;	// 消息参数
	int32 msgLen;		// 如果长度为-1则为非固定长度消息
	bool exposed;		// 是否对外暴露
	MessageHandlers* pMessageHandlers;	// 消息处理器

	// stats
	volatile mutable uint32 send_size;	// 发送的总字节数
	volatile mutable uint32 send_count;	// 发送的总次数
	volatile mutable uint32 recv_size;	// 接收的总字节数
	volatile mutable uint32 recv_count;	// 接收的总次数

	// 获取发送的总字节数
	uint32 sendsize() const  { return send_size; }
	// 获取发送的总次数
	uint32 sendcount() const  { return send_count; }
	// 获取发送的平均字节数
	uint32 sendavgsize() const  { return (send_count <= 0) ? 0 : send_size / send_count; }

	// 获取接收的总字节数
	uint32 recvsize() const  { return recv_size; }
	// 获取接收的总次数
	uint32 recvcount() const  { return recv_count; }
	// 获取接收的平均字节数
	uint32 recvavgsize() const  { return (recv_count <= 0) ? 0 : recv_size / recv_count; }

	/**
		默认返回类别为组件消息
	*/
	virtual NETWORK_MESSAGE_TYPE type() const
	{ 
		return NETWORK_MESSAGE_TYPE_COMPONENT; 
	}

	// 获取消息的最大长度
	virtual int32 msglenMax(){ return NETWORK_MESSAGE_MAX_SIZE; }

	// 获取消息的C字符串表示
	const char* c_str();

	/**
		当这个handler被正是安装到MessageHandlers后被调用
	*/
	virtual void onInstall(){}

	// 处理消息
	virtual void handle(Channel* pChannel, MemoryStream& s)
	{
		pArgs->createFromStream(s);
		
		// 将参数传给最终的接口
	};
};

// 消息处理器集合类
class MessageHandlers
{
public:
	static Network::MessageHandlers* pMainMessageHandlers; //  静态指针，指向主消息处理器
	typedef std::map<MessageID, MessageHandler*> MessageHandlerMap; //  消息处理器映射类型
	MessageHandlers(const std::string& name);
	~MessageHandlers();
	
	// 添加消息处理器
	MessageHandler* add(std::string ihName, MessageArgs* args, int32 msgLen, 
						MessageHandler* msgHandler);
	// 添加对外暴露的消息
	bool pushExposedMessage(std::string msgname);

	MessageHandler* find(MessageID msgID); //  根据消息ID查找消息处理器
	
	// 获取最后一个消息ID
	MessageID lastMsgID() {return msgID_ - 1;}

	// 初始化监视器
	bool initializeWatcher();
	
	static void finalise(void); //  静态函数，用于最终化
	// 获取所有消息处理器
	static std::vector<MessageHandlers*>& messageHandlers();

	// 获取消息处理器映射
	const MessageHandlerMap& msgHandlers(){ return msgHandlers_; }

	static std::string getDigestStr(); //  静态函数，返回摘要字符串

	std::string name() const {
		return name_;
	}

private:
	MessageHandlerMap msgHandlers_; //  消息处理器的映射
	MessageID msgID_;	// 最后一个消息ID

	std::vector< std::string > exposedMessages_; //  暴露的消息向量
	std::string name_; //  消息处理器的名称
};

}
}
#endif 
