// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_WEBSOCKET_PACKET_FILTER_H
#define KBE_WEBSOCKET_PACKET_FILTER_H

#include "network/packet_filter.h"
#include "network/websocket_protocol.h"

namespace KBEngine { 
namespace Network
{
class TCPPacket;

/**
 * @class WebSocketPacketFilter
 * 
 * @brief WebSocket ���ݰ���������
 * 
 * ����̳��� PacketFilter��ר�����ڴ��� WebSocket Э������ݰ����ˡ�
 * �����������ݰ����ͺͽ���ʱ���б�Ҫ�Ĵ���������Ƭ������Ϣ���ͼ��ȡ�
 */
class WebSocketPacketFilter : public PacketFilter
{
public:
	WebSocketPacketFilter(Channel* pChannel);
	virtual ~WebSocketPacketFilter();

 	/**
     * �������ݰ�
     * 
     * �÷����������ݰ�ͨ��ͨ�����ͳ�ȥ�������ط��ͽ����
     * 
     * @param pChannel ͨ��ָ��
     * @param sender ���ݰ�������
     * @param pPacket �����͵����ݰ�ָ��
     * @param userarg �û��Զ������
     * @return ���ͽ��
     */
	virtual Reason send(Channel * pChannel, PacketSender& sender, Packet * pPacket, int userarg);

	/**
     * �������ݰ�
     * 
     * �÷��������ͨ���н������ݰ��������ؽ��ս����
     * 
     * @param pChannel ͨ��ָ��
     * @param receiver ���ݰ�������
     * @param pPacket ���յ������ݰ�ָ��
     * @return ���ս��
     */
	virtual Reason recv(Channel * pChannel, PacketReceiver & receiver, Packet * pPacket);

protected:
	void reset();
	
	/**
     * ���� Ping ��Ϣ
     * 
     * �÷������ڴ�����յ��� Ping ��Ϣ��
     * 
     * @param pChannel ͨ��ָ��
     * @param pPacket ���յ��� Ping ��Ϣ���ݰ�ָ��
     * @return ������
     */
	Reason onPing(Channel * pChannel, Packet* pPacket);

protected:
	enum FragmentDataTypes //  �������ݰ�����
	{
		FRAGMENT_MESSAGE_HREAD,	// ��Ƭ��Ϣͷ
		FRAGMENT_MESSAGE_DATAS	// ��Ƭ��Ϣ����
	};

	int32										pFragmentDatasRemain_; //  ʣ���Ƭ������
	FragmentDataTypes							fragmentDatasFlag_; //  Ƭ����������

	uint8										msg_opcode_; //  ��Ϣ������
	uint8										msg_fin_; //  ��Ϣ�Ƿ����
	uint8										msg_masked_; //  ��Ϣ�Ƿ�����
	uint32										msg_mask_; //  ��Ϣ����
	int32										msg_length_field_; //  ��Ϣ�����ֶ�
	uint64										msg_payload_length_; //  ��Ϣ���س���
	websocket::WebSocketProtocol::FrameType		msg_frameType_; //  ��Ϣ֡����

	Channel*									pChannel_;

	TCPPacket*									pTCPPacket_;
};


}
}

#endif // KBE_WEBSOCKET_PACKET_FILTER_H
