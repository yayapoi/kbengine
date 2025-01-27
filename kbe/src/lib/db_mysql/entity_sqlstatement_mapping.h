// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ENTITY_SQL_STATEMENT_MAPPING_H
#define KBE_ENTITY_SQL_STATEMENT_MAPPING_H

// common include	
// #define NDEBUG
#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"

namespace KBEngine{ 

class SqlStatement;

/**
 * @brief 实体SQL语句映射类，用于管理不同表的查询、插入和更新SQL语句。
 * 
 * 该类继承自Singleton模板类，确保在整个应用程序中只有一个实例。
 */
class EntitySqlStatementMapping : public Singleton<EntitySqlStatementMapping>
{
public:
	EntitySqlStatementMapping()
	{
	}

	virtual ~EntitySqlStatementMapping()
	{
	}

	/**
     * @brief 添加查询SQL语句
     * 
     * @param tableName 表名
     * @param pSqlStatement SQL语句对象指针
     */
    void addQuerySqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);
    /**
     * @brief 添加插入SQL语句
     * 
     * @param tableName 表名
     * @param pSqlStatement SQL语句对象指针
     */
    void addInsertSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);
    /**
     * @brief 添加更新SQL语句
     * 
     * @param tableName 表名
     * @param pSqlStatement SQL语句对象指针
     */
    void addUpdateSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);

    /**
     * @brief 查找查询SQL语句
     * 
     * @param tableName 表名
     * @return SqlStatement* 找到的SQL语句对象指针，如果未找到则返回nullptr
     */
    SqlStatement* findQuerySqlStatement(const std::string& tableName);
    /**
     * @brief 查找插入SQL语句
     * 
     * @param tableName 表名
     * @return SqlStatement* 找到的SQL语句对象指针，如果未找到则返回nullptr
     */
    SqlStatement* findInsertSqlStatement(const std::string& tableName);
    /**
     * @brief 查找更新SQL语句
     * 
     * @param tableName 表名
     * @return SqlStatement* 找到的SQL语句对象指针，如果未找到则返回nullptr
     */
    SqlStatement* findUpdateSqlStatement(const std::string& tableName);

protected:
    /**
     * @brief 存储查询SQL语句的映射表
     */
    KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > query_sqlStatements_;
    /**
     * @brief 存储更新SQL语句的映射表
     */
    KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > update_sqlStatements_;
    /**
     * @brief 存储插入SQL语句的映射表
     */
    KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > insert_sqlStatements_;
};



}
#endif
