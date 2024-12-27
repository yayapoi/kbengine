// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBENGINE_RSA_H
#define KBENGINE_RSA_H

#include <string>

namespace KBEngine
{


/**
 *	openssl rsa的封装
 */
class KBE_RSA
{
public:
	KBE_RSA(const std::string& pubkeyname, 
		const std::string& prikeyname);

	KBE_RSA();

	virtual ~KBE_RSA();

	//生成一对新的 RSA 密钥，并将公钥和私钥分别保存到指定的文件中。默认密钥大小为 1024 位，公钥指数为 65537
	bool generateKey(const std::string& pubkeyname, 
		const std::string& prikeyname, int keySize = 1024, int e = 65537);

	//使用公钥对输入字符串进行加密，并返回加密后的字符串
	std::string encrypt(const std::string& instr);
	//使用公钥对输入字符串进行加密，并将结果存储在 outCertifdata 中，返回操作结果
	int encrypt(const std::string& instr, std::string& outCertifdata);
	//使用私钥对输入字符串进行解密，并将结果存储在 outstr 中，返回操作结果
	int decrypt(const std::string& inCertifdata, std::string& outstr);
	//使用私钥对输入字符串进行解密，并返回解密后的字符串
	std::string decrypt(const std::string& instr);

	//将输入的证书数据转换为十六进制字符串
	static void hexCertifData(const std::string& inCertifdata);

	//从指定的文件中加载公钥
	bool loadPublic(const std::string& keyname);
	bool loadPrivate(const std::string& keyname);

	virtual bool isGood() const { return rsa_public != NULL && rsa_private != NULL; }

protected:
	void* rsa_public, *rsa_private;	//指向公钥、私钥的指针
};


}

#endif // KBENGINE_RSA_H
