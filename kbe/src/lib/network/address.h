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
	static const Address NONE; //  定义一个静态常量，表示一个空的地址

	typedef KBEShared_ptr< SmartPoolObject< Address > > SmartPoolObjectPtr;
	static SmartPoolObjectPtr createSmartPoolObj(const std::string& logPoint);
	static ObjectPool<Address>& ObjPool();
	static Address* createPoolObject(const std::string& logPoint);
	static void reclaimPoolObject(Address* obj); //  回收对象池对象
	static void destroyObjPool(); //  销毁对象池
	void onReclaimObject(); //  回收对象

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

	// 将地址写入字符串
    int writeToString(char * str, int length) const;

    // 转换为 C 字符串
    operator char*() const { return this->c_str(); }

    // 获取 C 字符串形式的地址
    char * c_str() const;
    // 获取字符串形式的 IP 地址
    const char * ipAsString() const;
    // 判断是否为无效地址
    bool isNone() const { return this->ip == 0; }

    // 将字符串转换为 IP 地址
    static int string2ip(const char * string, u_int32_t & address);
    // 将 IP 地址转换为字符串
    static int ip2string(u_int32_t address, char * string);

private:
	static char s_stringBuf[2][32]; //  字符串缓冲区
	static int s_currStringBuf; //  当前字符串缓冲区索引
	static char * nextStringBuf(); //  获取下一个字符串缓冲区
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

// 比较操作符重载
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
