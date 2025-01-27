// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_REDIS_HELPER_H
#define KBE_REDIS_HELPER_H

#include "common.h"
#include "common/common.h"
#include "common/stringconv.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface_redis.h"

namespace KBEngine{ 

/**
 * @brief RedisHelper ���ṩ��һϵ�о�̬������������ Redis ���ݿ���н�����
 */
class RedisHelper
{
public:
	RedisHelper()
	{
	}

	virtual ~RedisHelper()
	{
	}

	/**
     * @brief ���� Redis ���Ĺ���ʱ��
     * 
     * @param pdbi ָ�� DBInterfaceRedis �����ָ��
     * @param key Ҫ���ù���ʱ��ļ�
     * @param secs ����ʱ�䣨�룩
     * @param printlog �Ƿ��ӡ��־
     * @return �ɹ����� true��ʧ�ܷ��� false
     */
	static bool expireKey(DBInterfaceRedis* pdbi, const std::string& key, int secs, bool printlog = true)
	{
		if (!pdbi->query(printlog, "EXPIRE %s %d", key.c_str(), secs))
			return false;
	}
	
	/**
     * @brief ��� Redis �ظ��Ƿ�Ϊ�������ͣ���������Ԫ�ض����������ַ���
     * 
     * @param pRedisReply ָ�� redisReply �����ָ��
     * @return �������Ԫ�ض����������ַ��������� true�����򷵻� false
     */
	static bool check_array_results(redisReply* pRedisReply)
	{
		for(size_t j = 0; j < pRedisReply->elements; ++j) 
		{
			if(pRedisReply->element[j]->type != REDIS_REPLY_INTEGER && 
				pRedisReply->element[j]->type != REDIS_REPLY_STRING)
			{
				return false;
			}
		}
		
		return true;
	}
	
	/**
     * @brief ��� Redis ���Ƿ����ָ���ı�
     * 
     * @param pdbi ָ�� DBInterfaceRedis �����ָ��
     * @param name ����
     * @param printlog �Ƿ��ӡ��־
     * @return �������ָ���ı����� true�����򷵻� false
     */
	static bool hasTable(DBInterfaceRedis* pdbi, const std::string& name, bool printlog = true)
	{
		redisReply* pRedisReply = NULL;
		
		if (!pdbi->query(fmt::format("scan 0 MATCH {}", name), &pRedisReply, printlog))
			return false;
		
		size_t size = 0;
		
		if(pRedisReply)
		{
			if(pRedisReply->elements == 2 && pRedisReply->element[1]->type == REDIS_REPLY_ARRAY)
			{
				size = pRedisReply->element[1]->elements;
			}
			
			freeReplyObject(pRedisReply); 
		}
		
		return size > 0;
	}
	
	/**
     * @brief ɾ�� Redis �еı�
     * 
     * @param pdbi ָ�� DBInterfaceRedis �����ָ��
     * @param tableName ����
     * @param printlog �Ƿ��ӡ��־
     * @return �ɹ�ɾ������ true��ʧ�ܷ��� false
     */
	static bool dropTable(DBInterfaceRedis* pdbi, const std::string& tableName, bool printlog = true)
	{
		uint64 index = 0;
		
		while(true)
		{
			redisReply* pRedisReply = NULL;
			
			pdbi->query(fmt::format("scan {} MATCH {}", index, tableName), &pRedisReply, printlog);
			
			if(pRedisReply)
			{
				if(pRedisReply->elements == 2)
				{
					KBE_ASSERT(pRedisReply->element[0]->type == REDIS_REPLY_STRING);
					
					// ��һ�������index��ʼ
					StringConv::str2value(index, pRedisReply->element[0]->str);
					
					redisReply* r0 = pRedisReply->element[1];
					KBE_ASSERT(r0->type == REDIS_REPLY_ARRAY);
					
					for(size_t j = 0; j < r0->elements; ++j) 
					{
						redisReply* r1 = r0->element[j];
						KBE_ASSERT(r1->type == REDIS_REPLY_STRING);
							
						pdbi->query(fmt::format("del {}", r1->str), &pRedisReply, printlog);
					}
				}
				
				freeReplyObject(pRedisReply); 
			}
			else
			{
				return false;
			}
			
			if(index == 0)
				break;
		}
		
		return true;
	}
	
	/**
     * @brief ɾ�� Redis ���е�ָ����
     * 
     * @param pdbi ָ�� DBInterfaceRedis �����ָ��
     * @param tableName ����
     * @param itemName Ҫɾ��������
     * @param printlog �Ƿ��ӡ��־
     * @return �ɹ�ɾ����� true��ʧ�ܷ��� false
     */
	static bool dropTableItem(DBInterfaceRedis* pdbi, const std::string& tableName, 
		const std::string& itemName, bool printlog = true)
	{
		uint64 index = 0;
		
		while(true)
		{
			redisReply* pRedisReply = NULL;
			
			pdbi->query(fmt::format("scan {} MATCH {}", index, tableName), &pRedisReply, printlog);
			
			if(pRedisReply)
			{
				if(pRedisReply->elements == 2)
				{
					KBE_ASSERT(pRedisReply->element[0]->type == REDIS_REPLY_STRING);
					
					// ��һ�������index��ʼ
					StringConv::str2value(index, pRedisReply->element[0]->str);
					
					redisReply* r0 = pRedisReply->element[1];
					KBE_ASSERT(r0->type == REDIS_REPLY_ARRAY);
					
					for(size_t j = 0; j < r0->elements; ++j) 
					{
						redisReply* r1 = r0->element[j];
						KBE_ASSERT(r1->type == REDIS_REPLY_STRING);

						pdbi->query(fmt::format("hdel {} {}", r1->str, itemName), &pRedisReply, printlog);
					}
				}
				
				freeReplyObject(pRedisReply); 
			}
			else
			{
				return false;
			}
			
			if(index == 0)
				break;
		}
		
		return true;
	}
};

}
#endif // KBE_REDIS_HELPER_H
