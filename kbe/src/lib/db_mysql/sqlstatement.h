// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_SQL_STATEMENT_H
#define KBE_SQL_STATEMENT_H

// common include	
// #define NDEBUG
#include <sstream>
#include "common.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "db_interface_mysql.h"

namespace KBEngine{ 

class SqlStatement
{
public:
	/**
     * 构造函数，初始化SQL语句的基本信息。
     *
     * @param pdbi 数据库接口指针
     * @param tableName 表名
     * @param parentDBID 父表的dbid
     * @param dbid 当前表的dbid
     * @param tableItemDatas 表的字段信息
     */
	// 构造函数，初始化SqlStatement对象
	SqlStatement(DBInterface* pdbi, std::string tableName, DBID parentDBID, DBID dbid, 
		mysql::DBContext::DB_ITEM_DATAS& tableItemDatas) :
	  tableItemDatas_(tableItemDatas),
	  sqlstr_(),
	  tableName_(tableName),
	  dbid_(dbid),
	  parentDBID_(parentDBID),
	  pdbi_(pdbi)
	{
	}

	virtual ~SqlStatement()
	{
	}

	std::string& sql(){ return sqlstr_; }

	virtual bool query(DBInterface* pdbi = NULL)
	{
		// 没有数据更新
		if(sqlstr_ == "")
			return true;

		bool ret = static_cast<DBInterfaceMysql*>(pdbi != NULL ? pdbi : pdbi_)->query(sqlstr_.c_str(), sqlstr_.size(), false);

		if(!ret)
		{
			ERROR_MSG(fmt::format("SqlStatement::query: {}\n\tsql:{}\n", 
				(pdbi != NULL ? pdbi : pdbi_)->getstrerror(), sqlstr_));

			return false;
		}

		return ret;
	}

	/**
     * 获取当前表的dbid
     *
     * @return 当前表的dbid
     */
	DBID dbid() const{ return dbid_; }

protected:
	mysql::DBContext::DB_ITEM_DATAS& tableItemDatas_;  // 表的字段信息
    std::string sqlstr_;                                // SQL语句字符串
    std::string tableName_;                             // 表名
    DBID dbid_;                                         // 当前表的dbid
    DBID parentDBID_;                                   // 父表的dbid
    DBInterface* pdbi_;                                 // 数据库接口指针 
};

class SqlStatementInsert : public SqlStatement
{
public:
	/**
     * 构造函数，生成INSERT语句。
     *
     * @param pdbi 数据库接口指针
     * @param tableName 表名
     * @param parentDBID 父表的dbid
     * @param dbid 当前表的dbid
     * @param tableItemDatas 表的字段信息
     */
	SqlStatementInsert(DBInterface* pdbi, std::string tableName, DBID parentDBID, 
		DBID dbid, mysql::DBContext::DB_ITEM_DATAS& tableItemDatas) :
	  SqlStatement(pdbi, tableName, parentDBID, dbid, tableItemDatas)
	{
		// insert into tbl_Account (sm_accountName) values("fdsafsad\0\fdsfasfsa\0fdsafsda");
		sqlstr_ = "insert into " ENTITY_TABLE_PERFIX "_";
		sqlstr_ += tableName;
		sqlstr_ += " (";
		sqlstr1_ = ")  values(";
		
		if(parentDBID > 0)
		{
			sqlstr_ += TABLE_PARENTID_CONST_STR;
			sqlstr_ += ",";
			
			char strdbid[MAX_BUF];
			kbe_snprintf(strdbid, MAX_BUF, "%" PRDBID, parentDBID);
			sqlstr1_ += strdbid;
			sqlstr1_ += ",";
		}

		mysql::DBContext::DB_ITEM_DATAS::iterator tableValIter = tableItemDatas.begin(); //  遍历tableItemDatas
		for(; tableValIter != tableItemDatas.end(); ++tableValIter)
		{
			KBEShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = (*tableValIter);

			if(dbid > 0)
			{
			}
			else //  否则，将sqlkey和sqlval添加到sqlstr_和sqlstr1_中
			{
				sqlstr_ += pSotvs->sqlkey;
				if(pSotvs->extraDatas.size() > 0)
					sqlstr1_ += pSotvs->extraDatas;
				else
					sqlstr1_ += pSotvs->sqlval;

				sqlstr_ += ",";
				sqlstr1_ += ",";
			}
		}
		
		if(parentDBID > 0 || sqlstr_.at(sqlstr_.size() - 1) == ',') //  如果parentDBID大于0或者sqlstr_的最后一个字符是逗号，则删除最后一个字符
			sqlstr_.erase(sqlstr_.size() - 1);

		if(parentDBID > 0 || sqlstr1_.at(sqlstr1_.size() - 1) == ',')
			sqlstr1_.erase(sqlstr1_.size() - 1);

		sqlstr1_ += ")";
		sqlstr_ += sqlstr1_;
	}

	virtual ~SqlStatementInsert()
	{
	}

	/**
     * 执行INSERT语句
     *
     * @param pdbi 数据库接口指针（可选，默认使用构造函数中的pdbi）
     * @return 执行结果
     */
	virtual bool query(DBInterface* pdbi = NULL)
	{
		// 没有数据更新
		if(sqlstr_ == "")
			return true;

		bool ret = SqlStatement::query(pdbi);
		if(!ret)
		{
			ERROR_MSG(fmt::format("SqlStatementInsert::query: {}\n\tsql:{}\n",
				(pdbi != NULL ? pdbi : pdbi_)->getstrerror(), sqlstr_));

			return false;
		}

		dbid_ = static_cast<DBInterfaceMysql*>(pdbi != NULL ? pdbi : pdbi_)->insertID();
		return ret;
	}

protected:
	std::string sqlstr1_;	// 用于构建VALUES部分的字符串
};

class SqlStatementUpdate : public SqlStatement
{
public:
	/**
     * 构造函数，生成UPDATE语句。
     *
     * @param pdbi 数据库接口指针
     * @param tableName 表名
     * @param parentDBID 父表的dbid
     * @param dbid 当前表的dbid
     * @param tableItemDatas 表的字段信息
     */
	SqlStatementUpdate(DBInterface* pdbi, std::string tableName, DBID parentDBID, 
		DBID dbid, mysql::DBContext::DB_ITEM_DATAS& tableItemDatas) :
	  SqlStatement(pdbi, tableName, parentDBID, dbid, tableItemDatas)
	{
		if(tableItemDatas.size() == 0)
		{
			sqlstr_ = "";
			return;
		}

		// update tbl_Account set sm_accountName="fdsafsad" where id=123;
		sqlstr_ = "update " ENTITY_TABLE_PERFIX "_";
		sqlstr_ += tableName;
		sqlstr_ += " set ";

		mysql::DBContext::DB_ITEM_DATAS::iterator tableValIter = tableItemDatas.begin();
		for(; tableValIter != tableItemDatas.end(); ++tableValIter)
		{
			KBEShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = (*tableValIter);
			
			sqlstr_ += pSotvs->sqlkey;
			sqlstr_ += "=";
				
			if(pSotvs->extraDatas.size() > 0)
				sqlstr_ += pSotvs->extraDatas;
			else
				sqlstr_ += pSotvs->sqlval;

			sqlstr_ += ",";
		}

		if(sqlstr_.at(sqlstr_.size() - 1) == ',') //  如果sqlstr_的最后一个字符是逗号，则删除该字符
			sqlstr_.erase(sqlstr_.size() - 1);

		sqlstr_ += " where id="; //  拼接WHERE语句
		
		char strdbid[MAX_BUF]; //  将dbid转换为字符串
		kbe_snprintf(strdbid, MAX_BUF, "%" PRDBID, dbid);
		sqlstr_ += strdbid;
	}

	virtual ~SqlStatementUpdate()
	{
	}

protected:
};

class SqlStatementQuery : public SqlStatement
{
public:
	/**
     * 构造函数，生成SELECT语句。
     *
     * @param pdbi 数据库接口指针
     * @param tableName 表名
     * @param parentTableDBIDs 父表的dbid列表
     * @param dbid 当前表的dbid
     * @param tableItemDatas 表的字段信息
     */
	SqlStatementQuery(DBInterface* pdbi, std::string tableName, const std::vector<DBID>& parentTableDBIDs, 
		DBID dbid, mysql::DBContext::DB_ITEM_DATAS& tableItemDatas) :
	  SqlStatement(pdbi, tableName, 0, dbid, tableItemDatas),
	  sqlstr1_()
	{

		// select id,xxx from tbl_SpawnPoint where id=123;
		sqlstr_ = "select id,";
		// 无论哪种情况都查询出ID字段
		sqlstr1_ += " from " ENTITY_TABLE_PERFIX "_";
		sqlstr1_ += tableName;
		
		char strdbid[MAX_BUF];

		if(parentTableDBIDs.size() == 0) //  如果没有父表ID，则直接查询指定ID的数据
		{
			sqlstr1_ += " where id=";
			kbe_snprintf(strdbid, MAX_BUF, "%" PRDBID, dbid);
			sqlstr1_ += strdbid;
		}
		else
		{
			sqlstr_ += TABLE_PARENTID_CONST_STR",";

			if(parentTableDBIDs.size() > 1) //  如果父表ID数量大于1，则查询多个父表ID对应的数据
			{
				sqlstr1_ += " where " TABLE_PARENTID_CONST_STR " in(";
				std::vector<DBID>::const_iterator iter = parentTableDBIDs.begin();
				for(; iter != parentTableDBIDs.end(); ++iter)
				{
					kbe_snprintf(strdbid, MAX_BUF, "%" PRDBID ",", (*iter));
					sqlstr1_ += strdbid;
				}

				sqlstr1_.erase(sqlstr1_.end() - 1);
				sqlstr1_ += ")";
			}
			else //  如果parentTableDBIDs不为空，则将parentTableDBIDs[0]的值赋给strdbid，并将strdbid拼接到sqlstr1_中
			{
				sqlstr1_ += " where " TABLE_PARENTID_CONST_STR "=";
				kbe_snprintf(strdbid, MAX_BUF, "%" PRDBID, parentTableDBIDs[0]);
				sqlstr1_ += strdbid;
			}
		}

		mysql::DBContext::DB_ITEM_DATAS::iterator tableValIter = tableItemDatas.begin();
		for(; tableValIter != tableItemDatas.end(); ++tableValIter)
		{
			KBEShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = (*tableValIter);
			
			sqlstr_ += pSotvs->sqlkey;
			sqlstr_ += ",";
		}

		if(sqlstr_.at(sqlstr_.size() - 1) == ',')
			sqlstr_.erase(sqlstr_.size() - 1);

		sqlstr_ += sqlstr1_;
	}

	virtual ~SqlStatementQuery()
	{
	}

protected:
	std::string sqlstr1_;	// 用于构建FROM和WHERE部分的字符串
};

}
#endif // KBE_SQL_STATEMENT_H
