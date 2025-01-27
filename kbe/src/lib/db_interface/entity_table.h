// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ENTITY_TABLE_H
#define KBE_ENTITY_TABLE_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "entitydef/common.h"
#include "thread/threadmutex.h"

namespace KBEngine { 

class DBUtil;
class DBInterface;
class ScriptDefModule;
class DataType;
class PropertyDescription;
class EntityTable;
class EntityTables;
class MemoryStream;

// 定义表项类型常量
#define TABLE_ITEM_TYPE_UNKONWN		0
#define TABLE_ITEM_TYPE_FIXEDARRAY	1
#define TABLE_ITEM_TYPE_FIXEDDICT	2
#define TABLE_ITEM_TYPE_STRING		3
#define TABLE_ITEM_TYPE_DIGIT		4
#define TABLE_ITEM_TYPE_BLOB		5
#define TABLE_ITEM_TYPE_VECTOR2		6
#define TABLE_ITEM_TYPE_VECTOR3		7
#define TABLE_ITEM_TYPE_VECTOR4		8
#define TABLE_ITEM_TYPE_UNICODE		9
#define TABLE_ITEM_TYPE_ENTITYCALL	10
#define TABLE_ITEM_TYPE_PYTHON		11
#define TABLE_ITEM_TYPE_COMPONENT	12

// 定义表前缀和字段名称常量
#define KBE_TABLE_PERFIX						"kbe"
#define ENTITY_TABLE_PERFIX						"tbl"
#define TABLE_ID_CONST_STR						"id"
#define TABLE_PARENTID_CONST_STR				"parentID"
#define TABLE_ITEM_PERFIX						"sm"
#define TABLE_ARRAY_ITEM_VALUE_CONST_STR		"value"
#define TABLE_ARRAY_ITEM_VALUES_CONST_STR		"values"
#define TABLE_AUTOLOAD_CONST_STR				"autoLoad"

/**
	db表操作
*/
enum DB_TABLE_OP
{
	TABLE_OP_INSERT					= 1, //  插入
	TABLE_OP_UPDATE					= 2, //  更新
	TABLE_OP_DELETE					= 3 //  删除
};

// 账户信息结构体
struct ACCOUNT_INFOS
{
	ACCOUNT_INFOS():
	name(),
	password(),
	datas(),
	dbid(0),
	flags(0),
	deadline(0)
	{
	}

	std::string name, password, datas, email;
	DBID dbid;	// 数据库ID

	uint32 flags; //  标志位
	uint64 deadline; //  截止时间
};

/**
	维护entity在数据库中的表中的一个字段
*/
class EntityTableItem
{
public:
	EntityTableItem(std::string itemDBType, uint32 datalength, uint32 flags):
		itemName_(), //  字段名
		tableName_(), //  表名
		utype_(0), //  数据类型
		pParentTable_(NULL), //  父表指针
		pParentTableItem_(NULL), //  父表字段指针
		pDataType_(NULL), //  数据类型指针
		pPropertyDescription_(NULL),
		itemDBType_(itemDBType),
		datalength_(datalength),
		flags_(flags),
		indexType_()
	{
	};

	virtual ~EntityTableItem() {};

	// 检查是否是相同的键
	virtual bool isSameKey(std::string key) { return itemName() == key; }

	// 获取表项类型
	virtual uint8 type() const { return TABLE_ITEM_TYPE_UNKONWN; }

	// 设置字段名称
	void itemName(std::string name) { itemName_ = name; }
	const char* itemName() { return itemName_.c_str(); }

	// 设置索引类型
	void indexType(std::string index) { indexType_ = index; }
	const char* indexType() { return indexType_.c_str(); }

	// 获取数据库类型
	const char* itemDBType() { return itemDBType_.c_str(); }

	// 设置和获取用户类型
	void utype(int32/*ENTITY_PROPERTY_UID*/ utype) { utype_ = utype; }
	int32 utype() const { return utype_; }

	// 设置和获取标志
	void flags(uint32 f) { flags_ = f; }
	uint32 flags() const { return flags_; }

	// 设置和获取父表
	void pParentTable(EntityTable* v) { pParentTable_ = v; }
	EntityTable* pParentTable() { return pParentTable_; }

	// 设置和获取父表项
	void pParentTableItem(EntityTableItem* v) { pParentTableItem_ = v; }
	EntityTableItem* pParentTableItem() { return pParentTableItem_; }

	// 获取数据类型
	const DataType* pDataType() { return pDataType_; }

	// 获取数据长度
	uint32 datalength() const { return datalength_; }

	// 获取属性描述
	const PropertyDescription* pPropertyDescription() const { return pPropertyDescription_; }

	/**
		初始化
	*/
	virtual bool initialize(const PropertyDescription* pPropertyDescription, 
		const DataType* pDataType, std::string itemName) = 0;

	// 设置表名
	void tableName(std::string name) { tableName_ = name; }
	const char* tableName() { return tableName_.c_str(); }

	/**
		同步entity表到数据库中
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL) = 0;

	/**
		更新数据
	*/
	virtual bool writeItem(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule) = 0;

	/**
		获取所有的数据放到流中
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule) = 0;

protected:
	// 字段名称
	std::string itemName_;
	std::string tableName_;
	int32/*ENTITY_PROPERTY_UID*/ utype_;

	EntityTable* pParentTable_; //  父表
	EntityTableItem* pParentTableItem_; //  父表项

	const DataType* pDataType_; //  数据类型
	const PropertyDescription* pPropertyDescription_; //  属性描述

	std::string itemDBType_; //  数据库类型
	uint32 datalength_; //  数据长度
	uint32 flags_;

	std::string indexType_; //  索引类型
};

/*
	维护entity在数据库中的表
*/
class EntityTable
{
public:
	// 表项映射类型
	typedef std::map<int32/*ENTITY_PROPERTY_UID*/, KBEShared_ptr<EntityTableItem> > TABLEITEM_MAP;

	EntityTable(EntityTables* pEntityTables) :
	tableName_(),
	tableItems_(),
	tableFixedOrderItems_(),
	isChild_(false),
	sync_(false),
	pEntityTables_(pEntityTables)
	{
	};

	virtual ~EntityTable(){};
	
	void tableName(std::string name){ tableName_ = name; }
	const char* tableName(){ return tableName_.c_str(); }

	/**
		初始化
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name) = 0;

	/**
		同步entity表到数据库中
	*/
	virtual bool syncToDB(DBInterface* pdbi) = 0;

	/**
		同步entity表索引到数据库中
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) = 0;

	/** 
		创建一个表item
	*/
	virtual EntityTableItem* createItem(std::string type, std::string defaultVal) = 0;

	/** 
		获得所有表字段
	*/
	const EntityTable::TABLEITEM_MAP& tableItems() const { return tableItems_; }
	const std::vector<EntityTableItem*>& tableFixedOrderItems() const{ return tableFixedOrderItems_; }

	// 添加表项
	void addItem(EntityTableItem* pItem);

	// 检查是否为子表
	bool isChild() const{ return isChild_; }
	void isChild(bool b){ isChild_ = b; }

	// 查找表项
	EntityTableItem* findItem(int32/*ENTITY_PROPERTY_UID*/ utype);

	/**
		更新表
	*/
	virtual DBID writeTable(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		从数据库删除entity
	*/
	virtual bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		获取所有的数据放到流中
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	/**
		设置是否自动加载
	*/
	virtual void entityShouldAutoLoad(DBInterface* pdbi, DBID dbid, bool shouldAutoLoad){};

	// 检查是否已同步
	bool hasSync() const { return sync_; }

	/**
		查询自动加载的实体
	*/
	virtual void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule, 
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs){}

	// 获取表管理器
	EntityTables* pEntityTables() const { return pEntityTables_; }
	void pEntityTables(EntityTables* v){ pEntityTables_ = v; }

protected:

	// 表名称
	std::string tableName_;

	// 所有的字段
	TABLEITEM_MAP tableItems_;

	// 和ScriptDefModule中保持一致秩序的item引用
	std::vector<EntityTableItem*> tableFixedOrderItems_; 

	// 是否为子表
	bool isChild_; 

	bool sync_;

	// 表管理器
	EntityTables* pEntityTables_;
};

// 管理多个表
class EntityTables
{
public:
	// 大小写不敏感的哈希函数
	struct case_insensitive_hasher
	{
		size_t operator()(const std::string& key) const
		{
			std::string keyCopy(key);
			std::transform(keyCopy.begin(), keyCopy.end(), keyCopy.begin(), tolower);
			return std::tr1::hash<std::string>()(keyCopy);
		}
	};

	// 大小写不敏感的比较函数
	struct case_insensitive_comparer
	{
		bool operator() (const std::string& x, const std::string& y) const
		{
			return x.size() == y.size() && kbe_stricmp(x.c_str(), y.c_str()) == 0;
		}
	};

	// 表管理器映射类型
	typedef KBEUnordered_map<std::string, EntityTables> ENTITY_TABLES_MAP;
	static ENTITY_TABLES_MAP sEntityTables;
	// 通过接口名称查找表管理器
	static EntityTables& findByInterfaceName(const std::string& dbInterfaceName);

	// 表映射类型
	typedef KBEUnordered_map<std::string, KBEShared_ptr<EntityTable>, case_insensitive_hasher, case_insensitive_comparer> TABLES_MAP;

	EntityTables();
	virtual ~EntityTables();
	
	// 加载表
	bool load(DBInterface* pdbi);
	// 同步表到数据库
	bool syncToDB(DBInterface* pdbi);

	// 设置数据库接口名称
	void dbInterfaceName(const std::string& dbInterfaceName){
		dbInterfaceName_ = dbInterfaceName;
	}

	/** 
		获得所有表
	*/
	const EntityTables::TABLES_MAP& tables() const { return tables_; }

	void addTable(EntityTable* pTable); //  添加表
	EntityTable* findTable(std::string name); //  根据名称查找表

	void addKBETable(EntityTable* pTable); //  添加KBETable
	EntityTable* findKBETable(std::string name); //  根据名称查找EntityTable

	/**
		写entity到数据库
	*/
	DBID writeEntity(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		从数据库删除entity
	*/
	bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		获取某个表所有的数据放到流中
	*/
	bool queryEntity(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	void onTableSyncSuccessfully(KBEShared_ptr<EntityTable> pEntityTable, bool error);

	/**
		查询自动加载的实体
	*/
	void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule, 
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs);

protected:
	// 所有的表
	TABLES_MAP tables_;
	TABLES_MAP kbe_tables_;

	// 已同步表的数量
	int numSyncTables_;
	// 同步表时是否有错误
	bool syncTablesError_;

	// 数据库接口名称
	std::string dbInterfaceName_;
};

}

#endif // KBE_ENTITY_TABLE_H
