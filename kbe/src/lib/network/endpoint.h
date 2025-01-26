// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ENDPOINT_H
#define KBE_ENDPOINT_H

#include "common/common.h"
#include "common/objectpool.h"
#include "helper/debug_helper.h"
#include "network/address.h"
#include "network/common.h"
#include "openssl/ssl.h"

namespace KBEngine { 
namespace Network
{

class Bundle;
class EndPoint : public PoolObject
{
public:
	typedef KBEShared_ptr< SmartPoolObject< EndPoint > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<EndPoint>& ObjPool();
	static EndPoint* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(EndPoint* obj); //  回收对象池对象
	static void destroyObjPool(); //  销毁对象池
	void onReclaimObject(); //  回收对象

	// 获取池对象占用的字节数
	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(KBESOCKET)
		 + address_.getPoolObjectBytes();

		return bytes;
	}

	EndPoint(Address address);
	EndPoint(u_int32_t networkAddr = 0, u_int16_t networkPort = 0);
	virtual ~EndPoint();

	// 转换为 KBESOCKET 类型
	INLINE operator KBESOCKET() const;
	
	static void initNetwork(); //  初始化网络
	INLINE bool good() const; //  判断EndPoint对象是否有效
		
	// 创建套接字
    void socket(int type);
    // 获取套接字
    INLINE KBESOCKET socket() const;
	
	INLINE void setFileDescriptor(int fd); //  设置文件描述符

	INLINE int joinMulticastGroup(u_int32_t networkAddr); //  加入多播组
	INLINE int quitMulticastGroup(u_int32_t networkAddr); //  退出多播组
	
	INLINE int close(); //  关闭socket
	
	INLINE int setnonblocking(bool nonblocking); //  设置非阻塞模式
	INLINE int setbroadcast(bool broadcast); //  设置广播模式
	INLINE int setreuseaddr(bool reuseaddr); //  设置地址重用
	INLINE int setkeepalive(bool keepalive); //  设置是否保持连接
	INLINE int setnodelay(bool nodelay = true); //  设置是否延迟发送
	INLINE int setlinger(uint16 onoff, uint16 linger); //  设置是否延迟关闭

	// 绑定端口
    INLINE int bind(u_int16_t networkPort = 0, u_int32_t networkAddr = INADDR_ANY);

    // 监听连接
    INLINE int listen(int backlog = 5);

    // 连接到指定地址和端口
    INLINE int connect(u_int16_t networkPort, u_int32_t networkAddr = INADDR_BROADCAST, bool autosetflags = true);
    // 连接到默认地址
    INLINE int connect(bool autosetflags = true);

    // 接受连接
    INLINE EndPoint* accept(u_int16_t * networkPort = NULL, u_int32_t * networkAddr = NULL, bool autosetflags = true);

    // 发送数据
    INLINE int send(const void * gramData, int gramSize);
    // 发送 Bundle 对象
    void send(Bundle * pBundle);

    // 接收数据
    INLINE int recv(void * gramData, int gramSize);
    // 接收所有数据
    bool recvAll(void * gramData, int gramSize);

    // 获取往返时间
    INLINE uint32 getRTT();

	INLINE int getInterfaceFlags(char * name, int & flags); //  获取接口标志
	INLINE int getInterfaceAddress(const char * name, u_int32_t & address); //  获取接口地址
	INLINE int getInterfaceNetmask(const char * name, u_int32_t & netmask); //  获取接口子网掩码
	bool getInterfaces(std::map< u_int32_t, std::string > &interfaces); //  获取所有接口

	int findIndicatedInterface(const char * spec, u_int32_t & address); //  根据指定接口获取地址
	int findDefaultInterface(char * name, int buffsize); //  获取默认接口

	// 通过名称获取接口地址
    int getInterfaceAddressByName(const char * name, u_int32_t & address);
    // 通过 MAC 地址获取接口地址
    int getInterfaceAddressByMAC(const char * mac, u_int32_t & address);
    // 获取默认接口地址
    int getDefaultInterfaceAddress(u_int32_t & address);

	// 获取缓冲区大小
    int getBufferSize(int optname) const;
    // 设置缓冲区大小
    bool setBufferSize(int optname, int size);

    // 获取本地地址
    INLINE int getlocaladdress(u_int16_t * networkPort, u_int32_t * networkAddr) const;
    // 获取远程地址
    INLINE int getremoteaddress(u_int16_t * networkPort, u_int32_t * networkAddr) const;

    // 获取本地地址
    Network::Address getLocalAddress() const;
    // 获取远程地址
    Network::Address getRemoteAddress() const;

	// 获取关闭的端口
    bool getClosedPort(Network::Address & closedPort);

    // 获取字符串表示
    INLINE const char * c_str() const;
    // 获取远程主机名
    INLINE int getremotehostname(std::string * name) const;

    // 发送到指定地址和端口
    INLINE int sendto(void * gramData, int gramSize);
    // 发送到指定地址和端口
    INLINE int sendto(void * gramData, int gramSize, u_int16_t networkPort, u_int32_t networkAddr = BROADCAST);
    // 发送到指定地址
    INLINE int sendto(void * gramData, int gramSize, struct sockaddr_in & sin);
    // 发送 Bundle 对象到指定地址和端口
    void sendto(Bundle * pBundle, u_int16_t networkPort, u_int32_t networkAddr = BROADCAST);

    // 从指定地址接收数据
    INLINE int recvfrom(void * gramData, int gramSize, u_int16_t * networkPort, u_int32_t * networkAddr);
    // 从指定地址接收数据
    INLINE int recvfrom(void * gramData, int gramSize, struct sockaddr_in & sin);

    // 获取地址
    INLINE const Address& addr() const;
    // 设置地址
    INLINE void addr(const Address& newAddress);
    // 设置地址
    INLINE void addr(u_int16_t newNetworkPort, u_int32_t newNetworkAddress);

    // 等待发送
    bool waitSend();

	// 设置套接字引用
    void setSocketRef(KBESOCKET s) {
        socket_ = s;
        isRefSocket_ = true;
    }

    // 设置 SSL
    bool setupSSL(int sslVersion, Packet* pPacket);

    // 销毁 SSL
    bool destroySSL();

    // 检查是否使用 SSL
    bool isSSL() const {
        return sslHandle_ != NULL;
    }

protected:
	KBESOCKET socket_; //  套接字
	Address address_; //  地址
	SSL* sslHandle_; //  SSL处理器
	SSL_CTX* sslContext_; //  SSL上下文
	bool isRefSocket_; //  是否引用套接字
};

}
}

#ifdef CODE_INLINE
#include "endpoint.inl"
#endif
#endif // KBE_ENDPOINT_H
