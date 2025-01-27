// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_KBE_TABLES_H
#define KBE_KBE_TABLES_H

#include "entity_table.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace KBEngine { 

/**
 * @brief 基础表类，继承自EntityTable
 */
class KBETable : public EntityTable
{
public:
	KBETable(EntityTables* pEntityTables) :
	EntityTable(pEntityTables)
	{
	}
	
	virtual ~KBETable()
	{
	}
	
	/**
		同步entity表到数据库中
	*/
	virtual bool syncToDB(DBInterface* pdbi) = 0;
	
	/**
		初始化
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name) { return true; };
	
	/**
     * @brief 创建表项
     * @param type 类型
     * @param defaultVal 默认值
     * @return 返回创建的表项指针，失败返回NULL
     */
	virtual EntityTableItem* createItem(std::string type, std::string defaultVal) { return NULL; }
	
protected:

};

/*
	kbe系统表，用于记录实体日志
*/
class KBEEntityLogTable : public KBETable
{
public:
	/**
     * @brief 实体日志结构体
     */
	struct EntityLog
	{
		DBID dbid; // 数据库ID
        ENTITY_ID entityID; // 实体ID
        char ip[MAX_IP]; // IP地址
        uint16 port; // 端口
        COMPONENT_ID componentID; // 组件ID

		// 由谁记录
		COMPONENT_ID serverGroupID; // 服务器组ID
	};

	KBEEntityLogTable(EntityTables* pEntityTables) :
	KBETable(pEntityTables)
	{
		tableName(KBE_TABLE_PERFIX "_entitylog");
	}
	
	virtual ~KBEEntityLogTable()
	{
	}
	
	/**
     * @brief 记录实体日志
     * @param pdbi 数据库接口指针
     * @param ip IP地址
     * @param port 端口
     * @param dbid 数据库ID
     * @param componentID 组件ID
     * @param entityID 实体ID
     * @param entityType 实体类型
     * @return 成功返回true，失败返回false
     */
    virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
                           COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType) = 0;

    /**
     * @brief 查询实体日志
     * @param pdbi 数据库接口指针
     * @param dbid 数据库ID
     * @param entitylog 实体日志结构体引用
     * @param entityType 实体类型
     * @return 成功返回true，失败返回false
     */
    virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType) = 0;

    /**
     * @brief 删除实体日志
     * @param pdbi 数据库接口指针
     * @param dbid 数据库ID
     * @param entityType 实体类型
     * @return 成功返回true，失败返回false
     */
    virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType) = 0;
    /**
     * @brief 删除基础应用的实体日志
     * @param pdbi 数据库接口指针
     * @param componentID 组件ID
     * @return 成功返回true，失败返回false
     */
    virtual bool eraseBaseappEntityLog(DBInterface * pdbi, COMPONENT_ID componentID) = 0;

protected:
	
};

/*
	kbe系统表，用于记录服务器日志
*/
class KBEServerLogTable : public KBETable
{
public:
	const static uint32 TIMEOUT = 3600;	// 超时时间

	/**
     * @brief 服务器日志结构体
     */
    struct ServerLog
    {
        uint64 heartbeatTime; // 心跳时间

        // 由谁记录
        COMPONENT_ID serverGroupID; // 服务器组ID

        uint8 isShareDB; // 是否共享数据库
    };

	KBEServerLogTable(EntityTables* pEntityTables) :
	KBETable(pEntityTables)
	{
		tableName(KBE_TABLE_PERFIX "_serverlog");
	}
	
	virtual ~KBEServerLogTable()
	{
	}
	
	/**
     * @brief 更新服务器日志
     * @param pdbi 数据库接口指针
     * @return 成功返回true，失败返回false
     */
    virtual bool updateServer(DBInterface * pdbi) = 0;

    /**
     * @brief 查询服务器日志
     * @param pdbi 数据库接口指针
     * @param serverlog 服务器日志结构体引用
     * @return 成功返回true，失败返回false
     */
    virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog) = 0;
    /**
     * @brief 查询所有服务器
     * @param pdbi 数据库接口指针
     * @return 返回服务器ID的向量
     */
    virtual std::vector<COMPONENT_ID> queryServers(DBInterface * pdbi) = 0;

    /**
     * @brief 查询超时的服务器
     * @param pdbi 数据库接口指针
     * @return 返回超时服务器ID的向量
     */
    virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi) = 0;

    /**
     * @brief 清除指定的服务器
     * @param pdbi 数据库接口指针
     * @param cids 服务器ID的向量
     * @return 成功返回true，失败返回false
     */
    virtual bool clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids) = 0;
	
protected:
	
};

/**
 * @brief 用于记录账户信息
 */
class KBEAccountTable : public KBETable
{
public:
	KBEAccountTable(EntityTables* pEntityTables) :
	KBETable(pEntityTables),
	accountDefMemoryStream_()
	{
		tableName(KBE_TABLE_PERFIX "_accountinfos");
	}
	
	virtual ~KBEAccountTable()
	{
	}

	/**
     * @brief 查询账户信息
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param info 账户信息结构体引用
     * @return 成功返回true，失败返回false
     */
    virtual bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief 记录账户信息
     * @param pdbi 数据库接口指针
     * @param info 账户信息结构体引用
     * @return 成功返回true，失败返回false
     */
    virtual bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief 设置标志和截止时间
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param flags 标志
     * @param deadline 截止时间
     * @return 成功返回true，失败返回false
     */
    virtual bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline) = 0;
    /**
     * @brief 更新计数
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param dbid 数据库ID
     * @return 成功返回true，失败返回false
     */
    virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid) = 0;
    /**
     * @brief 查询账户所有信息
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param info 账户信息结构体引用
     * @return 成功返回true，失败返回false
     */
    virtual bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief 更新密码
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param password 新密码
     * @return 成功返回true，失败返回false
     */
    virtual bool updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password) = 0;

    /**
     * @brief 获取账户定义内存流
     * @return 返回内存流引用
     */
    MemoryStream& accountDefMemoryStream()
    { 
        return accountDefMemoryStream_; 
    }

    /**
     * @brief 设置账户定义内存流
     * @param s 内存流引用
     */
    void accountDefMemoryStream(MemoryStream& s)
    {
        accountDefMemoryStream_.clear(false);
        accountDefMemoryStream_.append(s.data() + s.rpos(), s.length()); 
    }

protected:
    MemoryStream accountDefMemoryStream_; // 账户定义内存流
};

/**
 * @brief 用于记录邮箱验证信息
 */
class KBEEmailVerificationTable : public KBETable
{
public:
	enum V_TYPE
	{
		V_TYPE_CREATEACCOUNT = 1, //  创建账户
		V_TYPE_RESETPASSWORD = 2, //  重置密码
		V_TYPE_BIND_MAIL = 3 //  绑定邮箱
	};

	KBEEmailVerificationTable(EntityTables* pEntityTables) :
	KBETable(pEntityTables)
	{
		tableName(KBE_TABLE_PERFIX "_email_verification");
	}
	
	virtual ~KBEEmailVerificationTable()
	{
	}

	/**
     * @brief 查询账户信息
     * @param pdbi 数据库接口指针
     * @param type 验证类型
     * @param name 账户名
     * @param info 账户信息结构体引用
     * @return 成功返回true，失败返回false
     */
    virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief 记录账户信息
     * @param pdbi 数据库接口指针
     * @param type 验证类型
     * @param name 账户名
     * @param datas 额外数据
     * @param code 验证码
     * @return 成功返回true，失败返回false
     */
    virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code) = 0;
    /**
     * @brief 删除账户信息
     * @param pdbi 数据库接口指针
     * @param type 验证类型
     * @param name 账户名
     * @return 成功返回true，失败返回false
     */
    virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name) = 0;
    /**
     * @brief 激活账户
     * @param pdbi 数据库接口指针
     * @param code 验证码
     * @param info 账户信息结构体引用
     * @return 成功返回true，失败返回false
     */
    virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief 绑定邮箱
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param code 验证码
     * @return 成功返回true，失败返回false
     */
    virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code) = 0;
    /**
     * @brief 重置密码
     * @param pdbi 数据库接口指针
     * @param name 账户名
     * @param password 新密码
     * @param code 验证码
     * @return 成功返回true，失败返回false
     */
	virtual bool resetpassword(DBInterface * pdbi, const std::string& name, const std::string& password, const std::string& code) = 0;

protected:
};

}

#endif // KBE_KBE_TABLES_H
