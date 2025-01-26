// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ADDRESS_H
#define KBE_ADDRESS_H

#include "common/common.h"
#include "common/objectpool.h"
#include "network/common.h"

namespace KBEngine { 
namespace Network
{
class Address  : public PoolObject
{
public:
	static const Address NONE; //  ����һ����̬��������ʾһ���յĵ�ַ

	typedef KBEShared_ptr< SmartPoolObject< Address > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<Address>& ObjPool();
	static Address* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(Address* obj); //  ���ն���ض���
	static void destroyObjPool(); //  ���ٶ����
	void onReclaimObject(); //  ���ն���

	virtual size_t getPoolObjectBytes()
	{
		size_t bytes = sizeof(ip)
		 + sizeof(port);

		return bytes;
	}

	Address();
	Address(uint32 ipArg, uint16 portArg);
	Address(std::string ipArg, uint16 portArg);
	Address(const Address& addr);

	virtual ~Address();

	uint32	ip;
	uint16	port;

	// ����ַд���ַ���
    int writeToString(char * str, int length) const;

    // ת��Ϊ C �ַ���
    operator char*() const { return this->c_str(); }

    // ��ȡ C �ַ�����ʽ�ĵ�ַ
    char * c_str() const;
    // ��ȡ�ַ�����ʽ�� IP ��ַ
    const char * ipAsString() const;
    // �ж��Ƿ�Ϊ��Ч��ַ
    bool isNone() const { return this->ip == 0; }

    // ���ַ���ת��Ϊ IP ��ַ
    static int string2ip(const char * string, u_int32_t & address);
    // �� IP ��ַת��Ϊ�ַ���
    static int ip2string(u_int32_t address, char * string);

private:
	static char s_stringBuf[2][32]; //  �ַ���������
	static int s_currStringBuf; //  ��ǰ�ַ�������������
	static char * nextStringBuf(); //  ��ȡ��һ���ַ���������
};

inline Address::Address():
ip(0),
port(0)
{
}

inline Address::Address(uint32 ipArg, uint16 portArg) :
	ip(ipArg),
	port(portArg)
{
} 

// �Ƚϲ���������
inline bool operator==(const Address & a, const Address & b)
{
	return (a.ip == b.ip) && (a.port == b.port);
}

inline bool operator!=(const Address & a, const Address & b)
{
	return (a.ip != b.ip) || (a.port != b.port);
}

inline bool operator<(const Address & a, const Address & b)
{
	return (a.ip < b.ip) || (a.ip == b.ip && (a.port < b.port));
}


}
}
#endif // KBE_ADDRESS_H
