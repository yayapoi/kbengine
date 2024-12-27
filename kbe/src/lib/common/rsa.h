// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBENGINE_RSA_H
#define KBENGINE_RSA_H

#include <string>

namespace KBEngine
{


/**
 *	openssl rsa�ķ�װ
 */
class KBE_RSA
{
public:
	KBE_RSA(const std::string& pubkeyname, 
		const std::string& prikeyname);

	KBE_RSA();

	virtual ~KBE_RSA();

	//����һ���µ� RSA ��Կ��������Կ��˽Կ�ֱ𱣴浽ָ�����ļ��С�Ĭ����Կ��СΪ 1024 λ����Կָ��Ϊ 65537
	bool generateKey(const std::string& pubkeyname, 
		const std::string& prikeyname, int keySize = 1024, int e = 65537);

	//ʹ�ù�Կ�������ַ������м��ܣ������ؼ��ܺ���ַ���
	std::string encrypt(const std::string& instr);
	//ʹ�ù�Կ�������ַ������м��ܣ���������洢�� outCertifdata �У����ز������
	int encrypt(const std::string& instr, std::string& outCertifdata);
	//ʹ��˽Կ�������ַ������н��ܣ���������洢�� outstr �У����ز������
	int decrypt(const std::string& inCertifdata, std::string& outstr);
	//ʹ��˽Կ�������ַ������н��ܣ������ؽ��ܺ���ַ���
	std::string decrypt(const std::string& instr);

	//�������֤������ת��Ϊʮ�������ַ���
	static void hexCertifData(const std::string& inCertifdata);

	//��ָ�����ļ��м��ع�Կ
	bool loadPublic(const std::string& keyname);
	bool loadPrivate(const std::string& keyname);

	virtual bool isGood() const { return rsa_public != NULL && rsa_private != NULL; }

protected:
	void* rsa_public, *rsa_private;	//ָ��Կ��˽Կ��ָ��
};


}

#endif // KBENGINE_RSA_H
