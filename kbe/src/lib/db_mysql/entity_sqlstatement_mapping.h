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
 * @brief ʵ��SQL���ӳ���࣬���ڹ���ͬ��Ĳ�ѯ������͸���SQL��䡣
 * 
 * ����̳���Singletonģ���࣬ȷ��������Ӧ�ó�����ֻ��һ��ʵ����
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
     * @brief ��Ӳ�ѯSQL���
     * 
     * @param tableName ����
     * @param pSqlStatement SQL������ָ��
     */
    void addQuerySqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);
    /**
     * @brief ��Ӳ���SQL���
     * 
     * @param tableName ����
     * @param pSqlStatement SQL������ָ��
     */
    void addInsertSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);
    /**
     * @brief ��Ӹ���SQL���
     * 
     * @param tableName ����
     * @param pSqlStatement SQL������ָ��
     */
    void addUpdateSqlStatement(const std::string& tableName, SqlStatement* pSqlStatement);

    /**
     * @brief ���Ҳ�ѯSQL���
     * 
     * @param tableName ����
     * @return SqlStatement* �ҵ���SQL������ָ�룬���δ�ҵ��򷵻�nullptr
     */
    SqlStatement* findQuerySqlStatement(const std::string& tableName);
    /**
     * @brief ���Ҳ���SQL���
     * 
     * @param tableName ����
     * @return SqlStatement* �ҵ���SQL������ָ�룬���δ�ҵ��򷵻�nullptr
     */
    SqlStatement* findInsertSqlStatement(const std::string& tableName);
    /**
     * @brief ���Ҹ���SQL���
     * 
     * @param tableName ����
     * @return SqlStatement* �ҵ���SQL������ָ�룬���δ�ҵ��򷵻�nullptr
     */
    SqlStatement* findUpdateSqlStatement(const std::string& tableName);

protected:
    /**
     * @brief �洢��ѯSQL����ӳ���
     */
    KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > query_sqlStatements_;
    /**
     * @brief �洢����SQL����ӳ���
     */
    KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > update_sqlStatements_;
    /**
     * @brief �洢����SQL����ӳ���
     */
    KBEUnordered_map< std::string, KBEShared_ptr< SqlStatement > > insert_sqlStatements_;
};



}
#endif
