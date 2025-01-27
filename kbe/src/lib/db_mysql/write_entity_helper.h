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

	// ����SQL���
	static SqlStatement* createSql(DBInterface* pdbi, DB_TABLE_OP opType, 
		std::string tableName, DBID parentDBID, 
		DBID dbid, mysql::DBContext::DB_ITEM_DATAS& tableVal)
	{
		SqlStatement* pSqlcmd = NULL;

		switch(opType) //  ���ݲ������ʹ�����ͬ��SqlStatement����
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
			KBE_ASSERT(false && "no support!\n"); //  ����������Ͳ�֧�֣������
		};

		return pSqlcmd;
	}

	/**
     * �����ݸ��µ�����
     *
     * @param optype ��������
     * @param pdbi ���ݿ�ӿ�
     * @param context ����������
     * @return �����Ƿ�ɹ�
     */
	static bool writeDB(DB_TABLE_OP optype, DBInterface* pdbi, mysql::DBContext& context)
	{
		bool ret = true;

		if(!context.isEmpty) //  ������������Ĳ�Ϊ��
		{
			SqlStatement* pSqlcmd = createSql(pdbi, optype, context.tableName,
				context.parentTableDBID, 
				context.dbid, context.items);	//  ����SQL���

			ret = pSqlcmd->query(); //  ִ��SQL���
			context.dbid = pSqlcmd->dbid(); //  ��ȡ���ݿ�ID
			delete pSqlcmd; //  ɾ��SQL���
		}

		if(optype == TABLE_OP_INSERT) //  �����������Ϊ����
		{
			// ��ʼ�������е��ӱ�
			mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
			for(; iter1 != context.optable.end(); ++iter1)
			{
				mysql::DBContext& wbox = *iter1->second.get(); //  ��ȡ�ӱ�����������
				
				// �󶨱��ϵ
				wbox.parentTableDBID = context.dbid;

				// �����ӱ�
				writeDB(optype, pdbi, wbox);
			}
		}
		else
		{
			// ����и�ID���ȵõ����������ݿ���ͬ��id�������ж�����Ŀ�� ��ȡ��ÿ�����ݵ�id
			// Ȼ���ڴ��е�����˳����������ݿ⣬ ������ݿ����д��ڵ���Ŀ��˳�򸲸Ǹ������е���Ŀ�� �����������
			// �������ݿ������е���Ŀ�����ʣ������ݣ� ��������������ݿ��е���Ŀ��ɾ�����ݿ��е���Ŀ
			// select id from tbl_SpawnPoint_xxx_values where parentID = 7;
			KBEUnordered_map< std::string, std::vector<DBID> > childTableDBIDs;

			if(context.dbid > 0)
			{
				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin(); //  ����context.optable�е�ÿһ��Ԫ��
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get(); //  ��ȡ��ǰԪ�ص�DBContext

					KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter =
						childTableDBIDs.find(context.tableName);  //  ��childTableDBIDs�в���context.tableName

					if(iter == childTableDBIDs.end()) //  ���childTableDBIDs��û���ҵ�context.tableName
					{
						std::vector<DBID> v;
						childTableDBIDs.insert(std::pair< std::string, std::vector<DBID> >(wbox.tableName, v));
					}
				}
				
				if(childTableDBIDs.size() > 1) //  ����ӱ����������1������в�ѯ
				{
					std::string sqlstr_getids; //  �����ѯ�ַ���
					KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  �����ӱ�
					for(; tabiter != childTableDBIDs.end();)
					{
						char sqlstr[MAX_BUF * 10]; //  �����ѯ���
						kbe_snprintf(sqlstr, MAX_BUF * 10, "select count(id) from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID " union all ",  //  ��ѯ�ӱ��еļ�¼��
							tabiter->first.c_str(),
							context.dbid);
						
						sqlstr_getids += sqlstr;

						kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  ��ѯ�ӱ��еļ�¼ID
							tabiter->first.c_str(),
							context.dbid);

						sqlstr_getids += sqlstr;
						if(++tabiter != childTableDBIDs.end()) //  ����������һ���ӱ������union all
							sqlstr_getids += " union all ";
					}
					
					if(pdbi->query(sqlstr_getids.c_str(), sqlstr_getids.size(), false)) //  ִ�в�ѯ
					{
						MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql()); //  ��ȡ��ѯ���
						if(pResult)
						{
							MYSQL_ROW arow; //  ��ȡ��ѯ���
							int32 count = 0; //  ������
							tabiter = childTableDBIDs.begin(); //  �����ӱ�DBID
							bool first = true; //  �Ƿ��һ��

							while((arow = mysql_fetch_row(pResult)) != NULL) //  ������ѯ���
							{
								if(count == 0) //  ����ǵ�һ��
								{
									StringConv::str2value(count, arow[0]); //  ����һ�е�ֵת��Ϊ������
									if(!first || count <= 0) //  ������ǵ�һ�λ��߼�����С�ڵ���0����tabiter++
										tabiter++;
									continue; //  ������һ��ѭ��
								}

								DBID old_dbid; //  �����DBID
								StringConv::str2value(old_dbid, arow[0]); //  ����ѯ�����ֵת��Ϊ��DBID
								tabiter->second.push_back(old_dbid); //  ����DBID��ӵ�tabiterָ���second��
								count--; //  ��������1
								first = false; //  ���ǵ�һ��
							}

							mysql_free_result(pResult); //  �ͷŲ�ѯ���
						}
					}
				}
				else if(childTableDBIDs.size() == 1) //  ����ӱ�DBID�Ĵ�СΪ1
				{
					KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin(); //  �����ӱ�
						char sqlstr[MAX_BUF * 10];
						kbe_snprintf(sqlstr, MAX_BUF * 10, "select id from " ENTITY_TABLE_PERFIX "_%s where " TABLE_PARENTID_CONST_STR "=%" PRDBID,  //  ƴ��SQL��䣬��ѯ�ӱ��е�����
							tabiter->first.c_str(),
							context.dbid);

						if(pdbi->query(sqlstr, strlen(sqlstr), false))
						{
							MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());
							if(pResult)
							{
								MYSQL_ROW arow;
								while((arow = mysql_fetch_row(pResult)) != NULL) //  ������ѯ���
								{
									DBID old_dbid;
									StringConv::str2value(old_dbid, arow[0]); //  ����ѯ���ת��ΪDBID����
									tabiter->second.push_back(old_dbid); //  ����ѯ�����ӵ��ӱ��е�DBID�б���
								}

								mysql_free_result(pResult);
							}
						}
				}
			}

			// �����Ҫ��մ˱� ��ѭ��N���Ѿ��ҵ���dbid�� ʹ���ӱ��е��ӱ�Ҳ����Чɾ��
			if(!context.isEmpty)
			{
				// ��ʼ�������е��ӱ�
				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get();
					
					if(wbox.isEmpty) //  ����ӱ�Ϊ�գ�������
						continue;

					// �󶨱��ϵ
					wbox.parentTableDBID = context.dbid;

					KBEUnordered_map<std::string, std::vector<DBID> >::iterator iter =
						childTableDBIDs.find(wbox.tableName);  //  �����ӱ��ڸ����е�DBID
					
					if(iter != childTableDBIDs.end()) //  ����ҵ����ӱ�
					{
						if(iter->second.size() > 0) //  ����ӱ�Ϊ��
						{
							wbox.dbid = iter->second.front(); //  ���ӱ��DBID��ֵ���ӱ�
							iter->second.erase(iter->second.begin()); //  ���ӱ�DBID�б���ɾ����һ��DBID
						}

						if(iter->second.size() <= 0) //  ����ӱ�DBID�б�Ϊ��
						{
							childTableDBIDs.erase(wbox.tableName); //  ���ӱ�DBID�б���ɾ�����ӱ�
						}
					}

					// �����ӱ�
					writeDB(optype, pdbi, wbox);
				}
			}
			
			// ɾ��������������
			KBEUnordered_map< std::string, std::vector<DBID> >::iterator tabiter = childTableDBIDs.begin();
			for(; tabiter != childTableDBIDs.end(); ++tabiter)
			{
				if(tabiter->second.size() == 0)
					continue;

				// ��ɾ�����ݿ��еļ�¼
				std::string sqlstr = "delete from " ENTITY_TABLE_PERFIX "_";
				sqlstr += tabiter->first;
				sqlstr += " where " TABLE_ID_CONST_STR " in (";

				std::vector<DBID>::iterator iter = tabiter->second.begin();
				for(; iter != tabiter->second.end(); ++iter)
				{
					DBID dbid = (*iter);

					char sqlstr1[MAX_BUF];
					kbe_snprintf(sqlstr1, MAX_BUF, "%" PRDBID, dbid); //  ��DBIDת��Ϊ�ַ���
					sqlstr += sqlstr1;
					sqlstr += ",";
				}
				
				sqlstr.erase(sqlstr.size() - 1); //  ȥ�����һ������
				sqlstr += ")";
				bool ret = pdbi->query(sqlstr.c_str(), sqlstr.size(), false); //  ִ��SQL���
				KBE_ASSERT(ret); //  ����ret��ֵ

				mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin(); //  ����context.optable
				for(; iter1 != context.optable.end(); ++iter1)
				{
					mysql::DBContext& wbox = *iter1->second.get(); //  ��ȡwbox
					if(wbox.tableName == tabiter->first) //  ���wbox��tableName����tabiter��first
					{
						std::vector<DBID>::iterator iter = tabiter->second.begin(); //  ����tabiter��second
						for(; iter != tabiter->second.end(); ++iter)
						{
							DBID dbid = (*iter); //  ��ȡdbid
							
							wbox.parentTableDBID = context.dbid; //  ����wbox��parentTableDBID��dbid
							wbox.dbid = dbid;
							wbox.isEmpty = true; //  ����wbox��isEmptyΪtrue

							// �����ӱ�
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

