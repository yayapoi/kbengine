// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_MYSQL_DB_RW_CONTEXT_H
#define KBE_MYSQL_DB_RW_CONTEXT_H

#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace KBEngine { 
namespace mysql {

/**
	读写删操作时会用到，包含取到或待写入的各种信息。

	dbid：如果是主表就是实体的dbid，子表就是当前查询的dbid

	dbids：主表上dbids中只有一个dbid，就是实体的id，如果实体数据存在数组类则会有子表出现，当这个数据结构描述的是一个子表的时候
		dbids是这个数组的子表索引, 每个dbid都表示这个子表上对应的值并且按照排列顺序同时也表示在数组中对应位置的值。
		dbids = {
			123 : [xxx, xxx, ...], // 123为父表上的某个dbid，数组为在子表上与父表相关联的的dbids。
			...
		}

	items：中有这个表的字段信息，如果是写库则字段中也有对应的要写值。

	optable：子表结构

	results：读操作时查询到的数据, 数据的排列对应items中的strkey的数量乘以dbids的数量。
	readresultIdx：因为results中的数量是dbids * items，所以当在某些递归读的时候填充数据会根据这个readresultIdx计算填充的位置。

	parentTableDBID：父表的dbid
	parentTableName：父表的名称

	tableName：当前表的名称
 */
class DBContext
{
public:
	/**
		存储所有要操作的表item结构
	*/
	struct DB_ITEM_DATA
	{
		DB_ITEM_DATA()
		{
			sqlkey = NULL;
		}

		char sqlval[MAX_BUF];       // SQL 值
        const char* sqlkey;         // SQL 键
        std::string extraDatas;     // 额外数据
	};

	typedef std::vector< std::pair< std::string/*tableName*/, KBEShared_ptr< DBContext > > > DB_RW_CONTEXTS; //  定义一个数据库读写上下文的向量，每个元素包含一个表名和一个数据库上下文指针
	typedef std::vector< KBEShared_ptr<DB_ITEM_DATA>  > DB_ITEM_DATAS; //  定义一个数据库项数据的向量，每个元素包含一个数据库项数据指针

	DBContext()
	{
	}

	~DBContext()
	{
	}
	
	DB_ITEM_DATAS items;            // 表的字段信息

    std::string tableName;          // 当前表的名称
    std::string parentTableName;    // 父表的名称
    
    DBID parentTableDBID;           // 父表的dbid
    DBID dbid;                      // 当前表的dbid
    
    DB_RW_CONTEXTS optable;         // 子表结构
    
    bool isEmpty;                   // 是否为空

    std::map<DBID, std::vector<DBID> > dbids;  // 子表索引
    std::map<DBID, std::pair< std::vector<std::string>::size_type, std::vector<std::string> > > results;  // 读操作时查询到的数据

private:

};

}
}
#endif // KBE_MYSQL_DB_RW_CONTEXT_H

