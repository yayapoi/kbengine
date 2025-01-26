// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_SOCKETUDPPACKET_H
#define KBE_SOCKETUDPPACKET_H
	
#include "network/packet.h"
#include "common/objectpool.h"
	
namespace KBEngine{
namespace Network
{

/**
 * @class UDPPacket
 * 
 * @brief UDP���ݰ���
 * 
 * ����̳��� Packet��ר�����ڴ���UDPЭ������ݰ���
 * ���ṩ��UDP���ݰ��Ĵ��������ա����յȲ�������֧�ֶ���ع�����������ܡ�
 */
class UDPPacket : public Packet
{
public:
	/**
     * ����ָ�����Ͷ���
     */
	typedef KBEShared_ptr< SmartPoolObject< UDPPacket > > SmartPoolObjectPtr;

	// �������ܳض���
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<UDPPacket>& ObjPool(); //  ��ȡ�����
	// �Ӷ�����д�������
	static UDPPacket* createPoolObject(const std::string& logPoint);
	// ���ն��󵽶����
	static void reclaimPoolObject(UDPPacket* obj);
	static void destroyObjPool();
	// ��ȡ��󻺳�����С
	static size_t maxBufferSize();

    UDPPacket(MessageID msgID = 0, size_t res = 0);
	virtual ~UDPPacket(void);
	
	/**
     * �Ӷ˵����UDP���ݰ�
     * 
     * @param ep �˵��������
     * @param pAddr ��ַ����ָ�루��ѡ��
     * @return ���ս��
     */
	int recvFromEndPoint(EndPoint & ep, Address* pAddr = NULL);

	 /**
     * �������ʱ�Ĵ���
     * 
     * �÷����ڶ��󱻻���ʱ���ã������������ݰ�״̬��
     */
	virtual void onReclaimObject();
};

typedef SmartPointer<UDPPacket> UDPPacketPtr;
}
}

#endif // KBE_SOCKETUDPPACKET_H
