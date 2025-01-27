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

// ����������ͳ���
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

// �����ǰ׺���ֶ����Ƴ���
#define KBE_TABLE_PERFIX						"kbe"
#define ENTITY_TABLE_PERFIX						"tbl"
#define TABLE_ID_CONST_STR						"id"
#define TABLE_PARENTID_CONST_STR				"parentID"
#define TABLE_ITEM_PERFIX						"sm"
#define TABLE_ARRAY_ITEM_VALUE_CONST_STR		"value"
#define TABLE_ARRAY_ITEM_VALUES_CONST_STR		"values"
#define TABLE_AUTOLOAD_CONST_STR				"autoLoad"

/**
	db�����
*/
enum DB_TABLE_OP
{
	TABLE_OP_INSERT					= 1, //  ����
	TABLE_OP_UPDATE					= 2, //  ����
	TABLE_OP_DELETE					= 3 //  ɾ��
};

// �˻���Ϣ�ṹ��
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
	DBID dbid;	// ���ݿ�ID

	uint32 flags; //  ��־λ
	uint64 deadline; //  ��ֹʱ��
};

/**
	ά��entity�����ݿ��еı��е�һ���ֶ�
*/
class EntityTableItem
{
public:
	EntityTableItem(std::string itemDBType, uint32 datalength, uint32 flags):
		itemName_(), //  �ֶ���
		tableName_(), //  ����
		utype_(0), //  ��������
		pParentTable_(NULL), //  ����ָ��
		pParentTableItem_(NULL), //  �����ֶ�ָ��
		pDataType_(NULL), //  ��������ָ��
		pPropertyDescription_(NULL),
		itemDBType_(itemDBType),
		datalength_(datalength),
		flags_(flags),
		indexType_()
	{
	};

	virtual ~EntityTableItem() {};

	// ����Ƿ�����ͬ�ļ�
	virtual bool isSameKey(std::string key) { return itemName() == key; }

	// ��ȡ��������
	virtual uint8 type() const { return TABLE_ITEM_TYPE_UNKONWN; }

	// �����ֶ�����
	void itemName(std::string name) { itemName_ = name; }
	const char* itemName() { return itemName_.c_str(); }

	// ������������
	void indexType(std::string index) { indexType_ = index; }
	const char* indexType() { return indexType_.c_str(); }

	// ��ȡ���ݿ�����
	const char* itemDBType() { return itemDBType_.c_str(); }

	// ���úͻ�ȡ�û�����
	void utype(int32/*ENTITY_PROPERTY_UID*/ utype) { utype_ = utype; }
	int32 utype() const { return utype_; }

	// ���úͻ�ȡ��־
	void flags(uint32 f) { flags_ = f; }
	uint32 flags() const { return flags_; }

	// ���úͻ�ȡ����
	void pParentTable(EntityTable* v) { pParentTable_ = v; }
	EntityTable* pParentTable() { return pParentTable_; }

	// ���úͻ�ȡ������
	void pParentTableItem(EntityTableItem* v) { pParentTableItem_ = v; }
	EntityTableItem* pParentTableItem() { return pParentTableItem_; }

	// ��ȡ��������
	const DataType* pDataType() { return pDataType_; }

	// ��ȡ���ݳ���
	uint32 datalength() const { return datalength_; }

	// ��ȡ��������
	const PropertyDescription* pPropertyDescription() const { return pPropertyDescription_; }

	/**
		��ʼ��
	*/
	virtual bool initialize(const PropertyDescription* pPropertyDescription, 
		const DataType* pDataType, std::string itemName) = 0;

	// ���ñ���
	void tableName(std::string name) { tableName_ = name; }
	const char* tableName() { return tableName_.c_str(); }

	/**
		ͬ��entity�����ݿ���
	*/
	virtual bool syncToDB(DBInterface* pdbi, void* pData = NULL) = 0;

	/**
		��������
	*/
	virtual bool writeItem(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule) = 0;

	/**
		��ȡ���е����ݷŵ�����
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule) = 0;

protected:
	// �ֶ�����
	std::string itemName_;
	std::string tableName_;
	int32/*ENTITY_PROPERTY_UID*/ utype_;

	EntityTable* pParentTable_; //  ����
	EntityTableItem* pParentTableItem_; //  ������

	const DataType* pDataType_; //  ��������
	const PropertyDescription* pPropertyDescription_; //  ��������

	std::string itemDBType_; //  ���ݿ�����
	uint32 datalength_; //  ���ݳ���
	uint32 flags_;

	std::string indexType_; //  ��������
};

/*
	ά��entity�����ݿ��еı�
*/
class EntityTable
{
public:
	// ����ӳ������
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
		��ʼ��
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name) = 0;

	/**
		ͬ��entity�����ݿ���
	*/
	virtual bool syncToDB(DBInterface* pdbi) = 0;

	/**
		ͬ��entity�����������ݿ���
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) = 0;

	/** 
		����һ����item
	*/
	virtual EntityTableItem* createItem(std::string type, std::string defaultVal) = 0;

	/** 
		������б��ֶ�
	*/
	const EntityTable::TABLEITEM_MAP& tableItems() const { return tableItems_; }
	const std::vector<EntityTableItem*>& tableFixedOrderItems() const{ return tableFixedOrderItems_; }

	// ��ӱ���
	void addItem(EntityTableItem* pItem);

	// ����Ƿ�Ϊ�ӱ�
	bool isChild() const{ return isChild_; }
	void isChild(bool b){ isChild_ = b; }

	// ���ұ���
	EntityTableItem* findItem(int32/*ENTITY_PROPERTY_UID*/ utype);

	/**
		���±�
	*/
	virtual DBID writeTable(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		�����ݿ�ɾ��entity
	*/
	virtual bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		��ȡ���е����ݷŵ�����
	*/
	virtual bool queryTable(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	/**
		�����Ƿ��Զ�����
	*/
	virtual void entityShouldAutoLoad(DBInterface* pdbi, DBID dbid, bool shouldAutoLoad){};

	// ����Ƿ���ͬ��
	bool hasSync() const { return sync_; }

	/**
		��ѯ�Զ����ص�ʵ��
	*/
	virtual void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule, 
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs){}

	// ��ȡ�������
	EntityTables* pEntityTables() const { return pEntityTables_; }
	void pEntityTables(EntityTables* v){ pEntityTables_ = v; }

protected:

	// ������
	std::string tableName_;

	// ���е��ֶ�
	TABLEITEM_MAP tableItems_;

	// ��ScriptDefModule�б���һ�������item����
	std::vector<EntityTableItem*> tableFixedOrderItems_; 

	// �Ƿ�Ϊ�ӱ�
	bool isChild_; 

	bool sync_;

	// �������
	EntityTables* pEntityTables_;
};

// ��������
class EntityTables
{
public:
	// ��Сд�����еĹ�ϣ����
	struct case_insensitive_hasher
	{
		size_t operator()(const std::string& key) const
		{
			std::string keyCopy(key);
			std::transform(keyCopy.begin(), keyCopy.end(), keyCopy.begin(), tolower);
			return std::tr1::hash<std::string>()(keyCopy);
		}
	};

	// ��Сд�����еıȽϺ���
	struct case_insensitive_comparer
	{
		bool operator() (const std::string& x, const std::string& y) const
		{
			return x.size() == y.size() && kbe_stricmp(x.c_str(), y.c_str()) == 0;
		}
	};

	// �������ӳ������
	typedef KBEUnordered_map<std::string, EntityTables> ENTITY_TABLES_MAP;
	static ENTITY_TABLES_MAP sEntityTables;
	// ͨ���ӿ����Ʋ��ұ������
	static EntityTables& findByInterfaceName(const std::string& dbInterfaceName);

	// ��ӳ������
	typedef KBEUnordered_map<std::string, KBEShared_ptr<EntityTable>, case_insensitive_hasher, case_insensitive_comparer> TABLES_MAP;

	EntityTables();
	virtual ~EntityTables();
	
	// ���ر�
	bool load(DBInterface* pdbi);
	// ͬ�������ݿ�
	bool syncToDB(DBInterface* pdbi);

	// �������ݿ�ӿ�����
	void dbInterfaceName(const std::string& dbInterfaceName){
		dbInterfaceName_ = dbInterfaceName;
	}

	/** 
		������б�
	*/
	const EntityTables::TABLES_MAP& tables() const { return tables_; }

	void addTable(EntityTable* pTable); //  ��ӱ�
	EntityTable* findTable(std::string name); //  �������Ʋ��ұ�

	void addKBETable(EntityTable* pTable); //  ���KBETable
	EntityTable* findKBETable(std::string name); //  �������Ʋ���EntityTable

	/**
		дentity�����ݿ�
	*/
	DBID writeEntity(DBInterface* pdbi, DBID dbid, int8 shouldAutoLoad, MemoryStream* s, ScriptDefModule* pModule);

	/**
		�����ݿ�ɾ��entity
	*/
	bool removeEntity(DBInterface* pdbi, DBID dbid, ScriptDefModule* pModule);

	/**
		��ȡĳ�������е����ݷŵ�����
	*/
	bool queryEntity(DBInterface* pdbi, DBID dbid, MemoryStream* s, ScriptDefModule* pModule);

	void onTableSyncSuccessfully(KBEShared_ptr<EntityTable> pEntityTable, bool error);

	/**
		��ѯ�Զ����ص�ʵ��
	*/
	void queryAutoLoadEntities(DBInterface* pdbi, ScriptDefModule* pModule, 
		ENTITY_ID start, ENTITY_ID end, std::vector<DBID>& outs);

protected:
	// ���еı�
	TABLES_MAP tables_;
	TABLES_MAP kbe_tables_;

	// ��ͬ���������
	int numSyncTables_;
	// ͬ����ʱ�Ƿ��д���
	bool syncTablesError_;

	// ���ݿ�ӿ�����
	std::string dbInterfaceName_;
};

}

#endif // KBE_ENTITY_TABLE_H
