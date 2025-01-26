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

/** һ����Ϣ�Ĳ��������� */
class MessageArgs
{
public:
	enum MESSAGE_ARGS_TYPE
	{
		MESSAGE_ARGS_TYPE_VARIABLE = -1,		// �ɱ��������
		MESSAGE_ARGS_TYPE_FIXED = 0				// �̶���������
	};

	MessageArgs():strArgsTypes(){};
	virtual ~MessageArgs(){};
	// ���ڴ����д�����Ϣ����
	virtual void createFromStream(MemoryStream& s) = 0;
	// ����Ϣ������ӵ��ڴ�����
	virtual void addToStream(MemoryStream& s) = 0;
	virtual int32 dataSize(void) = 0;
	virtual MessageArgs::MESSAGE_ARGS_TYPE type(void){ return MESSAGE_ARGS_TYPE_FIXED; }

	std::vector<std::string> strArgsTypes; //  �洢��Ϣ�������͵��ַ���
};

struct ExposedMessageInfo //  ��¶����Ϣ��Ϣ�ṹ��
{
	std::string name; //  ��Ϣ����
	Network::MessageID id; //  ��ϢID
	int16 msgLen; // ��Ϣ���ȣ�������Ϣ���ᳬ��1500
	int8 argsType; //  ��������
	std::vector<uint8> argsTypes; //  ���������б�
};

// ��Ϣ������
class MessageHandler
{
public:
	MessageHandler();
	virtual ~MessageHandler();

	std::string name;	//  ��Ϣ����������
	MessageID msgID;	// ��ϢID
	MessageArgs* pArgs;	// ��Ϣ����
	int32 msgLen;		// �������Ϊ-1��Ϊ�ǹ̶�������Ϣ
	bool exposed;		// �Ƿ���Ⱪ¶
	MessageHandlers* pMessageHandlers;	// ��Ϣ������

	// stats
	volatile mutable uint32 send_size;	// ���͵����ֽ���
	volatile mutable uint32 send_count;	// ���͵��ܴ���
	volatile mutable uint32 recv_size;	// ���յ����ֽ���
	volatile mutable uint32 recv_count;	// ���յ��ܴ���

	// ��ȡ���͵����ֽ���
	uint32 sendsize() const  { return send_size; }
	// ��ȡ���͵��ܴ���
	uint32 sendcount() const  { return send_count; }
	// ��ȡ���͵�ƽ���ֽ���
	uint32 sendavgsize() const  { return (send_count <= 0) ? 0 : send_size / send_count; }

	// ��ȡ���յ����ֽ���
	uint32 recvsize() const  { return recv_size; }
	// ��ȡ���յ��ܴ���
	uint32 recvcount() const  { return recv_count; }
	// ��ȡ���յ�ƽ���ֽ���
	uint32 recvavgsize() const  { return (recv_count <= 0) ? 0 : recv_size / recv_count; }

	/**
		Ĭ�Ϸ������Ϊ�����Ϣ
	*/
	virtual NETWORK_MESSAGE_TYPE type() const
	{ 
		return NETWORK_MESSAGE_TYPE_COMPONENT; 
	}

	// ��ȡ��Ϣ����󳤶�
	virtual int32 msglenMax(){ return NETWORK_MESSAGE_MAX_SIZE; }

	// ��ȡ��Ϣ��C�ַ�����ʾ
	const char* c_str();

	/**
		�����handler�����ǰ�װ��MessageHandlers�󱻵���
	*/
	virtual void onInstall(){}

	// ������Ϣ
	virtual void handle(Channel* pChannel, MemoryStream& s)
	{
		pArgs->createFromStream(s);
		
		// �������������յĽӿ�
	};
};

// ��Ϣ������������
class MessageHandlers
{
public:
	static Network::MessageHandlers* pMainMessageHandlers; //  ��ָ̬�룬ָ������Ϣ������
	typedef std::map<MessageID, MessageHandler*> MessageHandlerMap; //  ��Ϣ������ӳ������
	MessageHandlers(const std::string& name);
	~MessageHandlers();
	
	// �����Ϣ������
	MessageHandler* add(std::string ihName, MessageArgs* args, int32 msgLen, 
						MessageHandler* msgHandler);
	// ��Ӷ��Ⱪ¶����Ϣ
	bool pushExposedMessage(std::string msgname);

	MessageHandler* find(MessageID msgID); //  ������ϢID������Ϣ������
	
	// ��ȡ���һ����ϢID
	MessageID lastMsgID() {return msgID_ - 1;}

	// ��ʼ��������
	bool initializeWatcher();
	
	static void finalise(void); //  ��̬�������������ջ�
	// ��ȡ������Ϣ������
	static std::vector<MessageHandlers*>& messageHandlers();

	// ��ȡ��Ϣ������ӳ��
	const MessageHandlerMap& msgHandlers(){ return msgHandlers_; }

	static std::string getDigestStr(); //  ��̬����������ժҪ�ַ���

	std::string name() const {
		return name_;
	}

private:
	MessageHandlerMap msgHandlers_; //  ��Ϣ��������ӳ��
	MessageID msgID_;	// ���һ����ϢID

	std::vector< std::string > exposedMessages_; //  ��¶����Ϣ����
	std::string name_; //  ��Ϣ������������
};

}
}
#endif 
