// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_KBE_TABLE_MYSQL_H
#define KBE_KBE_TABLE_MYSQL_H

#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "db_interface/entity_table.h"
#include "db_interface/kbe_tables.h"

namespace KBEngine { 

/*
	kbe系统表
*/
class KBEEntityLogTableMysql : public KBEEntityLogTable
{
public:
	KBEEntityLogTableMysql(EntityTables* pEntityTables);
	virtual ~KBEEntityLogTableMysql(){}
	
	/**
		同步表到数据库中
		@param pdbi 数据库接口指针
		@return 成功返回true，失败返回false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		同步索引到数据库中
		@param pdbi 数据库接口指针
		@return 总是返回true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		记录实体信息到数据库
		@param pdbi 数据库接口指针
		@param ip 实体IP地址
		@param port 实体端口
		@param dbid 数据库ID
		@param componentID 组件ID
		@param entityID 实体ID
		@param entityType 实体类型
		@return 成功返回true，失败返回false
	*/
	virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
						COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType);

	/**
		查询实体信息
		@param pdbi 数据库接口指针
		@param dbid 数据库ID
		@param entitylog 实体日志对象
		@param entityType 实体类型
		@return 成功返回true，失败返回false
	*/
	virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType);

	/**
		删除实体日志
		@param pdbi 数据库接口指针
		@param dbid 数据库ID
		@param entityType 实体类型
		@return 成功返回true，失败返回false
	*/
	virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType);
	/**
		删除指定组件的实体日志
		@param pdbi 数据库接口指针
		@param componentID 组件ID
		@return 成功返回true，失败返回false
	*/
	virtual bool eraseBaseappEntityLog(DBInterface * pdbi, COMPONENT_ID componentID);

protected:
	
};

class KBEServerLogTableMysql : public KBEServerLogTable
{
public:
	KBEServerLogTableMysql(EntityTables* pEntityTables);
	virtual ~KBEServerLogTableMysql(){}
	
	/**
		同步表到数据库中
		@param pdbi 数据库接口指针
		@return 成功返回true，失败返回false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		同步索引到数据库中
		@param pdbi 数据库接口指针
		@return 总是返回true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		更新服务器信息
		@param pdbi 数据库接口指针
		@return 成功返回true，失败返回false
	*/
	virtual bool updateServer(DBInterface * pdbi);

	/**
		查询服务器信息
		@param pdbi 数据库接口指针
		@param serverlog 服务器日志对象
		@return 成功返回true，失败返回false
	*/
	virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog);
	/**
		查询所有服务器ID
		@param pdbi 数据库接口指针
		@return 服务器ID列表
	*/
	virtual std::vector<COMPONENT_ID> queryServers(DBInterface * pdbi);

	/**
		查询超时的服务器ID
		@param pdbi 数据库接口指针
		@return 超时的服务器ID列表
	*/
	virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi);

	/**
		清除指定服务器ID的记录
		@param pdbi 数据库接口指针
		@param cids 服务器ID列表
		@return 成功返回true，失败返回false
	*/
	virtual bool clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids);

	/**
		查询所有服务器的共享数据库状态
		@param pdbi 数据库接口指针
		@return 服务器ID及其共享数据库状态的映射
	*/
	virtual std::map<COMPONENT_ID, bool> queryAllServerShareDBState(DBInterface * pdbi);

	/**
		检查服务器是否使用共享数据库
		@param pdbi 数据库接口指针
		@return 1表示使用共享数据库，0表示不使用
	*/
	virtual int isShareDB(DBInterface * pdbi);
	
protected:
	
};

class KBEAccountTableMysql : public KBEAccountTable
{
public:
	KBEAccountTableMysql(EntityTables* pEntityTables);
	virtual ~KBEAccountTableMysql(){}
	
	/**
		同步表到数据库中
		@param pdbi 数据库接口指针
		@return 成功返回true，失败返回false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		同步索引到数据库中
		@param pdbi 数据库接口指针
		@return 总是返回true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		查询账户信息
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param info 账户信息对象
		@return 成功返回true，失败返回false
	*/
	bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
	/**
		查询账户所有信息
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param info 账户信息对象
		@return 成功返回true，失败返回false
	*/
	bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
	/**
		记录账户信息
		@param pdbi 数据库接口指针
		@param info 账户信息对象
		@return 成功返回true，失败返回false
	*/
	bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info);
	/**
		设置账户标志和截止日期
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param flags 标志
		@param deadline 截止日期
		@return 成功返回true，失败返回false
	*/
	bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline);
	/**
		更新账户计数
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param dbid 数据库ID
		@return 成功返回true，失败返回false
	*/
	virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid);
	/**
		更新账户密码
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param password 新密码
		@return 成功返回true，失败返回false
	*/
	virtual bool updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password);
protected:
};

class KBEEmailVerificationTableMysql : public KBEEmailVerificationTable
{
public:

	KBEEmailVerificationTableMysql(EntityTables* pEntityTables);
	virtual ~KBEEmailVerificationTableMysql();

	/**
		同步表到数据库中
		@param pdbi 数据库接口指针
		@return 成功返回true，失败返回false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		同步索引到数据库中
		@param pdbi 数据库接口指针
		@return 总是返回true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		查询账户信息
		@param pdbi 数据库接口指针
		@param type 账户类型
		@param name 账户名称
		@param info 账户信息对象
		@return 成功返回true，失败返回false
	*/
	virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info);
	/**
		记录账户信息
		@param pdbi 数据库接口指针
		@param type 账户类型
		@param name 账户名称
		@param datas 额外数据
		@param code 验证码
		@return 成功返回true，失败返回false
	*/
	virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code);
	/**
		删除账户信息
		@param pdbi 数据库接口指针
		@param type 账户类型
		@param name 账户名称
		@return 成功返回true，失败返回false
	*/
	virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name);
	/**
		激活账户
		@param pdbi 数据库接口指针
		@param code 验证码
		@param info 账户信息对象
		@return 成功返回true，失败返回false
	*/
	virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info);
	/**
		绑定邮箱
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param code 验证码
		@return 成功返回true，失败返回false
	*/
	virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code);
	/**
		重置密码
		@param pdbi 数据库接口指针
		@param name 账户名称
		@param password 新密码
		@param code 验证码
		@return 成功返回true，失败返回false
	*/
	virtual bool resetpassword(DBInterface * pdbi, const std::string& name, 
		const std::string& password, const std::string& code);

protected:
};

}

#endif // KBE_KBE_TABLE_MYSQL_H
