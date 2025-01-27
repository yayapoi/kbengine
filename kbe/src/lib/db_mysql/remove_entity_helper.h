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

	// ��̬���������ڴ����ݿ���ɾ��ʵ��
    static bool removeDB(DBInterface* dbi, mysql::DBContext& context)
    {
        // ����˽�з��� _removeDB ���г���ɾ������
        bool ret = _removeDB(dbi, context);

        if (!ret)
            return false;

        // ���� SQL ��䣬ɾ��ָ�����еļ�¼
        std::string sqlstr = "delete from " ENTITY_TABLE_PERFIX "_";
        sqlstr += context.tableName;
        sqlstr += " where " TABLE_ID_CONST_STR "=";

        char sqlstr1[MAX_BUF];
        kbe_snprintf(sqlstr1, MAX_BUF, "%" PRDBID, context.dbid);
        sqlstr += sqlstr1;
        
        // ִ�� SQL ���
        ret = dbi->query(sqlstr.c_str(), sqlstr.size(), false);
        KBE_ASSERT(ret);

        return ret;
    }

	// ˽�о�̬���������ڵݹ�ɾ�����ݿ��е���ؼ�¼
    static bool _removeDB(DBInterface* dbi, mysql::DBContext& context)
    {
        bool ret = true;

        // �洢�ӱ����Ӧ�� DBID �б�
        KBEUnordered_map< std::string, std::vector<DBID> > childTableDBIDs;

        // �������пɲ����ı�
        mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
        for (; iter1 != context.optable.end(); ++iter1) //  ���� optable �е�ÿһ����Ŀ
        {
            mysql::DBContext& wbox = *iter1->second.get(); //  ��ȡ DBContext ����

            // ����ӱ������� childTableDBIDs �У�������µ���Ŀ
            KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter = 
                childTableDBIDs.find(context.tableName);

            if (iter == childTableDBIDs.end())
            {
                std::vector<DBID> v; //  ����һ���µ� vector �����뵽 childTableDBIDs ��
                childTableDBIDs.insert(std::pair< std::string, std::vector<DBID> >(wbox.tableName, v));
            }
        }
        
        // ����ӱ��������� 1
        if (childTableDBIDs.size() > 1)
        {
            std::string sqlstr_getids; //  ����һ���ַ������ڴ洢 SQL ���
            KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  ���� childTableDBIDs �е�ÿһ����Ŀ
            for (; tabiter != childTableDBIDs.end();)
            {
                char sqlstr[MAX_BUF * 10]; //  ����һ���ַ��������ڴ洢 SQL ���
                kbe_snprintf(sqlstr, MAX_BUF * 10, "select count(id) from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID " union all ",  //  ʹ�� kbe_snprintf �������� SQL ���
                    tabiter->first.c_str(),
                    context.dbid);
                
                sqlstr_getids += sqlstr; //  �� SQL ���ƴ������

                kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  ƴ�� SQL ��ѯ���
                    tabiter->first.c_str(),
                    context.dbid);

                sqlstr_getids += sqlstr; //  �� SQL ���ƴ������
                if (++tabiter != childTableDBIDs.end()) //  ��������ӱ���ƴ�� " union all "
                    sqlstr_getids += " union all ";
            }
            
            // ִ�� SQL ��ѯ
            if (dbi->query(sqlstr_getids.c_str(), sqlstr_getids.size(), false))
            {
                MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(dbi)->mysql()); //  ��ȡ��ѯ���
                if (pResult)
                {
                    MYSQL_ROW arow; //  ��ȡ��ѯ�����
                    int32 count = 0; //  ������
                    tabiter = childTableDBIDs.begin(); //  �ӱ������
                    bool first = true; //  �Ƿ��ǵ�һ��

                    while ((arow = mysql_fetch_row(pResult)) != NULL) //  ������ѯ���
                    {
                        if (count == 0) //  ���������Ϊ 0
                        {
                            StringConv::str2value(count, arow[0]); //  ���ַ���ת��Ϊ��ֵ
                            if (!first || count <= 0) //  ������ǵ�һ��ѭ������countС�ڵ���0����tabiter++
                                tabiter++;
                            continue; //  ������һ��ѭ��
                        }

                        DBID old_dbid; //  ����һ��DBID���͵ı���
                        StringConv::str2value(old_dbid, arow[0]); //  ���ַ���ת��ΪDBID����
                        tabiter->second.push_back(old_dbid); //  ��ת�����DBID��ӵ�tabiter��second��
                        count--; //  count��1
                        first = false; //  ��first����Ϊfalse
                    }

                    mysql_free_result(pResult); //  �ͷŽ����
                }
            }
        }
        else if (childTableDBIDs.size() == 1) //  ���childTableDBIDs�Ĵ�СΪ1
        {
            KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  ����һ����������ָ��childTableDBIDs��begin
            char sqlstr[MAX_BUF * 10]; //  ����һ���ַ����飬���ڴ洢sql���
            kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  ��sql����ʽ����sqlstr��
                tabiter->first.c_str(),
                context.dbid);

            // ִ�� SQL ��ѯ
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
    
        // ɾ��������������
        KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
        for (; tabiter != childTableDBIDs.end(); ++tabiter)
        {
            if (tabiter->second.size() == 0) //  ����ñ��������Ϊ�գ���ɾ���ñ�
                continue;

            // ���� SQL ��䣬ɾ��ָ�����еļ�¼
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
            
            sqlstr.erase(sqlstr.size() - 1); //  ȥ�����һ������
            sqlstr += ")";
            bool ret = dbi->query(sqlstr.c_str(), sqlstr.size(), false); //  ִ�� SQL ���
            KBE_ASSERT(ret);

            // �������пɲ����ı��ݹ�ɾ����ؼ�¼
            mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
            for (; iter1 != context.optable.end(); ++iter1)
            {
                mysql::DBContext& wbox = *iter1->second.get();
                if (wbox.tableName == tabiter->first)
                {
                    std::vector<DBID>::iterator iter = tabiter->second.begin(); //  ����tabiter->second�е�����DBID
                    for (; iter != tabiter->second.end(); ++iter)
                    {
                        DBID dbid = (*iter); //  ��ȡ��ǰDBID
                        
                        wbox.parentTableDBID = context.dbid; //  ����wbox�ĸ���DBID�͵�ǰDBID
                        wbox.dbid = dbid;
                        wbox.isEmpty = true;

                        _removeDB(dbi, wbox); //  ����_removeDB����ɾ��DBID
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
