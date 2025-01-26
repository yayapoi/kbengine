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
	static void reclaimPoolObject(Channel* obj); //  回收对象池对象
	static void destroyObjPool(); //  销毁对象池
	virtual void onReclaimObject(); //  回收对象时调用
	virtual size_t getPoolObjectBytes(); //  获取对象池对象字节数
	virtual void onEabledPoolObject(); //  启用对象池对象时调用

	enum Traits //  枚举类型，描述通道属性
	{
		/// This describes the properties of channel from server to server.
		INTERNAL = 0,// 服务器到服务器

		/// This describes the properties of a channel from client to server.
		EXTERNAL = 1,// 客户端到服务器
	};
	
	enum ChannelTypes
	{
		/// 普通通道
		CHANNEL_NORMAL = 0,

		// 浏览器web通道
		CHANNEL_WEB = 1,
	};

	enum Flags
	{
		FLAG_SENDING					= 0x00000001,	// 发送信息中
		FLAG_DESTROYED					= 0x00000002,	// 通道已经销毁
		FLAG_HANDSHAKE					= 0x00000004,	// 已经握手过
		FLAG_CONDEMN_AND_WAIT_DESTROY	= 0x00000008,	// 该频道已经变得不合法，即将在数据发送完毕后关闭
		FLAG_CONDEMN_AND_DESTROY		= 0x00000010,	// 该频道已经变得不合法，即将关闭
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
	
	 //  开始不活动检测，inactivityPeriod为不活动时间，checkPeriod为检查周期
	void startInactivityDetection( float inactivityPeriod,
			float checkPeriod = 1.f );
	
	void stopInactivityDetection(); //  停止不活动检测

	// 获取数据包过滤器
	PacketFilterPtr pFilter() const { return pFilter_; }
	// 设置一个数据包过滤器
	void pFilter(PacketFilterPtr pFilter) { pFilter_ = pFilter; }

	void destroy();
	bool isDestroyed() const { return (flags_ & FLAG_DESTROYED) > 0; }

	// 获取当前对象的网络接口的引用
	NetworkInterface & networkInterface()			{ return *pNetworkInterface_; }
	NetworkInterface* pNetworkInterface()			{ return pNetworkInterface_; }
	void pNetworkInterface(NetworkInterface* pNetworkInterface) { pNetworkInterface_ = pNetworkInterface; }

	INLINE const Address& addr() const; //  获取地址
	void pEndPoint(const EndPoint* pEndPoint); //  设置EndPoint指针
	INLINE EndPoint * pEndPoint() const; //  获取EndPoint指针

	typedef std::vector<Bundle*> Bundles;
	Bundles & bundles();	//  获取Bundle列表
	const Bundles & bundles() const;

	/**
		创建发送bundle，该bundle可能是从send放入发送队列中获取的，如果队列为空
		则创建一个新的
	*/
	Bundle* createSendBundle();
	void clearBundle();

	int32 bundlesLength(); //  获取Bundle列表的长度

	INLINE void pushBundle(Bundle* pBundle); //  将Bundle推入发送队列
	
	bool sending() const; //  判断是否正在发送
	void stopSend(); //  停止发送

	void send(Bundle* pBundle = NULL); //  发送Bundle
	void sendto(bool reliable = true, Bundle* pBundle = NULL); //  发送Bundle，并指定是否可靠
	void sendCheck(uint32 bundleSize); //  检查Bundle的大小

	void delayedSend(); //  延迟发送
	bool waitSend(); //  等待发送?检查是否等待发送?

	// 获取 KCP 对象指针
	ikcpcb* pKCP() const {
		return pKCP_;
	}

	// 获取包读取器
    INLINE PacketReader* pPacketReader() const;
    // 获取包发送器
    INLINE PacketSender* pPacketSender() const;
    // 设置包发送器
    INLINE void pPacketSender(PacketSender* pPacketSender);
    // 获取包接收器
    INLINE PacketReceiver* pPacketReceiver() const;
    // 设置包接收器
    INLINE void pPacketReceiver(PacketReceiver* pPacketReceiver);

	// 获取通道属性
    Traits traits() const { return traits_; }
    // 检查是否是外部通道
    bool isExternal() const { return traits_ == EXTERNAL; }
    // 检查是否是内部通道
    bool isInternal() const { return traits_ == INTERNAL; }
		
	// 处理接收到的包
    void onPacketReceived(int bytes);
    // 处理发送完成的包
    void onPacketSent(int bytes, bool sentCompleted);
    // 处理发送完成
    void onSendCompleted();

	// 获取通道字符串表示
    const char * c_str() const;
    // 获取通道 ID
    ChannelID id() const { return id_; }
    // 设置通道 ID
    void id(ChannelID v) { id_ = v; }

	// 获取发送的包数
    uint32 numPacketsSent() const { return numPacketsSent_; }
    // 获取接收到的包数
    uint32 numPacketsReceived() const { return numPacketsReceived_; }
    // 获取发送的字节数
    uint32 numBytesSent() const { return numBytesSent_; }
    // 获取接收到的字节数
    uint32 numBytesReceived() const { return numBytesReceived_; }

	// 获取最后接收到的时间
    uint64 lastReceivedTime() const { return lastReceivedTime_; }
    // 更新最后接收到的时间
    void updateLastReceivedTime() { lastReceivedTime_ = timestamp(); }

    // 添加接收窗口中的包
    void addReceiveWindow(Packet* pPacket);

	// 获取不活动异常期
    uint64 inactivityExceptionPeriod() const { return inactivityExceptionPeriod_; }

    // 更新定时器
    void updateTick(KBEngine::Network::MessageHandlers* pMsgHandlers);
    // 处理包
    void processPackets(KBEngine::Network::MessageHandlers* pMsgHandlers, Packet* pPacket);

	// 获取通道状态
	uint32 condemn() const
	{
		if ((flags_ & FLAG_CONDEMN_AND_DESTROY) > 0)
			return FLAG_CONDEMN_AND_DESTROY;

		if ((flags_ & FLAG_CONDEMN_AND_WAIT_DESTROY) > 0)
			return FLAG_CONDEMN_AND_WAIT_DESTROY;

		return 0;
	}

	// 标记通道为不合法
	void condemn(const std::string& reason, bool waitSendCompletedDestroy = false);
	// 获取标记为不合法的原因
	std::string condemnReason() const { return condemnReason_; }

	// 检查是否已经握手
	bool hasHandshake() const { return (flags_ & FLAG_HANDSHAKE) > 0; }

	// 设置标志
	void setFlags(bool add, uint32 flag)
	{ 
		if(add)
			flags_ |= flag;
		else
			flags_ &= ~flag;
	}
	
	// 获取代理 ID
    ENTITY_ID proxyID() const { return proxyID_; }
    // 设置代理 ID
    void proxyID(ENTITY_ID pid){ proxyID_ = pid; }

    // 获取扩展信息
    const std::string& extra() const { return strextra_; }
    // 设置扩展信息
    void extra(const std::string& s){ strextra_ = s; }

    // 获取组件 ID
    COMPONENT_ID componentID() const{ return componentID_; }
    // 设置组件 ID
    void componentID(COMPONENT_ID cid){ componentID_ = cid; }

    // 处理握手
    bool handshake(Packet* pPacket);

	// 获取消息处理器
    KBEngine::Network::MessageHandlers* pMsgHandlers() const { return pMsgHandlers_; }
    // 设置消息处理器
    void pMsgHandlers(KBEngine::Network::MessageHandlers* pMsgHandlers) { pMsgHandlers_ = pMsgHandlers; }

	bool initialize(NetworkInterface & networkInterface, 
		const EndPoint * pEndPoint, 
		Traits traits, 
		ProtocolType pt = PROTOCOL_TCP, 
		ProtocolSubType spt = SUB_PROTOCOL_DEFAULT,
		PacketFilterPtr pFilter = NULL, 
		ChannelID id = CHANNEL_ID_NULL);

	bool finalise(); //  结束

	ChannelTypes type() const {
		return channelType_;;
	}

	bool init_kcp(); //  初始化kcp
	bool fina_kcp(); //  结束kcp
	void kcpUpdate(); //  更新kcp
	void addKcpUpdate(int64 microseconds = 1); //  添加kcp更新

	// 获取协议类型
    ProtocolType protocoltype() const { return protocoltype_; }
    // 获取协议子类型
    ProtocolSubType protocolSubtype() const { return protocolSubtype_; }

    // 设置协议类型
    void protocoltype(ProtocolType v) { protocoltype_ = v; }
    // 设置协议子类型
    void protocolSubtype(ProtocolSubType v) { protocolSubtype_ = v; }

	/**
		round-trip time (RTT) Microseconds
		获取往返时间（RTT）微秒
	*/
	uint32 getRTT();

private:
	static int kcp_output(const char *buf, int len, ikcpcb *kcp, void *user); //  KCP 输出回调函数
	static void kcp_writeLog(const char *log, struct IKCPCB *kcp, void *user); //  KCP 日志回调函数

private:

	enum TimeOutType //  超时类型枚举
	{
		TIMEOUT_INACTIVITY_CHECK = 0, //  不活动检查超时
		KCP_UPDATE = 1 //  kcp更新超时
	};

	virtual void handleTimeout(TimerHandle, void * pUser); //  处理超时函数
	void clearState( bool warnOnDiscard = false ); //  清除状态函数
	EventDispatcher & dispatcher(); //  获取事件调度器

private:
	NetworkInterface * 			pNetworkInterface_; //  网络接口指针

	Traits						traits_; //  通道属性
	ProtocolType				protocoltype_; //  协议类型
	ProtocolSubType				protocolSubtype_; //  协议子类型

	ChannelID					id_; //  通道ID
	
	TimerHandle					inactivityTimerHandle_; //  不活动定时器句柄
	
	uint64						inactivityExceptionPeriod_; //  不活动异常周期
	
	uint64						lastReceivedTime_; //  最后接收时间
	
	Bundles						bundles_; //  数据包集合
	
	uint32						lastTickBufferedReceives_;	// 上次缓冲接收的数量

	PacketReader*				pPacketReader_; //  数据包读取器指针

	// Statistics
	uint32						numPacketsSent_; //  发送的数据包数量
	uint32						numPacketsReceived_; //  接收的数据包数量
	uint32						numBytesSent_; //  发送的字节数
	uint32						numBytesReceived_; //  接收的字节数
	uint32						lastTickBytesReceived_; //  最后一次接收的字节数
	uint32						lastTickBytesSent_; //  最后一次发送的字节数

	PacketFilterPtr				pFilter_;	// 数据包过滤器指针
	
	EndPoint *					pEndPoint_;	// 端点指针
	PacketReceiver*				pPacketReceiver_;	// 数据包接收器指针
	PacketSender*				pPacketSender_;	// 数据包发送器指针

	// 如果是外部通道且代理了一个前端则会绑定前端代理ID
	ENTITY_ID					proxyID_;

	// 扩展用
	std::string					strextra_; //  扩展字符串

	// 通道类别
	ChannelTypes				channelType_;

	COMPONENT_ID				componentID_; //  组件ID

	// 支持指定某个通道使用某个消息handlers
	KBEngine::Network::MessageHandlers* pMsgHandlers_; //  消息处理器

	uint32						flags_;	// 通道标志位

	ikcpcb*						pKCP_;	// KCP 控制块指针
	TimerHandle					kcpUpdateTimerHandle_;	// KCP 更新定时器句柄
	bool						hasSetNextKcpUpdate_;	// 是否设置了下一个KCP更新

	std::string					condemnReason_;	// 通道被标记为不合法的原因
};

}
}

#ifdef CODE_INLINE
#include "channel.inl"
#endif
#endif // KBE_NETWORKCHANCEL_H
