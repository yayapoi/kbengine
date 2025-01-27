// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_WRITE_ENTITY_HELPER_H
#define KBE_WRITE_ENTITY_HELPER_H

// common include	
// #define NDEBUG
#include <sstream>
#include "common.h"
#include "sqlstatement.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "db_interface_mysql.h"

namespace KBEngine{ 

class WriteEntityHelper
{
public:
	WriteEntityHelper()
	{
	}

	virtual ~WriteEntityHelper()
	{
	}

	// 创建SQL语句
	static SqlStatement* createSql(DBInterface* pdbi, DB_TABLE_OP opType, 
		std::string tableName, DBID parentDBID, 
		DBID dbid, mysql::DBContext::DB_ITEM_DATAS& tableVal)
	{
		SqlStatement* pSqlcmd = NULL;

		switch(opType) //  根据操作类型创建不同的SqlStatement对象
		{
		case TABLE_OP_UPDATE:
			if(dbid > 0)
				pSqlcmd = new SqlStatementUpdate(pdbi, tableName, parentDBID, dbid, tableVal);
			else
				pSqlcmd = new SqlStatementInsert(pdbi, tableName, parentDBID, dbid, tableVal);
			break;
		case TABLE_OP_INSERT:
			pSqlcmd = new SqlStatementInsert(pdbi, tableName, parentDBID, dbid, tableVal);
			break;
		case TABLE_OP_DELETE:
			break;
		default:
			KBE_ASSERT(false && "no support!\n"); //  如果操作类型不支持，则断言
		};

		return pSqlcmd;
	}

	/**
     * 将数据更新到表中
     *
     * @param optype 操作类型
     * @param pdbi 数据库接口
     * @param context 数据上下文
     * @return 操作是否成功
     */
	static bool writeDB(DB_TABLE_OP optype, DBInterface* pdbi, mysql::DBContext& context)
	{
		bool ret = true;

		if(!context.isEmpty) //  如果数据上下文不为空
		{
			SqlStatement* pSqlcmd = createSql(pdbi, optype, context.tableName,
				context.parentTableDBID, 
				context.dbid, context.items);	//  创建SQL语句

			ret = pSqlcmd->query(); //  执行SQL语句
			context.dbid = pSqlcmd->dbid(); //  获取数据库ID
			delete pSqlcmd; //  删除SQL语句
		}

		if(optype == TABLE_OP_INSERT) //  如果操作类型为插入
		{
			// 开始更新所有的子表
			mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
			for(; iter1 != context.optable.end(); ++iter1)
			{
				mysql::DBContext& wbox = *iter1->second.get(); //  获取子表数据上下文
				
				// 绑定表关系
				wbox.parentTableDBID = context.dbid;

				// 更新子表
				writeDB(optype, pdbi, wbox);
			}
		}
		else
		{
			// 如果有父ID首先得到该属性数据库中同父id的数据有多少条目， 并取出每条数据的id
			// 然后将内存中的数据顺序更新至数据库， 如果数据库中有存在的条目则顺序覆盖更新已有的条目， 如果数据数量
			// 大于数据库中已有的条目则插入剩余的数据， 如果数据少于数据库中的条目则删除数据库中的条目
			// select id from tbl_SpawnPoint_xxx_values where parentID = 7;
			KBEUnordered_map< std::string, std::vector<DBID> > childTableDBIDs;

			if(context.dbid > 0)
			{
				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin(); //  遍历context.optable中的每一个元素
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get(); //  获取当前元素的DBContext

					KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter =
						childTableDBIDs.find(context.tableName);  //  在childTableDBIDs中查找context.tableName

					if(iter == childTableDBIDs.end()) //  如果childTableDBIDs中没有找到context.tableName
					{
						std::vector<DBID> v;
						childTableDBIDs.insert(std::pair< std::string, std::vector<DBID> >(wbox.tableName, v));
					}
				}
				
				if(childTableDBIDs.size() > 1) //  如果子表的数量大于1，则进行查询
				{
					std::string sqlstr_getids; //  定义查询字符串
					KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  遍历子表
					for(; tabiter != childTableDBIDs.end();)
					{
						char sqlstr[MAX_BUF * 10]; //  定义查询语句
						kbe_snprintf(sqlstr, MAX_BUF * 10, "select count(id) from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID " union all ",  //  查询子表中的记录数
							tabiter->first.c_str(),
							context.dbid);
						
						sqlstr_getids += sqlstr;

						kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  查询子表中的记录ID
							tabiter->first.c_str(),
							context.dbid);

						sqlstr_getids += sqlstr;
						if(++tabiter != childTableDBIDs.end()) //  如果不是最后一个子表，则添加union all
							sqlstr_getids += " union all ";
					}
					
					if(pdbi->query(sqlstr_getids.c_str(), sqlstr_getids.size(), false)) //  执行查询
					{
						MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql()); //  获取查询结果
						if(pResult)
						{
							MYSQL_ROW arow; //  获取查询结果
							int32 count = 0; //  计数器
							tabiter = childTableDBIDs.begin(); //  遍历子表DBID
							bool first = true; //  是否第一次

							while((arow = mysql_fetch_row(pResult)) != NULL) //  遍历查询结果
							{
								if(count == 0) //  如果是第一行
								{
									StringConv::str2value(count, arow[0]); //  将第一行的值转换为计数器
									if(!first || count <= 0) //  如果不是第一次或者计数器小于等于0，则tabiter++
										tabiter++;
									continue; //  继续下一次循环
								}

								DBID old_dbid; //  定义旧DBID
								StringConv::str2value(old_dbid, arow[0]); //  将查询结果的值转换为旧DBID
								tabiter->second.push_back(old_dbid); //  将旧DBID添加到tabiter指向的second中
								count--; //  计数器减1
								first = false; //  不是第一次
							}

							mysql_free_result(pResult); //  释放查询结果
						}
					}
				}
				else if(childTableDBIDs.size() == 1) //  如果子表DBID的大小为1
				{
					KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  遍历子表
						char sqlstr[MAX_BUF * 10];
						kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  拼接SQL语句，查询子表中的数据
							tabiter->first.c_str(),
							context.dbid);

						if(pdbi->query(sqlstr, strlen(sqlstr), false))
						{
							MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
							if(pResult)
							{
								MYSQL_ROW arow;
								while((arow = mysql_fetch_row(pResult)) != NULL) //  遍历查询结果
								{
									DBID old_dbid;
									StringConv::str2value(old_dbid, arow[0]); //  将查询结果转换为DBID类型
									tabiter->second.push_back(old_dbid); //  将查询结果添加到子表中的DBID列表中
								}

								mysql_free_result(pResult);
							}
						}
				}
			}

			// 如果是要清空此表， 则循环N次已经找到的dbid， 使其子表中的子表也能有效删除
			if(!context.isEmpty)
			{
				// 开始更新所有的子表
				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get();
					
					if(wbox.isEmpty) //  如果子表为空，则跳过
						continue;

					// 绑定表关系
					wbox.parentTableDBID = context.dbid;

					KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter =
						childTableDBIDs.find(wbox.tableName);  //  查找子表在父表中的DBID
					
					if(iter != childTableDBIDs.end()) //  如果找到了子表
					{
						if(iter->second.size() > 0) //  如果子表不为空
						{
							wbox.dbid = iter->second.front(); //  将子表的DBID赋值给子表
							iter->second.erase(iter->second.begin()); //  从子表DBID列表中删除第一个DBID
						}

						if(iter->second.size() <= 0) //  如果子表DBID列表为空
						{
							childTableDBIDs.erase(wbox.tableName); //  从子表DBID列表中删除该子表
						}
					}

					// 更新子表
					writeDB(optype, pdbi, wbox);
				}
			}
			
			// 删除废弃的数据项
			KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
			for(; tabiter != childTableDBIDs.end(); ++tabiter)
			{
				if(tabiter->second.size() == 0)
					continue;

				// 先删除数据库中的记录
				std::string sqlstr = "delete from " ENTITY_TABLE_PERFIX "_";
				sqlstr += tabiter->first;
				sqlstr += " where " TABLE_ID_CONST_STR " in (";

				std::vector<DBID>::iterator iter = tabiter->second.begin();
				for(; iter != tabiter->second.end(); ++iter)
				{
					DBID dbid = (*iter);

					char sqlstr1[MAX_BUF];
					kbe_snprintf(sqlstr1, MAX_BUF, "%" PRDBID, dbid); //  将DBID转换为字符串
					sqlstr += sqlstr1;
					sqlstr += ",";
				}
				
				sqlstr.erase(sqlstr.size() - 1); //  去掉最后一个逗号
				sqlstr += ")";
				bool ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), false); //  执行SQL语句
				KBE_ASSERT(ret); //  断言ret的值

				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin(); //  遍历context.optable
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get(); //  获取wbox
					if(wbox.tableName == tabiter->first) //  如果wbox的tableName等于tabiter的first
					{
						std::vector<DBID>::iterator iter = tabiter->second.begin(); //  遍历tabiter的second
						for(; iter != tabiter->second.end(); ++iter)
						{
							DBID dbid = (*iter); //  获取dbid
							
							wbox.parentTableDBID = context.dbid; //  设置wbox的parentTableDBID和dbid
							wbox.dbid = dbid;
							wbox.isEmpty = true; //  设置wbox的isEmpty为true

							// 更新子表
							writeDB(optype, pdbi, wbox);
						}
					}
				}
			}
		}
		return ret;
	}

protected:

};

}
#endif // KBE_WRITE_ENTITY_HELPER_H

