// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_CONSOLE_HELPER_H
#define KBE_CONSOLE_HELPER_H
#include "network/message_handler.h"
#include "network/channel.h"

// 定义控制台命令回调消息ID
#define CONSOLE_COMMANDCB_MSGID 65500
// 定义控制台日志消息ID
#define CONSOLE_LOG_MSGID 65501
// 定义控制台观察者回调消息ID
#define CONSOLE_WATCHERCB_MSGID 65502
// 定义控制台性能分析回调消息ID
#define CONSOLE_PROFILECB_MSGID 65503

namespace KBEngine{
namespace ConsoleInterface {
	// 控制台执行命令回调消息处理参数类
	class ConsoleExecCommandCBMessageHandlerArgs1 : public Network::MessageArgs	
	{	
	public:
		std::string strarg; // 命令参数字符串
	public:
		ConsoleExecCommandCBMessageHandlerArgs1():Network::MessageArgs(){}	
		ConsoleExecCommandCBMessageHandlerArgs1(std::string init_strarg):				
		Network::MessageArgs(),
		strarg(init_strarg)				
		{}
		~ConsoleExecCommandCBMessageHandlerArgs1(){}	
		
		// 将参数添加到网络数据包中
        static void staticAddToBundle(Network::Bundle& s, std::string init_strarg) {
            s.appendBlob(init_strarg);
        }
        // 将参数添加到内存流中
        static void staticAddToStream(MemoryStream& s, std::string init_strarg) {
            s.appendBlob(init_strarg);
        }
        // 返回数据大小
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }
        // 将参数添加到内存流中
        virtual void addToStream(MemoryStream& s) {
            s.appendBlob(strarg);
        }
        // 从内存流中读取参数
        virtual void createFromStream(MemoryStream& s) {
            s.readBlob(strarg);
        }
	};	
				
	// 控制台执行命令回调消息处理类
	class ConsoleExecCommandCBMessageHandler : public Network::MessageHandler
	{
	public:
		ConsoleExecCommandCBMessageHandler():
		  Network::MessageHandler()
		{
			onInstall();
		}

		// 安装消息处理器
		virtual void onInstall()
		{
			// 强制这条协议ID 设置消息ID
			msgID = CONSOLE_COMMANDCB_MSGID;
			msgLen = NETWORK_VARIABLE_MESSAGE;// 设置消息长度
			name = "console::execPythonCommand";// 设置消息名称
		}

		// 返回最大消息长度
		virtual int32 msglenMax()
		{ 
			return NETWORK_MESSAGE_MAX_SIZE * 1000; 
		}

		// 处理消息
		virtual void handle(Network::Channel* pChannel, MemoryStream& s)
		{
		};
	};


	// 控制台日志消息处理参数类
    class ConsoleLogMessageHandlerArgsStream : public Network::MessageArgs {
    public:
        ConsoleLogMessageHandlerArgsStream() : Network::MessageArgs() {}
        ~ConsoleLogMessageHandlerArgsStream() {}

        // 返回数据大小
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }

        // 返回消息类型
        virtual MessageArgs::MESSAGE_ARGS_TYPE type(void) {
            return MESSAGE_ARGS_TYPE_VARIABLE;
        }

        // 返回最大消息长度
        virtual int32 msglenMax() { 
            return NETWORK_MESSAGE_MAX_SIZE * 1000; 
        }

        // 将参数添加到内存流中
        virtual void addToStream(MemoryStream& s) {
        }

        // 从内存流中读取参数
        virtual void createFromStream(MemoryStream& s) {
        }
    };	
				
	// 控制台日志消息处理类
    class ConsoleLogMessageHandler : public Network::MessageHandler {
    public:
        ConsoleLogMessageHandler() : Network::MessageHandler() {
            onInstall();
        }

        // 安装消息处理程序
        virtual void onInstall() {
            // 设置消息ID,强制这条协议ID
			msgID = CONSOLE_LOG_MSGID;
            // 设置消息长度
            msgLen = NETWORK_VARIABLE_MESSAGE;
            // 设置消息名称
            name = "console::querylogs";
        }

        // 处理消息
        virtual void handle(Network::Channel* pChannel, MemoryStream& s) {
            // 处理逻辑
        };
    };
	
	// 控制台观察者回调消息处理参数类
    class ConsoleWatcherCBHandlerMessageArgsStream : public Network::MessageArgs {
    public:
        ConsoleWatcherCBHandlerMessageArgsStream() : Network::MessageArgs() {}
        ~ConsoleWatcherCBHandlerMessageArgsStream() {}

        // 返回数据大小
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }

        // 返回消息类型
        virtual MessageArgs::MESSAGE_ARGS_TYPE type(void) {
            return MESSAGE_ARGS_TYPE_VARIABLE;
        }

        // 返回最大消息长度
        virtual int32 msglenMax() { 
            return NETWORK_MESSAGE_MAX_SIZE * 1000; 
        }

        // 将参数添加到内存流中
        virtual void addToStream(MemoryStream& s) {
        }

        // 从内存流中读取参数
        virtual void createFromStream(MemoryStream& s) {
        }
    };
				
	// 控制台观察者回调消息处理类
    class ConsoleWatcherCBMessageHandler : public Network::MessageHandler {
    public:
        ConsoleWatcherCBMessageHandler() : Network::MessageHandler() {
            onInstall();
        }

        // 安装消息处理程序
        virtual void onInstall() {
            // 设置消息ID,强制这条协议ID
			msgID = CONSOLE_WATCHERCB_MSGID;
            // 设置消息长度
            msgLen = NETWORK_VARIABLE_MESSAGE;
            // 设置消息名称
            name = "console::queryWatcher";
        }

        // 处理消息
        virtual void handle(Network::Channel* pChannel, MemoryStream& s) {
            // 处理逻辑
        };
    };

	// 控制台性能分析消息处理参数类
    class ConsoleProfileHandlerArgsStream : public Network::MessageArgs {
    public:
        ConsoleProfileHandlerArgsStream() : Network::MessageArgs() {}
        ~ConsoleProfileHandlerArgsStream() {}

        // 返回数据大小
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }

        // 返回消息类型
        virtual MessageArgs::MESSAGE_ARGS_TYPE type(void) {
            return MESSAGE_ARGS_TYPE_VARIABLE;
        }

        // 返回最大消息长度
        virtual int32 msglenMax() { 
            return NETWORK_MESSAGE_MAX_SIZE * 1000; 
        }

        // 将参数添加到内存流中
        virtual void addToStream(MemoryStream& s) {
        }

        // 从内存流中读取参数
        virtual void createFromStream(MemoryStream& s) {
        }
    };	
				
	// 控制台性能分析消息处理类
    class ConsoleProfileHandler : public Network::MessageHandler {
    public:
        ConsoleProfileHandler() : Network::MessageHandler() {
            onInstall();
        }

        // 安装消息处理程序
        virtual void onInstall() {
            // 设置消息ID,强制这条协议ID
			msgID = CONSOLE_PROFILECB_MSGID;
            // 设置消息长度
            msgLen = NETWORK_VARIABLE_MESSAGE;
            // 设置消息名称
            name = "console::profile";
        }

        // 处理消息
        virtual void handle(Network::Channel* pChannel, MemoryStream& s) {
            // 处理逻辑
        };
    };

	// 控制台查询应用负载消息处理类
	class ConsoleQueryAppsLoadsHandler : public Network::MessageHandler
	{
	public:
		ConsoleQueryAppsLoadsHandler() :
			Network::MessageHandler()
		{
			onInstall();
		}

		virtual void onInstall()
		{
			// 强制这条协议ID
			msgID = CONSOLE_PROFILECB_MSGID;
			msgLen = NETWORK_VARIABLE_MESSAGE;
			name = "console::queryAppsLoads";
		}

		virtual void handle(Network::Channel* pChannel, MemoryStream& s)
		{
		};
	};

	// 控制台查询空间消息处理类
	class ConsoleQuerySpacesHandler : public Network::MessageHandler
	{
	public:
		ConsoleQuerySpacesHandler() :
			Network::MessageHandler()
		{
			onInstall();
		}

		virtual void onInstall()
		{
			// 强制这条协议ID
			msgID = CONSOLE_PROFILECB_MSGID;
			msgLen = NETWORK_VARIABLE_MESSAGE;
			name = "console::querySpaces";
		}

		virtual void handle(Network::Channel* pChannel, MemoryStream& s)
		{
		};
	};
}
}

#endif // KBE_CONSOLE_HELPER_H
