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
	static void reclaimPoolObject(EndPoint* obj); //  ���ն���ض���
	static void destroyObjPool(); //  ���ٶ����
	void onReclaimObject(); //  ���ն���

	// ��ȡ�ض���ռ�õ��ֽ���
	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(KBESOCKET)
		 + address_.getPoolObjectBytes();

		return bytes;
	}

	EndPoint(Address address);
	EndPoint(u_int32_t networkAddr = 0, u_int16_t networkPort = 0);
	virtual ~EndPoint();

	// ת��Ϊ KBESOCKET ����
	INLINE operator KBESOCKET() const;
	
	static void initNetwork(); //  ��ʼ������
	INLINE bool good() const; //  �ж�EndPoint�����Ƿ���Ч
		
	// �����׽���
    void socket(int type);
    // ��ȡ�׽���
    INLINE KBESOCKET socket() const;
	
	INLINE void setFileDescriptor(int fd); //  �����ļ�������

	INLINE int joinMulticastGroup(u_int32_t networkAddr); //  ����ಥ��
	INLINE int quitMulticastGroup(u_int32_t networkAddr); //  �˳��ಥ��
	
	INLINE int close(); //  �ر�socket
	
	INLINE int setnonblocking(bool nonblocking); //  ���÷�����ģʽ
	INLINE int setbroadcast(bool broadcast); //  ���ù㲥ģʽ
	INLINE int setreuseaddr(bool reuseaddr); //  ���õ�ַ����
	INLINE int setkeepalive(bool keepalive); //  �����Ƿ񱣳�����
	INLINE int setnodelay(bool nodelay = true); //  �����Ƿ��ӳٷ���
	INLINE int setlinger(uint16 onoff, uint16 linger); //  �����Ƿ��ӳٹر�

	// �󶨶˿�
    INLINE int bind(u_int16_t networkPort = 0, u_int32_t networkAddr = INADDR_ANY);

    // ��������
    INLINE int listen(int backlog = 5);

    // ���ӵ�ָ����ַ�Ͷ˿�
    INLINE int connect(u_int16_t networkPort, u_int32_t networkAddr = INADDR_BROADCAST, bool autosetflags = true);
    // ���ӵ�Ĭ�ϵ�ַ
    INLINE int connect(bool autosetflags = true);

    // ��������
    INLINE EndPoint* accept(u_int16_t * networkPort = NULL, u_int32_t * networkAddr = NULL, bool autosetflags = true);

    // ��������
    INLINE int send(const void * gramData, int gramSize);
    // ���� Bundle ����
    void send(Bundle * pBundle);

    // ��������
    INLINE int recv(void * gramData, int gramSize);
    // ������������
    bool recvAll(void * gramData, int gramSize);

    // ��ȡ����ʱ��
    INLINE uint32 getRTT();

	INLINE int getInterfaceFlags(char * name, int & flags); //  ��ȡ�ӿڱ�־
	INLINE int getInterfaceAddress(const char * name, u_int32_t & address); //  ��ȡ�ӿڵ�ַ
	INLINE int getInterfaceNetmask(const char * name, u_int32_t & netmask); //  ��ȡ�ӿ���������
	bool getInterfaces(std::map< u_int32_t, std::string > &interfaces); //  ��ȡ���нӿ�

	int findIndicatedInterface(const char * spec, u_int32_t & address); //  ����ָ���ӿڻ�ȡ��ַ
	int findDefaultInterface(char * name, int buffsize); //  ��ȡĬ�Ͻӿ�

	// ͨ�����ƻ�ȡ�ӿڵ�ַ
    int getInterfaceAddressByName(const char * name, u_int32_t & address);
    // ͨ�� MAC ��ַ��ȡ�ӿڵ�ַ
    int getInterfaceAddressByMAC(const char * mac, u_int32_t & address);
    // ��ȡĬ�Ͻӿڵ�ַ
    int getDefaultInterfaceAddress(u_int32_t & address);

	// ��ȡ��������С
    int getBufferSize(int optname) const;
    // ���û�������С
    bool setBufferSize(int optname, int size);

    // ��ȡ���ص�ַ
    INLINE int getlocaladdress(u_int16_t * networkPort, u_int32_t * networkAddr) const;
    // ��ȡԶ�̵�ַ
    INLINE int getremoteaddress(u_int16_t * networkPort, u_int32_t * networkAddr) const;

    // ��ȡ���ص�ַ
    Network::Address getLocalAddress() const;
    // ��ȡԶ�̵�ַ
    Network::Address getRemoteAddress() const;

	// ��ȡ�رյĶ˿�
    bool getClosedPort(Network::Address & closedPort);

    // ��ȡ�ַ�����ʾ
    INLINE const char * c_str() const;
    // ��ȡԶ��������
    INLINE int getremotehostname(std::string * name) const;

    // ���͵�ָ����ַ�Ͷ˿�
    INLINE int sendto(void * gramData, int gramSize);
    // ���͵�ָ����ַ�Ͷ˿�
    INLINE int sendto(void * gramData, int gramSize, u_int16_t networkPort, u_int32_t networkAddr = BROADCAST);
    // ���͵�ָ����ַ
    INLINE int sendto(void * gramData, int gramSize, struct sockaddr_in & sin);
    // ���� Bundle ����ָ����ַ�Ͷ˿�
    void sendto(Bundle * pBundle, u_int16_t networkPort, u_int32_t networkAddr = BROADCAST);

    // ��ָ����ַ��������
    INLINE int recvfrom(void * gramData, int gramSize, u_int16_t * networkPort, u_int32_t * networkAddr);
    // ��ָ����ַ��������
    INLINE int recvfrom(void * gramData, int gramSize, struct sockaddr_in & sin);

    // ��ȡ��ַ
    INLINE const Address& addr() const;
    // ���õ�ַ
    INLINE void addr(const Address& newAddress);
    // ���õ�ַ
    INLINE void addr(u_int16_t newNetworkPort, u_int32_t newNetworkAddress);

    // �ȴ�����
    bool waitSend();

	// �����׽�������
    void setSocketRef(KBESOCKET s) {
        socket_ = s;
        isRefSocket_ = true;
    }

    // ���� SSL
    bool setupSSL(int sslVersion, Packet* pPacket);

    // ���� SSL
    bool destroySSL();

    // ����Ƿ�ʹ�� SSL
    bool isSSL() const {
        return sslHandle_ != NULL;
    }

protected:
	KBESOCKET socket_; //  �׽���
	Address address_; //  ��ַ
	SSL* sslHandle_; //  SSL������
	SSL_CTX* sslContext_; //  SSL������
	bool isRefSocket_; //  �Ƿ������׽���
};

}
}

#ifdef CODE_INLINE
#include "endpoint.inl"
#endif
#endif // KBE_ENDPOINT_H
