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
 * @brief UDP数据包类
 * 
 * 该类继承自 Packet，专门用于处理UDP协议的数据包。
 * 它提供了UDP数据包的创建、接收、回收等操作，并支持对象池管理以提高性能。
 */
class UDPPacket : public Packet
{
public:
	/**
     * 智能指针类型定义
     */
	typedef KBEShared_ptr< SmartPoolObject< UDPPacket > > SmartPoolObjectPtr;

	// 创建智能池对象
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<UDPPacket>& ObjPool(); //  获取对象池
	// 从对象池中创建对象
	static UDPPacket* createPoolObject(const std::string& logPoint);
	// 回收对象到对象池
	static void reclaimPoolObject(UDPPacket* obj);
	static void destroyObjPool();
	// 获取最大缓冲区大小
	static size_t maxBufferSize();

    UDPPacket(MessageID msgID = 0, size_t res = 0);
	virtual ~UDPPacket(void);
	
	/**
     * 从端点接收UDP数据包
     * 
     * @param ep 端点对象引用
     * @param pAddr 地址对象指针（可选）
     * @return 接收结果
     */
	int recvFromEndPoint(EndPoint & ep, Address* pAddr = NULL);

	 /**
     * 对象回收时的处理
     * 
     * 该方法在对象被回收时调用，用于重置数据包状态。
     */
	virtual void onReclaimObject();
};

typedef SmartPointer<UDPPacket> UDPPacketPtr;
}
}

#endif // KBE_SOCKETUDPPACKET_H
