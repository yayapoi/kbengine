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
 * @brief RedisHelper 类提供了一系列静态方法，用于与 Redis 数据库进行交互。
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
     * @brief 设置 Redis 键的过期时间
     * 
     * @param pdbi 指向 DBInterfaceRedis 对象的指针
     * @param key 要设置过期时间的键
     * @param secs 过期时间（秒）
     * @param printlog 是否打印日志
     * @return 成功返回 true，失败返回 false
     */
	static bool expireKey(DBInterfaceRedis* pdbi, const std::string& key, int secs, bool printlog = true)
	{
		if (!pdbi->query(printlog, "EXPIRE %s %d", key.c_str(), secs))
			return false;
	}
	
	/**
     * @brief 检查 Redis 回复是否为数组类型，并且所有元素都是整数或字符串
     * 
     * @param pRedisReply 指向 redisReply 对象的指针
     * @return 如果所有元素都是整数或字符串，返回 true；否则返回 false
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
     * @brief 检查 Redis 中是否存在指定的表
     * 
     * @param pdbi 指向 DBInterfaceRedis 对象的指针
     * @param name 表名
     * @param printlog 是否打印日志
     * @return 如果存在指定的表，返回 true；否则返回 false
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
     * @brief 删除 Redis 中的表
     * 
     * @param pdbi 指向 DBInterfaceRedis 对象的指针
     * @param tableName 表名
     * @param printlog 是否打印日志
     * @return 成功删除表返回 true，失败返回 false
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
					
					// 下一次由这个index开始
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
     * @brief 删除 Redis 表中的指定项
     * 
     * @param pdbi 指向 DBInterfaceRedis 对象的指针
     * @param tableName 表名
     * @param itemName 要删除的项名
     * @param printlog 是否打印日志
     * @return 成功删除项返回 true，失败返回 false
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
					
					// 下一次由这个index开始
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
