// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_REDIS_TRANSACTION_HELPER_H
#define KBE_REDIS_TRANSACTION_HELPER_H

#include "hiredis.h"

namespace KBEngine { 
class DBInterface;
namespace redis {

/**
 * @brief DBTransaction �����ڹ��� Redis ����
 * 
 * �����ṩ��һ�ַ���ķ�ʽ����ʼ���ύ�ͻع� Redis ���������ṩ���Զ���������Ĺ��ܣ�
 * ���ҿ��Լ�������Ƿ���Ҫ���ԡ�
 */
class DBTransaction
{
public:
	DBTransaction(DBInterface* pdbi, bool autostart = true);
	~DBTransaction();
	
	/**
     * @brief ��ʼ����
     */
    void start();
    /**
     * @brief ��������
     */
    void end();

    /**
     * @brief �ύ����
     */
    void commit();
    /**
     * @brief �ع�����
     */
    void rollback();
	
	/**
     * @brief ��������Ƿ���Ҫ����
     * 
     * @return ���������Ҫ���Է��� true�����򷵻� false
     */
    bool shouldRetry() const;

    /**
     * @brief ���� DBInterface ָ��
     * 
     * @param pdbi ָ�� DBInterface ��ָ��
     */
    void pdbi(DBInterface* pdbi) { pdbi_ = pdbi; }

    /**
     * @brief ��ȡ Redis �ظ�ָ��
     * 
     * @return ָ�� redisReply ��ָ��
     */
    redisReply* pRedisReply() { return pRedisReply_; }

private:
	DBInterface* pdbi_; //  ���ݿ�ӿ�ָ��
	bool committed_; //  �Ƿ����ύ
	bool autostart_; //  �Ƿ��Զ�����
	redisReply* pRedisReply_;	// ָ�� Redis �ظ���ָ��
};

}
}
#endif // KBE_REDIS_TRANSACTION_HELPER_H

