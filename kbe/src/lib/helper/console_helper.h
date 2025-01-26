// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_CONSOLE_HELPER_H
#define KBE_CONSOLE_HELPER_H
#include "network/message_handler.h"
#include "network/channel.h"

// �������̨����ص���ϢID
#define CONSOLE_COMMANDCB_MSGID 65500
// �������̨��־��ϢID
#define CONSOLE_LOG_MSGID 65501
// �������̨�۲��߻ص���ϢID
#define CONSOLE_WATCHERCB_MSGID 65502
// �������̨���ܷ����ص���ϢID
#define CONSOLE_PROFILECB_MSGID 65503

namespace KBEngine{
namespace ConsoleInterface {
	// ����ִ̨������ص���Ϣ���������
	class ConsoleExecCommandCBMessageHandlerArgs1 : public Network::MessageArgs	
	{	
	public:
		std::string strarg; // ��������ַ���
	public:
		ConsoleExecCommandCBMessageHandlerArgs1():Network::MessageArgs(){}	
		ConsoleExecCommandCBMessageHandlerArgs1(std::string init_strarg):				
		Network::MessageArgs(),
		strarg(init_strarg)				
		{}
		~ConsoleExecCommandCBMessageHandlerArgs1(){}	
		
		// ��������ӵ��������ݰ���
        static void staticAddToBundle(Network::Bundle& s, std::string init_strarg) {
            s.appendBlob(init_strarg);
        }
        // ��������ӵ��ڴ�����
        static void staticAddToStream(MemoryStream& s, std::string init_strarg) {
            s.appendBlob(init_strarg);
        }
        // �������ݴ�С
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }
        // ��������ӵ��ڴ�����
        virtual void addToStream(MemoryStream& s) {
            s.appendBlob(strarg);
        }
        // ���ڴ����ж�ȡ����
        virtual void createFromStream(MemoryStream& s) {
            s.readBlob(strarg);
        }
	};	
				
	// ����ִ̨������ص���Ϣ������
	class ConsoleExecCommandCBMessageHandler : public Network::MessageHandler
	{
	public:
		ConsoleExecCommandCBMessageHandler():
		  Network::MessageHandler()
		{
			onInstall();
		}

		// ��װ��Ϣ������
		virtual void onInstall()
		{
			// ǿ������Э��ID ������ϢID
			msgID = CONSOLE_COMMANDCB_MSGID;
			msgLen = NETWORK_VARIABLE_MESSAGE;// ������Ϣ����
			name = "console::execPythonCommand";// ������Ϣ����
		}

		// ���������Ϣ����
		virtual int32 msglenMax()
		{ 
			return NETWORK_MESSAGE_MAX_SIZE * 1000; 
		}

		// ������Ϣ
		virtual void handle(Network::Channel* pChannel, MemoryStream& s)
		{
		};
	};


	// ����̨��־��Ϣ���������
    class ConsoleLogMessageHandlerArgsStream : public Network::MessageArgs {
    public:
        ConsoleLogMessageHandlerArgsStream() : Network::MessageArgs() {}
        ~ConsoleLogMessageHandlerArgsStream() {}

        // �������ݴ�С
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }

        // ������Ϣ����
        virtual MessageArgs::MESSAGE_ARGS_TYPE type(void) {
            return MESSAGE_ARGS_TYPE_VARIABLE;
        }

        // ���������Ϣ����
        virtual int32 msglenMax() { 
            return NETWORK_MESSAGE_MAX_SIZE * 1000; 
        }

        // ��������ӵ��ڴ�����
        virtual void addToStream(MemoryStream& s) {
        }

        // ���ڴ����ж�ȡ����
        virtual void createFromStream(MemoryStream& s) {
        }
    };	
				
	// ����̨��־��Ϣ������
    class ConsoleLogMessageHandler : public Network::MessageHandler {
    public:
        ConsoleLogMessageHandler() : Network::MessageHandler() {
            onInstall();
        }

        // ��װ��Ϣ�������
        virtual void onInstall() {
            // ������ϢID,ǿ������Э��ID
			msgID = CONSOLE_LOG_MSGID;
            // ������Ϣ����
            msgLen = NETWORK_VARIABLE_MESSAGE;
            // ������Ϣ����
            name = "console::querylogs";
        }

        // ������Ϣ
        virtual void handle(Network::Channel* pChannel, MemoryStream& s) {
            // �����߼�
        };
    };
	
	// ����̨�۲��߻ص���Ϣ���������
    class ConsoleWatcherCBHandlerMessageArgsStream : public Network::MessageArgs {
    public:
        ConsoleWatcherCBHandlerMessageArgsStream() : Network::MessageArgs() {}
        ~ConsoleWatcherCBHandlerMessageArgsStream() {}

        // �������ݴ�С
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }

        // ������Ϣ����
        virtual MessageArgs::MESSAGE_ARGS_TYPE type(void) {
            return MESSAGE_ARGS_TYPE_VARIABLE;
        }

        // ���������Ϣ����
        virtual int32 msglenMax() { 
            return NETWORK_MESSAGE_MAX_SIZE * 1000; 
        }

        // ��������ӵ��ڴ�����
        virtual void addToStream(MemoryStream& s) {
        }

        // ���ڴ����ж�ȡ����
        virtual void createFromStream(MemoryStream& s) {
        }
    };
				
	// ����̨�۲��߻ص���Ϣ������
    class ConsoleWatcherCBMessageHandler : public Network::MessageHandler {
    public:
        ConsoleWatcherCBMessageHandler() : Network::MessageHandler() {
            onInstall();
        }

        // ��װ��Ϣ�������
        virtual void onInstall() {
            // ������ϢID,ǿ������Э��ID
			msgID = CONSOLE_WATCHERCB_MSGID;
            // ������Ϣ����
            msgLen = NETWORK_VARIABLE_MESSAGE;
            // ������Ϣ����
            name = "console::queryWatcher";
        }

        // ������Ϣ
        virtual void handle(Network::Channel* pChannel, MemoryStream& s) {
            // �����߼�
        };
    };

	// ����̨���ܷ�����Ϣ���������
    class ConsoleProfileHandlerArgsStream : public Network::MessageArgs {
    public:
        ConsoleProfileHandlerArgsStream() : Network::MessageArgs() {}
        ~ConsoleProfileHandlerArgsStream() {}

        // �������ݴ�С
        virtual int32 dataSize(void) {
            return NETWORK_VARIABLE_MESSAGE;
        }

        // ������Ϣ����
        virtual MessageArgs::MESSAGE_ARGS_TYPE type(void) {
            return MESSAGE_ARGS_TYPE_VARIABLE;
        }

        // ���������Ϣ����
        virtual int32 msglenMax() { 
            return NETWORK_MESSAGE_MAX_SIZE * 1000; 
        }

        // ��������ӵ��ڴ�����
        virtual void addToStream(MemoryStream& s) {
        }

        // ���ڴ����ж�ȡ����
        virtual void createFromStream(MemoryStream& s) {
        }
    };	
				
	// ����̨���ܷ�����Ϣ������
    class ConsoleProfileHandler : public Network::MessageHandler {
    public:
        ConsoleProfileHandler() : Network::MessageHandler() {
            onInstall();
        }

        // ��װ��Ϣ�������
        virtual void onInstall() {
            // ������ϢID,ǿ������Э��ID
			msgID = CONSOLE_PROFILECB_MSGID;
            // ������Ϣ����
            msgLen = NETWORK_VARIABLE_MESSAGE;
            // ������Ϣ����
            name = "console::profile";
        }

        // ������Ϣ
        virtual void handle(Network::Channel* pChannel, MemoryStream& s) {
            // �����߼�
        };
    };

	// ����̨��ѯӦ�ø�����Ϣ������
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
			// ǿ������Э��ID
			msgID = CONSOLE_PROFILECB_MSGID;
			msgLen = NETWORK_VARIABLE_MESSAGE;
			name = "console::queryAppsLoads";
		}

		virtual void handle(Network::Channel* pChannel, MemoryStream& s)
		{
		};
	};

	// ����̨��ѯ�ռ���Ϣ������
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
			// ǿ������Э��ID
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
