// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_REDIS_TRANSACTION_HELPER_H
#define KBE_REDIS_TRANSACTION_HELPER_H

#include "hiredis.h"

namespace KBEngine { 
class DBInterface;
namespace redis {

/**
 * @brief DBTransaction 类用于管理 Redis 事务。
 * 
 * 该类提供了一种方便的方式来开始、提交和回滚 Redis 事务。它还提供了自动启动事务的功能，
 * 并且可以检查事务是否需要重试。
 */
class DBTransaction
{
public:
	DBTransaction(DBInterface* pdbi, bool autostart = true);
	~DBTransaction();
	
	/**
     * @brief 开始事务
     */
    void start();
    /**
     * @brief 结束事务
     */
    void end();

    /**
     * @brief 提交事务
     */
    void commit();
    /**
     * @brief 回滚事务
     */
    void rollback();
	
	/**
     * @brief 检查事务是否需要重试
     * 
     * @return 如果事务需要重试返回 true，否则返回 false
     */
    bool shouldRetry() const;

    /**
     * @brief 设置 DBInterface 指针
     * 
     * @param pdbi 指向 DBInterface 的指针
     */
    void pdbi(DBInterface* pdbi) { pdbi_ = pdbi; }

    /**
     * @brief 获取 Redis 回复指针
     * 
     * @return 指向 redisReply 的指针
     */
    redisReply* pRedisReply() { return pRedisReply_; }

private:
	DBInterface* pdbi_; //  数据库接口指针
	bool committed_; //  是否已提交
	bool autostart_; //  是否自动启动
	redisReply* pRedisReply_;	// 指向 Redis 回复的指针
};

}
}
#endif // KBE_REDIS_TRANSACTION_HELPER_H

