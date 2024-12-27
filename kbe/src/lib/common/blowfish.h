// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBENGINE_BLOWFISH_H
#define KBENGINE_BLOWFISH_H

#include "openssl/blowfish.h"
#include <string>

namespace KBEngine { 

/**
* 该类使用 OpenSSL 库中的 Blowfish 加密算法来实现加密和解密功能
*/
class KBEBlowfish
{
public:
	// 每块大小
	static const int BLOCK_SIZE = 64 / 8;

	// key的最小和最大大小
	static const int MIN_KEY_SIZE = 32 / 8;
	static const int MAX_KEY_SIZE = 448 / 8;

	// 默认key的大小
	static const int DEFAULT_KEY_SIZE = 128 / 8;

	typedef std::string Key;

	virtual ~KBEBlowfish();
	KBEBlowfish(const Key & key);
	KBEBlowfish(int keySize = DEFAULT_KEY_SIZE);

	const Key & key() const { return key_; }	//返回当前使用的密钥
	const char * strBlowFishKey() const;		//返回密钥的字符串表示
	bool isGood() const { return isGood_; }

	int encrypt(const unsigned char * src, unsigned char * dest, int length);	//对指定的数据进行加密
	int decrypt(const unsigned char * src, unsigned char * dest, int length);

	BF_KEY * pBlowFishKey() { return (BF_KEY*)pBlowFishKey_; }	//返回指向 BF_KEY 结构的指针
protected:
	bool init();		//初始化对象，设置密钥等

	Key key_;			//存储加密密钥的字符串
	int keySize_;		//密钥的大小（以字节为单位）
	bool isGood_;		//表示对象是否初始化成功

	void * pBlowFishKey_;	//指向 OpenSSL 中 BF_KEY 结构的指针，用于存储 Blowfish 算法的密钥信息
};

}

#endif // KBENGINE_BLOWFISH_H
