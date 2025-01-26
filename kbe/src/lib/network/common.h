// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORK_COMMON_H
#define KBE_NETWORK_COMMON_H

// common include
#include "common/common.h"
#include "helper/debug_option.h"

namespace KBEngine { 
namespace Network
{
const uint32 BROADCAST = 0xFFFFFFFF;	// �㲥��ַ
const uint32 LOCALHOST = 0x0100007F;	// ���ػػ���ַ

// ��Ϣ��ID
typedef uint16	MessageID;

// ��Ϣ���ȣ�Ŀǰ������2�֣�Ĭ����Ϣ�������MessageLength
// �����������ʱ��Ҫ��չ���ȣ��ײ�ʹ��MessageLength1
typedef uint16	MessageLength;		// ���65535
typedef uint32	MessageLength1;		// ���4294967295

typedef int32	ChannelID;
const ChannelID CHANNEL_ID_NULL = 0;	// ��Ч��ͨ��ID

// ͨ����ʱʱ��
extern float g_channelInternalTimeout;	// �ڲ�ͨ����ʱʱ��
extern float g_channelExternalTimeout;	// �ⲿͨ����ʱʱ��

// ͨ�����ͳ�ʱ����
extern uint32 g_intReSendInterval; //  �ڲ�ͨ�����Լ��
extern uint32 g_intReSendRetries; //  �ڲ�ͨ�����Դ���
extern uint32 g_extReSendInterval; //  �ⲿͨ�����Լ��
extern uint32 g_extReSendRetries; //  �ⲿͨ�����Դ���

// �ⲿͨ���������
extern int8 g_channelExternalEncryptType;

// listen�����������ֵ
extern uint32 g_SOMAXCONN;

// udp���ְ�
extern const char* UDP_HELLO; //  UDP��������
extern const char* UDP_HELLO_ACK; //  UDP������Ӧ

// UDP����
extern uint32 g_rudp_intWritePacketsQueueSize;	// �ڲ�UDPд���д�С
extern uint32 g_rudp_intReadPacketsQueueSize;	// �ڲ�UDP�����д�С
extern uint32 g_rudp_extWritePacketsQueueSize;	// �ⲿUDPд���д�С
extern uint32 g_rudp_extReadPacketsQueueSize;	// �ⲿUDP�����д�С
extern uint32 g_rudp_tickInterval;	// UDP�������
extern uint32 g_rudp_minRTO;	// UDP��С�ش���ʱʱ��
extern uint32 g_rudp_mtu;	// UDP����䵥Ԫ
extern uint32 g_rudp_missAcksResend;	// UDP��ʧȷ���ش�����
extern bool g_rudp_congestionControl;	// �Ƿ�����ӵ������
extern bool g_rudp_nodelay;	// �Ƿ�����ӳ�

// HTTPS/WSS/SSLͨ�������֤���ļ�
extern std::string g_sslCertificate;	// SSL֤���ļ�·��
extern std::string g_sslPrivateKey;	// SSL˽Կ�ļ�·��

// ����ͨ����ʱ���
#define CLOSE_CHANNEL_INACTIVITIY_DETECTION()										\
{																					\
	Network::g_channelExternalTimeout = Network::g_channelInternalTimeout = -1.0f;	\
}																					\

	
namespace udp{
}

namespace tcp{
}

// ���ܶ���洢����Ϣռ���ֽ�(����+���)
#define ENCRYPTTION_WASTAGE_SIZE			(1 + 7)

#define PACKET_MAX_SIZE						1500	// ������С
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP					1460	// TCP������С
#endif
#define PACKET_MAX_SIZE_UDP					1472	// UDP������С

typedef uint16								PacketLength;				// ���������65535
#define PACKET_LENGTH_SIZE					sizeof(PacketLength)	// �����ȴ�С

#define NETWORK_MESSAGE_ID_SIZE				sizeof(Network::MessageID)	 // ��ϢID��С
#define NETWORK_MESSAGE_LENGTH_SIZE			sizeof(Network::MessageLength)	// ��Ϣ���ȴ�С
#define NETWORK_MESSAGE_LENGTH1_SIZE		sizeof(Network::MessageLength1)	 // ��չ��Ϣ���ȴ�С
#define NETWORK_MESSAGE_MAX_SIZE			65535	// ��Ϣ��󳤶�
#define NETWORK_MESSAGE_MAX_SIZE1			4294967295	// ��չ��Ϣ��󳤶�

// ��Ϸ���ݿ��ð���С
#define GAME_PACKET_MAX_SIZE_TCP			PACKET_MAX_SIZE_TCP - NETWORK_MESSAGE_ID_SIZE - \
											NETWORK_MESSAGE_LENGTH_SIZE - ENCRYPTTION_WASTAGE_SIZE

/** kbe machine�˿� */
#define KBE_PORT_START						20000	// kbe�˿���ʼֵ
#define KBE_MACHINE_BROADCAST_SEND_PORT		KBE_PORT_START + 86			// machine���չ㲥�Ķ˿�
#define KBE_PORT_BROADCAST_DISCOVERY		KBE_PORT_START + 87	// �㲥���ֶ˿�
#define KBE_MACHINE_TCP_PORT				KBE_PORT_START + 88	// machine TCP�˿�

#define KBE_INTERFACES_TCP_PORT				30099	// �ӿ�TCP�˿�

/*
	������Ϣ���ͣ� �������߱䳤��
	�����Ҫ�Զ��峤������NETWORK_INTERFACE_DECLARE_BEGIN������ʱ���볤�ȼ��ɡ�
*/
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0 //  �̶�������Ϣ����
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1	 // �䳤��Ϣ
#endif

// ������Ϣ���
enum NETWORK_MESSAGE_TYPE
{
	NETWORK_MESSAGE_TYPE_COMPONENT = 0,	// �����Ϣ
	NETWORK_MESSAGE_TYPE_ENTITY = 1,	// entity��Ϣ
};

enum ProtocolType
{
	PROTOCOL_TCP = 0, //  TCPЭ��
	PROTOCOL_UDP = 1, //  UDPЭ��
};

enum ProtocolSubType
{
	SUB_PROTOCOL_DEFAULT = 0, //  Ĭ��Э��
	SUB_PROTOCOL_UDP = 1, //  UDPЭ��
	SUB_PROTOCOL_KCP = 2, //  KCPЭ��
};

enum Reason
{
	REASON_SUCCESS = 0,				 // No reason.�ɹ�
	REASON_TIMER_EXPIRED = -1,		 // Timer expired.��ʱ����ʱ
	REASON_NO_SUCH_PORT = -2,		 // Destination port is not open.Ŀ��˿�δ��
	REASON_GENERAL_NETWORK = -3,	 // The network is stuffed.�������
	REASON_CORRUPTED_PACKET = -4,	 // Got a bad packet.����
	REASON_NONEXISTENT_ENTRY = -5,	 // Wanted to call a null function.�����˿պ���
	REASON_WINDOW_OVERFLOW = -6,	 // Channel send window overflowed.���ʹ������
	REASON_INACTIVITY = -7,			 // Channel inactivity timeout.ͨ�������ʱ
	REASON_RESOURCE_UNAVAILABLE = -8,// Corresponds to EAGAIN.��Դ������
	REASON_CLIENT_DISCONNECTED = -9, // Client disconnected voluntarily.�ͻ��������Ͽ�����
	REASON_TRANSMIT_QUEUE_FULL = -10,// Corresponds to ENOBUFS.���Ͷ�����
	REASON_CHANNEL_LOST = -11,		 // Corresponds to channel lost.ͨ����ʧ
	REASON_SHUTTING_DOWN = -12,		 // Corresponds to shutting down app.Ӧ�ó���ر�
	REASON_WEBSOCKET_ERROR = -13,    // html5 error. WebSocket����
	REASON_CHANNEL_CONDEMN = -14,	 // condemn error.ͨ����Ǵ��
};

inline
const char * reasonToString(Reason reason)
{
	const char * reasons[] =
	{
		"REASON_SUCCESS",
		"REASON_TIMER_EXPIRED",
		"REASON_NO_SUCH_PORT",
		"REASON_GENERAL_NETWORK",
		"REASON_CORRUPTED_PACKET",
		"REASON_NONEXISTENT_ENTRY",
		"REASON_WINDOW_OVERFLOW",
		"REASON_INACTIVITY",
		"REASON_RESOURCE_UNAVAILABLE",
		"REASON_CLIENT_DISCONNECTED",
		"REASON_TRANSMIT_QUEUE_FULL",
		"REASON_CHANNEL_LOST",
		"REASON_SHUTTING_DOWN",
		"REASON_WEBSOCKET_ERROR",
		"REASON_CHANNEL_CONDEMN"
	};

	unsigned int index = -reason; //  ��reasonת��Ϊunsigned int����

	if (index < sizeof(reasons)/sizeof(reasons[0])) //  ���indexС��reasons����Ĵ�С
	{
		return reasons[index]; //  ����reasons�����ж�Ӧ���ַ���
	}

	return "REASON_UNKNOWN"; //  ���򷵻�"REASON_UNKNOWN"
}

// �������ݵ�ָ���˵�
#define NETWORK_SEND_TO_ENDPOINT(ep, op, pPacket)															\
{																											\
	int retries = 0;																						\
	Network::Reason reason;																					\
																											\
	while(true)																								\
	{																										\
		retries++;																							\
		int slen = ep->op(pPacket->data(), pPacket->totalSize());											\
																											\
		if(slen != (int)pPacket->totalSize())																\
		{																									\
			reason = Network::PacketSender::checkSocketErrors(ep, slen, pPacket->totalSize());				\
			/* ������ͳ��ִ�����ô���ǿ��Լ�������һ�Σ� ����3���˳�	*/										\
			if (reason == Network::REASON_NO_SUCH_PORT && retries <= 3)										\
			{																								\
				continue;																					\
			}																								\
																											\
			/* ���ϵͳ���ͻ����Ѿ����ˣ������ǵȴ�10ms	*/													\
			if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)					\
															&& retries <= 3)								\
			{																								\
				WARNING_MSG(fmt::format("{}: "																\
					"Transmit queue full, waiting for space... ({})\n",										\
					__FUNCTION__, retries));																\
																											\
				KBEngine::sleep(10);																		\
				continue;																					\
			}																								\
																											\
			if(retries > 3 && reason != Network::REASON_SUCCESS)											\
			{																								\
				ERROR_MSG(fmt::format("NETWORK_SEND::send: packet discarded(reason={}).\n",					\
															(reasonToString(reason))));						\
				break;																						\
			}																								\
		}																									\
		else																								\
		{																									\
			break;																							\
		}																									\
	}																										\
																											\
}																											\

// �������ݰ�����
#define SEND_BUNDLE_COMMON(SND_FUNC, BUNDLE)																\
	BUNDLE.finiMessage();																					\
																											\
	Network::Bundle::Packets::iterator iter = BUNDLE.packets().begin();										\
	for (; iter != BUNDLE.packets().end(); ++iter)															\
	{																										\
		Packet* pPacket = (*iter);																			\
		int retries = 0;																					\
		Reason reason;																						\
		pPacket->sentSize = 0;																				\
																											\
		while(true)																							\
		{																									\
			++retries;																						\
			int slen = SND_FUNC;																			\
																											\
			if(slen > 0)																					\
				pPacket->sentSize += slen;																	\
																											\
			if(pPacket->sentSize != pPacket->length())														\
			{																								\
				reason = PacketSender::checkSocketErrors(&ep);												\
				/* ������ͳ��ִ�����ô���ǿ��Լ�������һ�Σ� ����60���˳�	*/									\
				if (reason == REASON_NO_SUCH_PORT && retries <= 3)											\
				{																							\
					continue;																				\
				}																							\
																											\
				/* ���ϵͳ���ͻ����Ѿ����ˣ������ǵȴ�10ms	*/												\
				if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)				\
																					&& retries <= 60)		\
				{																							\
					WARNING_MSG(fmt::format("{}: "															\
						"Transmit queue full, waiting for space... ({})\n",									\
						__FUNCTION__, retries));															\
																											\
					ep.waitSend();																			\
					continue;																				\
				}																							\
																											\
				if(retries > 60 && reason != REASON_SUCCESS)												\
				{																							\
					ERROR_MSG(fmt::format("Bundle::basicSendWithRetries: packet discarded(reason={}).\n",	\
															(reasonToString(reason))));						\
					break;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				break;																						\
			}																								\
		}																									\
																											\
	}																										\
																											\
	BUNDLE.clearPackets();																					\
																											\


#define SEND_BUNDLE(ENDPOINT, BUNDLE)																		\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.send(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize), BUNDLE);														\
}																											\


#define SENDTO_BUNDLE(ENDPOINT, ADDR, PORT, BUNDLE)															\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.sendto(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize, PORT, ADDR), BUNDLE);											\
}																											\

// �������ݰ�
#define MALLOC_PACKET(outputPacket, isTCPPacket)															\
{																											\
	if(isTCPPacket)																							\
		outputPacket = TCPPacket::createPoolObject(OBJECTPOOL_POINT);										\
	else																									\
		outputPacket = UDPPacket::createPoolObject(OBJECTPOOL_POINT);										\
}																											\

// �������ݰ�
#define RECLAIM_PACKET(isTCPPacket, pPacket)																\
{																											\
	if(isTCPPacket)																							\
		TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));										\
	else																									\
		UDPPacket::reclaimPoolObject(static_cast<UDPPacket*>(pPacket));										\
}																											\


// ��Ϸ��������ѡ��trace_packetʹ�ã���������һ�������������Ϣ��
#define TRACE_MESSAGE_PACKET(isrecv, pPacket, pCurrMsgHandler, length, addr, readPacketHead)				\
	if(Network::g_trace_packet > 0)																			\
	{																										\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger("packetlogs");											\
																											\
		size_t headsize = 0;																				\
		if(pCurrMsgHandler && readPacketHead)																\
		{																									\
			headsize = NETWORK_MESSAGE_ID_SIZE;																\
			if (pCurrMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)										\
			{																								\
				headsize += NETWORK_MESSAGE_LENGTH_SIZE;													\
				if (length >= NETWORK_MESSAGE_MAX_SIZE)														\
					headsize += NETWORK_MESSAGE_LENGTH1_SIZE;												\
			}																								\
		}																									\
																											\
		bool isprint = true;																				\
		if(pCurrMsgHandler)																					\
		{																									\
			std::vector<std::string>::iterator iter = std::find(Network::g_trace_packet_disables.begin(),	\
														Network::g_trace_packet_disables.end(),				\
															pCurrMsgHandler->name);							\
																											\
			if(iter != Network::g_trace_packet_disables.end())												\
			{																								\
				isprint = false;																			\
			}																								\
			else																							\
			{																								\
				DEBUG_MSG(fmt::format("{} {}:msgID:{}, currMsgLength:{}, addr:{}\n",						\
						((isrecv == true) ? "====>" : "<===="),												\
						pCurrMsgHandler->name.c_str(),														\
						pCurrMsgHandler->msgID,																\
						(length + headsize),																\
						addr));																				\
			}																								\
		}																									\
																											\
		if(isprint)																							\
		{																									\
																											\
			size_t rpos = pPacket->rpos();																	\
			if(headsize > 0)																				\
				pPacket->rpos(pPacket->rpos() - headsize);													\
																											\
			switch(Network::g_trace_packet)																	\
			{																								\
			case 1:																							\
				pPacket->hexlike();																			\
				break;																						\
			case 2:																							\
				pPacket->textlike();																		\
				break;																						\
			default:																						\
				pPacket->print_storage();																	\
				break;																						\
			};																								\
																											\
			pPacket->rpos(rpos);																			\
		}																									\
																											\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger(COMPONENT_NAME_EX(g_componentType));					\
	}																										\


bool kbe_poll(int fd); //  ����ļ��������Ƿ�ɶ�

void destroyObjPool(); //  ���ٶ����

// network stats.����ͳ����Ϣ
extern uint64						g_numPacketsSent;	// ���͵����ݰ�����
extern uint64						g_numPacketsReceived;	// ���յ����ݰ�����
extern uint64						g_numBytesSent;	// ���͵��ֽ���
extern uint64						g_numBytesReceived;	// ���յ��ֽ���

// �����մ������
extern uint32						g_receiveWindowMessagesOverflowCritical;// ���մ�����Ϣ����ٽ�ֵ
extern uint32						g_intReceiveWindowMessagesOverflow;	// �ڲ����մ�����Ϣ���
extern uint32						g_extReceiveWindowMessagesOverflow;	// �ⲿ���մ�����Ϣ���
extern uint32						g_intReceiveWindowBytesOverflow;	// �ڲ����մ����ֽ����
extern uint32						g_extReceiveWindowBytesOverflow;	// �ⲿ���մ����ֽ����

extern uint32						g_sendWindowMessagesOverflowCritical;	// ���ʹ�����Ϣ����ٽ�ֵ
extern uint32						g_intSendWindowMessagesOverflow;	// �ڲ����ʹ�����Ϣ���
extern uint32						g_extSendWindowMessagesOverflow;	// �ⲿ���ʹ�����Ϣ���
extern uint32						g_intSendWindowBytesOverflow;	// �ڲ����ʹ����ֽ����
extern uint32						g_extSendWindowBytesOverflow;	// �ⲿ���ʹ����ֽ����
extern uint32						g_intSentWindowBytesOverflow;	// �ڲ��ѷ��ʹ����ֽ����
extern uint32						g_extSentWindowBytesOverflow;	// �ⲿ�ѷ��ʹ����ֽ����

bool initializeWatcher();	// ��ʼ��������
bool initialize();	// ��ʼ������ģ��
void finalise(void);	// �ͷ�����ģ����Դ

}
}

#endif // KBE_NETWORK_COMMON_H
