// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORKCHANCEL_H
#define KBE_NETWORKCHANCEL_H

#include "common/common.h"
#include "common/timer.h"
#include "common/smartpointer.h"
#include "common/timestamp.h"
#include "common/objectpool.h"
#include "helper/debug_helper.h"
#include "network/address.h"
#include "network/event_dispatcher.h"
#include "network/endpoint.h"
#include "network/packet.h"
#include "network/common.h"
#include "network/bundle.h"
#include "network/interfaces.h"
#include "network/packet_filter.h"
#include "network/ikcp.h"

namespace KBEngine { 
namespace Network
{

class Bundle;
class NetworkInterface;
class MessageHandlers;
class PacketReader;
class PacketSender;

class Channel : public TimerHandler, public PoolObject
{
public:
	typedef KBEShared_ptr< SmartPoolObject< Channel > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<Channel>& ObjPool();
	static Channel* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(Channel* obj); //  ���ն���ض���
	static void destroyObjPool(); //  ���ٶ����
	virtual void onReclaimObject(); //  ���ն���ʱ����
	virtual size_t getPoolObjectBytes(); //  ��ȡ����ض����ֽ���
	virtual void onEabledPoolObject(); //  ���ö���ض���ʱ����

	enum Traits //  ö�����ͣ�����ͨ������
	{
		/// This describes the properties of channel from server to server.
		INTERNAL = 0,// ��������������

		/// This describes the properties of a channel from client to server.
		EXTERNAL = 1,// �ͻ��˵�������
	};
	
	enum ChannelTypes
	{
		/// ��ͨͨ��
		CHANNEL_NORMAL = 0,

		// �����webͨ��
		CHANNEL_WEB = 1,
	};

	enum Flags
	{
		FLAG_SENDING					= 0x00000001,	// ������Ϣ��
		FLAG_DESTROYED					= 0x00000002,	// ͨ���Ѿ�����
		FLAG_HANDSHAKE					= 0x00000004,	// �Ѿ����ֹ�
		FLAG_CONDEMN_AND_WAIT_DESTROY	= 0x00000008,	// ��Ƶ���Ѿ���ò��Ϸ������������ݷ�����Ϻ�ر�
		FLAG_CONDEMN_AND_DESTROY		= 0x00000010,	// ��Ƶ���Ѿ���ò��Ϸ��������ر�
		FLAG_CONDEMN					= FLAG_CONDEMN_AND_WAIT_DESTROY | FLAG_CONDEMN_AND_DESTROY,
	};

public:
	Channel();

	Channel(NetworkInterface & networkInterface, 
		const EndPoint * pEndPoint, 
		Traits traits, 
		ProtocolType pt = PROTOCOL_TCP, 
		ProtocolSubType spt = SUB_PROTOCOL_DEFAULT,
		PacketFilterPtr pFilter = NULL, 
		ChannelID id = CHANNEL_ID_NULL);

	virtual ~Channel();
	
	static Channel * get(NetworkInterface & networkInterface,
			const Address& addr);
	
	static Channel * get(NetworkInterface & networkInterface,
			const EndPoint* pSocket);
	
	 //  ��ʼ�����⣬inactivityPeriodΪ���ʱ�䣬checkPeriodΪ�������
	void startInactivityDetection( float inactivityPeriod,
			float checkPeriod = 1.f );
	
	void stopInactivityDetection(); //  ֹͣ������

	// ��ȡ���ݰ�������
	PacketFilterPtr pFilter() const { return pFilter_; }
	// ����һ�����ݰ�������
	void pFilter(PacketFilterPtr pFilter) { pFilter_ = pFilter; }

	void destroy();
	bool isDestroyed() const { return (flags_ & FLAG_DESTROYED) > 0; }

	// ��ȡ��ǰ���������ӿڵ�����
	NetworkInterface & networkInterface()			{ return *pNetworkInterface_; }
	NetworkInterface* pNetworkInterface()			{ return pNetworkInterface_; }
	void pNetworkInterface(NetworkInterface* pNetworkInterface) { pNetworkInterface_ = pNetworkInterface; }

	INLINE const Address& addr() const; //  ��ȡ��ַ
	void pEndPoint(const EndPoint* pEndPoint); //  ����EndPointָ��
	INLINE EndPoint * pEndPoint() const; //  ��ȡEndPointָ��

	typedef std::vector<Bundle*> Bundles;
	Bundles & bundles();	//  ��ȡBundle�б�
	const Bundles & bundles() const;

	/**
		��������bundle����bundle�����Ǵ�send���뷢�Ͷ����л�ȡ�ģ��������Ϊ��
		�򴴽�һ���µ�
	*/
	Bundle* createSendBundle();
	void clearBundle();

	int32 bundlesLength(); //  ��ȡBundle�б�ĳ���

	INLINE void pushBundle(Bundle* pBundle); //  ��Bundle���뷢�Ͷ���
	
	bool sending() const; //  �ж��Ƿ����ڷ���
	void stopSend(); //  ֹͣ����

	void send(Bundle* pBundle = NULL); //  ����Bundle
	void sendto(bool reliable = true, Bundle* pBundle = NULL); //  ����Bundle����ָ���Ƿ�ɿ�
	void sendCheck(uint32 bundleSize); //  ���Bundle�Ĵ�С

	void delayedSend(); //  �ӳٷ���
	bool waitSend(); //  �ȴ�����?����Ƿ�ȴ�����?

	// ��ȡ KCP ����ָ��
	ikcpcb* pKCP() const {
		return pKCP_;
	}

	// ��ȡ����ȡ��
    INLINE PacketReader* pPacketReader() const;
    // ��ȡ��������
    INLINE PacketSender* pPacketSender() const;
    // ���ð�������
    INLINE void pPacketSender(PacketSender* pPacketSender);
    // ��ȡ��������
    INLINE PacketReceiver* pPacketReceiver() const;
    // ���ð�������
    INLINE void pPacketReceiver(PacketReceiver* pPacketReceiver);

	// ��ȡͨ������
    Traits traits() const { return traits_; }
    // ����Ƿ����ⲿͨ��
    bool isExternal() const { return traits_ == EXTERNAL; }
    // ����Ƿ����ڲ�ͨ��
    bool isInternal() const { return traits_ == INTERNAL; }
		
	// ������յ��İ�
    void onPacketReceived(int bytes);
    // ��������ɵİ�
    void onPacketSent(int bytes, bool sentCompleted);
    // ���������
    void onSendCompleted();

	// ��ȡͨ���ַ�����ʾ
    const char * c_str() const;
    // ��ȡͨ�� ID
    ChannelID id() const { return id_; }
    // ����ͨ�� ID
    void id(ChannelID v) { id_ = v; }

	// ��ȡ���͵İ���
    uint32 numPacketsSent() const { return numPacketsSent_; }
    // ��ȡ���յ��İ���
    uint32 numPacketsReceived() const { return numPacketsReceived_; }
    // ��ȡ���͵��ֽ���
    uint32 numBytesSent() const { return numBytesSent_; }
    // ��ȡ���յ����ֽ���
    uint32 numBytesReceived() const { return numBytesReceived_; }

	// ��ȡ�����յ���ʱ��
    uint64 lastReceivedTime() const { return lastReceivedTime_; }
    // ���������յ���ʱ��
    void updateLastReceivedTime() { lastReceivedTime_ = timestamp(); }

    // ��ӽ��մ����еİ�
    void addReceiveWindow(Packet* pPacket);

	// ��ȡ����쳣��
    uint64 inactivityExceptionPeriod() const { return inactivityExceptionPeriod_; }

    // ���¶�ʱ��
    void updateTick(KBEngine::Network::MessageHandlers* pMsgHandlers);
    // �����
    void processPackets(KBEngine::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);

	// ��ȡͨ��״̬
	uint32 condemn() const
	{
		if ((flags_ & FLAG_CONDEMN_AND_DESTROY) > 0)
			return FLAG_CONDEMN_AND_DESTROY;

		if ((flags_ & FLAG_CONDEMN_AND_WAIT_DESTROY) > 0)
			return FLAG_CONDEMN_AND_WAIT_DESTROY;

		return 0;
	}

	// ���ͨ��Ϊ���Ϸ�
	void condemn(const std::string& reason, bool waitSendCompletedDestroy = false);
	// ��ȡ���Ϊ���Ϸ���ԭ��
	std::string condemnReason() const { return condemnReason_; }

	// ����Ƿ��Ѿ�����
	bool hasHandshake() const { return (flags_ & FLAG_HANDSHAKE) > 0; }

	// ���ñ�־
	void setFlags(bool add, uint32 flag)
	{ 
		if(add)
			flags_ |= flag;
		else
			flags_ &= ~flag;
	}
	
	// ��ȡ���� ID
    ENTITY_ID proxyID() const { return proxyID_; }
    // ���ô��� ID
    void proxyID(ENTITY_ID pid){ proxyID_ = pid; }

    // ��ȡ��չ��Ϣ
    const std::string& extra() const { return strextra_; }
    // ������չ��Ϣ
    void extra(const std::string& s){ strextra_ = s; }

    // ��ȡ��� ID
    COMPONENT_ID componentID() const{ return componentID_; }
    // ������� ID
    void componentID(COMPONENT_ID cid){ componentID_ = cid; }

    // ��������
    bool handshake(Packet* pPacket);

	// ��ȡ��Ϣ������
    KBEngine::Network::MessageHandlers* pMsgHandlers() const { return pMsgHandlers_; }
    // ������Ϣ������
    void pMsgHandlers(KBEngine::Network::MessageHandlers* pMsgHandlers) { pMsgHandlers_ = pMsgHandlers; }

	bool initialize(NetworkInterface & networkInterface, 
		const EndPoint * pEndPoint, 
		Traits traits, 
		ProtocolType pt = PROTOCOL_TCP, 
		ProtocolSubType spt = SUB_PROTOCOL_DEFAULT,
		PacketFilterPtr pFilter = NULL, 
		ChannelID id = CHANNEL_ID_NULL);

	bool finalise(); //  ����

	ChannelTypes type() const {
		return channelType_;;
	}

	bool init_kcp(); //  ��ʼ��kcp
	bool fina_kcp(); //  ����kcp
	void kcpUpdate(); //  ����kcp
	void addKcpUpdate(int64 microseconds = 1); //  ���kcp����

	// ��ȡЭ������
    ProtocolType protocoltype() const { return protocoltype_; }
    // ��ȡЭ��������
    ProtocolSubType protocolSubtype() const { return protocolSubtype_; }

    // ����Э������
    void protocoltype(ProtocolType v) { protocoltype_ = v; }
    // ����Э��������
    void protocolSubtype(ProtocolSubType v) { protocolSubtype_ = v; }

	/**
		round-trip time (RTT) Microseconds
		��ȡ����ʱ�䣨RTT��΢��
	*/
	uint32 getRTT();

private:
	static int kcp_output(const char *buf, int len, ikcpcb *kcp, void *user); //  KCP ����ص�����
	static void kcp_writeLog(const char *log, struct IKCPCB *kcp, void *user); //  KCP ��־�ص�����

private:

	enum TimeOutType //  ��ʱ����ö��
	{
		TIMEOUT_INACTIVITY_CHECK = 0, //  �����鳬ʱ
		KCP_UPDATE = 1 //  kcp���³�ʱ
	};

	virtual void handleTimeout(TimerHandle, void * pUser); //  ����ʱ����
	void clearState( bool warnOnDiscard = false ); //  ���״̬����
	EventDispatcher & dispatcher(); //  ��ȡ�¼�������

private:
	NetworkInterface * 			pNetworkInterface_; //  ����ӿ�ָ��

	Traits						traits_; //  ͨ������
	ProtocolType				protocoltype_; //  Э������
	ProtocolSubType				protocolSubtype_; //  Э��������

	ChannelID					id_; //  ͨ��ID
	
	TimerHandle					inactivityTimerHandle_; //  �����ʱ�����
	
	uint64						inactivityExceptionPeriod_; //  ����쳣����
	
	uint64						lastReceivedTime_; //  ������ʱ��
	
	Bundles						bundles_; //  ���ݰ�����
	
	uint32						lastTickBufferedReceives_;	// �ϴλ�����յ�����

	PacketReader*				pPacketReader_; //  ���ݰ���ȡ��ָ��

	// Statistics
	uint32						numPacketsSent_; //  ���͵����ݰ�����
	uint32						numPacketsReceived_; //  ���յ����ݰ�����
	uint32						numBytesSent_; //  ���͵��ֽ���
	uint32						numBytesReceived_; //  ���յ��ֽ���
	uint32						lastTickBytesReceived_; //  ���һ�ν��յ��ֽ���
	uint32						lastTickBytesSent_; //  ���һ�η��͵��ֽ���

	PacketFilterPtr				pFilter_;	// ���ݰ�������ָ��
	
	EndPoint *					pEndPoint_;	// �˵�ָ��
	PacketReceiver*				pPacketReceiver_;	// ���ݰ�������ָ��
	PacketSender*				pPacketSender_;	// ���ݰ�������ָ��

	// ������ⲿͨ���Ҵ�����һ��ǰ������ǰ�˴���ID
	ENTITY_ID					proxyID_;

	// ��չ��
	std::string					strextra_; //  ��չ�ַ���

	// ͨ�����
	ChannelTypes				channelType_;

	COMPONENT_ID				componentID_; //  ���ID

	// ֧��ָ��ĳ��ͨ��ʹ��ĳ����Ϣhandlers
	KBEngine::Network::MessageHandlers* pMsgHandlers_; //  ��Ϣ������

	uint32						flags_;	// ͨ����־λ

	ikcpcb*						pKCP_;	// KCP ���ƿ�ָ��
	TimerHandle					kcpUpdateTimerHandle_;	// KCP ���¶�ʱ�����
	bool						hasSetNextKcpUpdate_;	// �Ƿ���������һ��KCP����

	std::string					condemnReason_;	// ͨ�������Ϊ���Ϸ���ԭ��
};

}
}

#ifdef CODE_INLINE
#include "channel.inl"
#endif
#endif // KBE_NETWORKCHANCEL_H
