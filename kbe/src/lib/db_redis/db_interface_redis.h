// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_DB_INTERFACE_REDIS_H
#define KBE_DB_INTERFACE_REDIS_H

#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"

#include "hiredis.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#ifdef _DEBUG
#pragma comment (lib, "hiredis_d.lib")
#else
#pragma comment (lib, "hiredis.lib")
#endif
#endif

namespace KBEngine { 

class DBException;

/*
	数据库接口
	tbl_Account_Auto_increment = uint64(1)
	tbl_Account:1 = hashes(name, password, xxx)
	tbl_Account:2 = hashes(name, password, xxx)
	tbl_Account:3 = hashes(name, password, xxx(array))

	// array of type
	tbl_Account_xxx_values:3:size = uint64(3)
	tbl_Account_xxx_values:3:1 = val
	tbl_Account_xxx_values:3:2 = val
	tbl_Account_xxx_values:3:3 = val	

	数据库接口类，用于与Redis数据库进行交互
 * 
 * 该类继承自DBInterface，提供了与Redis数据库交互的具体实现。
 * 包括连接管理、查询、事务处理等功能。
*/
class DBInterfaceRedis : public DBInterface
{
public:
	DBInterfaceRedis(const char* name);
	virtual ~DBInterfaceRedis();

	/**
     * @brief 初始化数据库接口
     * 
     * @param pdbi 数据库接口指针
     * @return 成功返回true，失败返回false
     */
	static bool initInterface(DBInterface* pdbi);
	
	/**
     * @brief 检查与Redis服务器的连接
     * 
     * @param pRedisContext Redis连接上下文，默认为NULL
     * @return 成功返回true，失败返回false
     */
    bool ping(redisContext* pRedisContext = NULL);
    
    /**
     * @brief 设置是否在事务中
     * 
     * @param value 是否在事务中
     */
    void inTransaction(bool value)
    {
        KBE_ASSERT(inTransaction_ != value);
        inTransaction_ = value;
    }

	/**
     * @brief 获取Redis连接上下文
     * 
     * @return Redis连接上下文指针
     */
    redisContext* context() { return pRedisContext_; }
    
    /**
     * @brief 检查是否失去连接
     * 
     * @return 如果失去连接返回true，否则返回false
     */
    bool hasLostConnection() const { return hasLostConnection_; }
    /**
     * @brief 设置是否失去连接
     * 
     * @param v 是否失去连接
     */
    void hasLostConnection(bool v) { hasLostConnection_ = v; }
	
	/**
		检查环境
	*/
	virtual bool checkEnvironment();
	
	/**
		检查错误， 对错误的内容进行纠正
		如果纠正不成功返回失败
	*/
	virtual bool checkErrors();

	/**
		与某个数据库关联
	*/
	bool reattach();
	virtual bool attach(const char* databaseName = NULL);
	virtual bool detach();

	/**
		获取数据库所有的表名
	*/
	virtual bool getTableNames( std::vector<std::string>& tableNames, const char * pattern);

	/**
		获取数据库某个表所有的字段名称
	*/
	virtual bool getTableItemNames(const char* tableName, std::vector<std::string>& itemNames);

	/**
		查询表
	*/
	virtual bool query(const char* cmd, uint32 size, bool printlog = true, MemoryStream * result = NULL);
	bool query(const std::string& cmd, redisReply** pRedisReply, bool printlog = true);
	bool query(bool printlog, const char* format, ...);
	bool queryAppend(bool printlog, const char* format, ...);
	// 获取查询回复
	bool getQueryReply(redisReply **pRedisReply);
	
	/**
     * @brief 将查询结果写入MemoryStream
     * 
     * @param pRedisReply 查询回复
     * @param result 存储结果的MemoryStream指针
     */
    void write_query_result(redisReply* pRedisReply, MemoryStream * result);
    /**
     * @brief 将查询结果的元素写入MemoryStream
     * 
     * @param pRedisReply 查询回复
     * @param result 存储结果的MemoryStream指针
     */
    void write_query_result_element(redisReply* pRedisReply, MemoryStream * result);
		
	/**
		返回这个接口的描述
	*/
	virtual const char* c_str();

	/** 
		获取错误
	*/
	virtual const char* getstrerror();

	/** 
		获取错误编号
	*/
	virtual int getlasterror();

	/**
		创建一个entity存储表
	*/
	virtual EntityTable* createEntityTable(EntityTables* pEntityTables);

	/** 
		从数据库删除entity表
	*/
	virtual bool dropEntityTableFromDB(const char* tableName);
	
	/** 
		从数据库删除entity表字段
	*/
	virtual bool dropEntityTableItemFromDB(const char* tableName, const char* tableItemName);

	/**
		锁住接口操作
	*/
	virtual bool lock();
	virtual bool unlock();

	void throwError(DBException* pDBException);
	
	/**
		处理异常
	*/
	virtual bool processException(std::exception & e);
	
protected:
	redisContext* pRedisContext_;  // Redis连接上下文
    bool hasLostConnection_;       // 是否失去连接
    bool inTransaction_;           // 是否在事务中
};


}

#endif // KBE_DB_INTERFACE_REDIS_H
