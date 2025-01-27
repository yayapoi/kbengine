// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_REMOVE_ENTITY_HELPER_H
#define KBE_REMOVE_ENTITY_HELPER_H

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

class RemoveEntityHelper
{
public:
	RemoveEntityHelper()
	{
	}

	virtual ~RemoveEntityHelper()
	{
	}

	// 静态方法，用于从数据库中删除实体
    static bool removeDB(DBInterface* dbi, mysql::DBContext& context)
    {
        // 调用私有方法 _removeDB 进行初步删除操作
        bool ret = _removeDB(dbi, context);

        if (!ret)
            return false;

        // 构建 SQL 语句，删除指定表中的记录
        std::string sqlstr = "delete from " ENTITY_TABLE_PERFIX "_";
        sqlstr += context.tableName;
        sqlstr += " where " TABLE_ID_CONST_STR "=";

        char sqlstr1[MAX_BUF];
        kbe_snprintf(sqlstr1, MAX_BUF, "%" PRDBID, context.dbid);
        sqlstr += sqlstr1;
        
        // 执行 SQL 语句
        ret = dbi->query(sqlstr.c_str(), sqlstr.size(), false);
        KBE_ASSERT(ret);

        return ret;
    }

	// 私有静态方法，用于递归删除数据库中的相关记录
    static bool _removeDB(DBInterface* dbi, mysql::DBContext& context)
    {
        bool ret = true;

        // 存储子表及其对应的 DBID 列表
        KBEUnordered_map< std::string, std::vector<DBID> > childTableDBIDs;

        // 遍历所有可操作的表
        mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
        for (; iter1 != context.optable.end(); ++iter1) //  遍历 optable 中的每一个条目
        {
            mysql::DBContext& wbox = *iter1->second.get(); //  获取 DBContext 对象

            // 如果子表不存在于 childTableDBIDs 中，则插入新的条目
            KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter = 
                childTableDBIDs.find(context.tableName);

            if (iter == childTableDBIDs.end())
            {
                std::vector<DBID> v; //  创建一个新的 vector 并插入到 childTableDBIDs 中
                childTableDBIDs.insert(std::pair< std::string, std::vector<DBID> >(wbox.tableName, v));
            }
        }
        
        // 如果子表数量大于 1
        if (childTableDBIDs.size() > 1)
        {
            std::string sqlstr_getids; //  定义一个字符串用于存储 SQL 语句
            KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  遍历 childTableDBIDs 中的每一个条目
            for (; tabiter != childTableDBIDs.end();)
            {
                char sqlstr[MAX_BUF * 10]; //  定义一个字符数组用于存储 SQL 语句
                kbe_snprintf(sqlstr, MAX_BUF * 10, "select count(id) from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID " union all ",  //  使用 kbe_snprintf 函数生成 SQL 语句
                    tabiter->first.c_str(),
                    context.dbid);
                
                sqlstr_getids += sqlstr; //  将 SQL 语句拼接起来

                kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  拼接 SQL 查询语句
                    tabiter->first.c_str(),
                    context.dbid);

                sqlstr_getids += sqlstr; //  将 SQL 语句拼接起来
                if (++tabiter != childTableDBIDs.end()) //  如果还有子表，则拼接 " union all "
                    sqlstr_getids += " union all ";
            }
            
            // 执行 SQL 查询
            if (dbi->query(sqlstr_getids.c_str(), sqlstr_getids.size(), false))
            {
                MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(dbi)->mysql()); //  获取查询结果
                if (pResult)
                {
                    MYSQL_ROW arow; //  获取查询结果行
                    int32 count = 0; //  计数器
                    tabiter = childTableDBIDs.begin(); //  子表迭代器
                    bool first = true; //  是否是第一行

                    while ((arow = mysql_fetch_row(pResult)) != NULL) //  遍历查询结果
                    {
                        if (count == 0) //  如果计数器为 0
                        {
                            StringConv::str2value(count, arow[0]); //  将字符串转换为数值
                            if (!first || count <= 0) //  如果不是第一次循环或者count小于等于0，则tabiter++
                                tabiter++;
                            continue; //  继续下一次循环
                        }

                        DBID old_dbid; //  定义一个DBID类型的变量
                        StringConv::str2value(old_dbid, arow[0]); //  将字符串转换为DBID类型
                        tabiter->second.push_back(old_dbid); //  将转换后的DBID添加到tabiter的second中
                        count--; //  count减1
                        first = false; //  将first设置为false
                    }

                    mysql_free_result(pResult); //  释放结果集
                }
            }
        }
        else if (childTableDBIDs.size() == 1) //  如果childTableDBIDs的大小为1
        {
            KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  定义一个迭代器，指向childTableDBIDs的begin
            char sqlstr[MAX_BUF * 10]; //  定义一个字符数组，用于存储sql语句
            kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  将sql语句格式化到sqlstr中
                tabiter->first.c_str(),
                context.dbid);

            // 执行 SQL 查询
            if (dbi->query(sqlstr, strlen(sqlstr), false))
            {
                MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(dbi)->mysql());
                if (pResult)
                {
                    MYSQL_ROW arow;
                    while ((arow = mysql_fetch_row(pResult)) != NULL)
                    {
                        DBID old_dbid;
                        StringConv::str2value(old_dbid, arow[0]);
                        tabiter->second.push_back(old_dbid);
                    }

                    mysql_free_result(pResult);
                }
            }
        }
    
        // 删除废弃的数据项
        KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
        for (; tabiter != childTableDBIDs.end(); ++tabiter)
        {
            if (tabiter->second.size() == 0) //  如果该表的数据项为空，则删除该表
                continue;

            // 构建 SQL 语句，删除指定表中的记录
            std::string sqlstr = "delete from " ENTITY_TABLE_PERFIX "_";
            sqlstr += tabiter->first;
            sqlstr += " where " TABLE_ID_CONST_STR " in (";

            std::vector<DBID>::iterator iter = tabiter->second.begin();
            for (; iter != tabiter->second.end(); ++iter)
            {
                DBID dbid = (*iter);

                char sqlstr1[MAX_BUF];
                kbe_snprintf(sqlstr1, MAX_BUF, "%" PRDBID, dbid);
                sqlstr += sqlstr1;
                sqlstr += ",";
            }
            
            sqlstr.erase(sqlstr.size() - 1); //  去掉最后一个逗号
            sqlstr += ")";
            bool ret = dbi->query(sqlstr.c_str(), sqlstr.size(), false); //  执行 SQL 语句
            KBE_ASSERT(ret);

            // 遍历所有可操作的表，递归删除相关记录
            mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
            for (; iter1 != context.optable.end(); ++iter1)
            {
                mysql::DBContext& wbox = *iter1->second.get();
                if (wbox.tableName == tabiter->first)
                {
                    std::vector<DBID>::iterator iter = tabiter->second.begin(); //  遍历tabiter->second中的所有DBID
                    for (; iter != tabiter->second.end(); ++iter)
                    {
                        DBID dbid = (*iter); //  获取当前DBID
                        
                        wbox.parentTableDBID = context.dbid; //  设置wbox的父表DBID和当前DBID
                        wbox.dbid = dbid;
                        wbox.isEmpty = true;

                        _removeDB(dbi, wbox); //  调用_removeDB函数删除DBID
                    }
                }
            }
        }

        return ret;
    }

protected:

};

}
#endif // KBE_REMOVE_ENTITY_HELPER_H
