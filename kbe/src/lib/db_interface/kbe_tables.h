// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_KBE_TABLES_H
#define KBE_KBE_TABLES_H

#include "entity_table.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace KBEngine { 

/**
 * @brief �������࣬�̳���EntityTable
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
		ͬ��entity�����ݿ���
	*/
	virtual bool syncToDB(DBInterface* pdbi) = 0;
	
	/**
		��ʼ��
	*/
	virtual bool initialize(ScriptDefModule* sm, std::string name) { return true; };
	
	/**
     * @brief ��������
     * @param type ����
     * @param defaultVal Ĭ��ֵ
     * @return ���ش����ı���ָ�룬ʧ�ܷ���NULL
     */
	virtual EntityTableItem* createItem(std::string type, std::string defaultVal) { return NULL; }
	
protected:

};

/*
	kbeϵͳ�����ڼ�¼ʵ����־
*/
class KBEEntityLogTable : public KBETable
{
public:
	/**
     * @brief ʵ����־�ṹ��
     */
	struct EntityLog
	{
		DBID dbid; // ���ݿ�ID
        ENTITY_ID entityID; // ʵ��ID
        char ip[MAX_IP]; // IP��ַ
        uint16 port; // �˿�
        COMPONENT_ID componentID; // ���ID

		// ��˭��¼
		COMPONENT_ID serverGroupID; // ��������ID
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
     * @brief ��¼ʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param ip IP��ַ
     * @param port �˿�
     * @param dbid ���ݿ�ID
     * @param componentID ���ID
     * @param entityID ʵ��ID
     * @param entityType ʵ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
                           COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType) = 0;

    /**
     * @brief ��ѯʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param dbid ���ݿ�ID
     * @param entitylog ʵ����־�ṹ������
     * @param entityType ʵ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType) = 0;

    /**
     * @brief ɾ��ʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param dbid ���ݿ�ID
     * @param entityType ʵ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType) = 0;
    /**
     * @brief ɾ������Ӧ�õ�ʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param componentID ���ID
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool eraseBaseappEntityLog(DBInterface * pdbi, COMPONENT_ID componentID) = 0;

protected:
	
};

/*
	kbeϵͳ�����ڼ�¼��������־
*/
class KBEServerLogTable : public KBETable
{
public:
	const static uint32 TIMEOUT = 3600;	// ��ʱʱ��

	/**
     * @brief ��������־�ṹ��
     */
    struct ServerLog
    {
        uint64 heartbeatTime; // ����ʱ��

        // ��˭��¼
        COMPONENT_ID serverGroupID; // ��������ID

        uint8 isShareDB; // �Ƿ������ݿ�
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
     * @brief ���·�������־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool updateServer(DBInterface * pdbi) = 0;

    /**
     * @brief ��ѯ��������־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param serverlog ��������־�ṹ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog) = 0;
    /**
     * @brief ��ѯ���з�����
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ���ط�����ID������
     */
    virtual std::vector<COMPONENT_ID> queryServers(DBInterface * pdbi) = 0;

    /**
     * @brief ��ѯ��ʱ�ķ�����
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ���س�ʱ������ID������
     */
    virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi) = 0;

    /**
     * @brief ���ָ���ķ�����
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param cids ������ID������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids) = 0;
	
protected:
	
};

/**
 * @brief ���ڼ�¼�˻���Ϣ
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
     * @brief ��ѯ�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param info �˻���Ϣ�ṹ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief ��¼�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param info �˻���Ϣ�ṹ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief ���ñ�־�ͽ�ֹʱ��
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param flags ��־
     * @param deadline ��ֹʱ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline) = 0;
    /**
     * @brief ���¼���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param dbid ���ݿ�ID
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid) = 0;
    /**
     * @brief ��ѯ�˻�������Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param info �˻���Ϣ�ṹ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief ��������
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param password ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password) = 0;

    /**
     * @brief ��ȡ�˻������ڴ���
     * @return �����ڴ�������
     */
    MemoryStream& accountDefMemoryStream()
    { 
        return accountDefMemoryStream_; 
    }

    /**
     * @brief �����˻������ڴ���
     * @param s �ڴ�������
     */
    void accountDefMemoryStream(MemoryStream& s)
    {
        accountDefMemoryStream_.clear(false);
        accountDefMemoryStream_.append(s.data() + s.rpos(), s.length()); 
    }

protected:
    MemoryStream accountDefMemoryStream_; // �˻������ڴ���
};

/**
 * @brief ���ڼ�¼������֤��Ϣ
 */
class KBEEmailVerificationTable : public KBETable
{
public:
	enum V_TYPE
	{
		V_TYPE_CREATEACCOUNT = 1, //  �����˻�
		V_TYPE_RESETPASSWORD = 2, //  ��������
		V_TYPE_BIND_MAIL = 3 //  ������
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
     * @brief ��ѯ�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param type ��֤����
     * @param name �˻���
     * @param info �˻���Ϣ�ṹ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief ��¼�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param type ��֤����
     * @param name �˻���
     * @param datas ��������
     * @param code ��֤��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code) = 0;
    /**
     * @brief ɾ���˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param type ��֤����
     * @param name �˻���
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name) = 0;
    /**
     * @brief �����˻�
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param code ��֤��
     * @param info �˻���Ϣ�ṹ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info) = 0;
    /**
     * @brief ������
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param code ��֤��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code) = 0;
    /**
     * @brief ��������
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻���
     * @param password ������
     * @param code ��֤��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
	virtual bool resetpassword(DBInterface * pdbi, const std::string& name, const std::string& password, const std::string& code) = 0;

protected:
};

}

#endif // KBE_KBE_TABLES_H
