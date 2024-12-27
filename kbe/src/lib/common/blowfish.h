// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBENGINE_BLOWFISH_H
#define KBENGINE_BLOWFISH_H

#include "openssl/blowfish.h"
#include <string>

namespace KBEngine { 

/**
* ����ʹ�� OpenSSL ���е� Blowfish �����㷨��ʵ�ּ��ܺͽ��ܹ���
*/
class KBEBlowfish
{
public:
	// ÿ���С
	static const int BLOCK_SIZE = 64 / 8;

	// key����С������С
	static const int MIN_KEY_SIZE = 32 / 8;
	static const int MAX_KEY_SIZE = 448 / 8;

	// Ĭ��key�Ĵ�С
	static const int DEFAULT_KEY_SIZE = 128 / 8;

	typedef std::string Key;

	virtual ~KBEBlowfish();
	KBEBlowfish(const Key & key);
	KBEBlowfish(int keySize = DEFAULT_KEY_SIZE);

	const Key & key() const { return key_; }	//���ص�ǰʹ�õ���Կ
	const char * strBlowFishKey() const;		//������Կ���ַ�����ʾ
	bool isGood() const { return isGood_; }

	int encrypt(const unsigned char * src, unsigned char * dest, int length);	//��ָ�������ݽ��м���
	int decrypt(const unsigned char * src, unsigned char * dest, int length);

	BF_KEY * pBlowFishKey() { return (BF_KEY*)pBlowFishKey_; }	//����ָ�� BF_KEY �ṹ��ָ��
protected:
	bool init();		//��ʼ������������Կ��

	Key key_;			//�洢������Կ���ַ���
	int keySize_;		//��Կ�Ĵ�С�����ֽ�Ϊ��λ��
	bool isGood_;		//��ʾ�����Ƿ��ʼ���ɹ�

	void * pBlowFishKey_;	//ָ�� OpenSSL �� BF_KEY �ṹ��ָ�룬���ڴ洢 Blowfish �㷨����Կ��Ϣ
};

}

#endif // KBENGINE_BLOWFISH_H
