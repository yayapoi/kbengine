// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_KBE_TABLE_REDIS_H
#define KBE_KBE_TABLE_REDIS_H

#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "db_interface/entity_table.h"
#include "db_interface/kbe_tables.h"

namespace KBEngine { 

/*
	kbeϵͳ�����ڴ���ʵ����־��Redisʵ��
*/
class KBEEntityLogTableRedis : public KBEEntityLogTable
{
public:
	KBEEntityLogTableRedis(EntityTables* pEntityTables);
	virtual ~KBEEntityLogTableRedis(){}
	
	/**
		ͬ�������ݿ���
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
     * @brief ͬ�����������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ���Ƿ���true
     */
    virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

    /**
     * @brief ��¼ʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param ip ʵ��IP��ַ
     * @param port ʵ��˿�
     * @param dbid ���ݿ�ID
     * @param componentID ���ID
     * @param entityID ʵ��ID
     * @param entityType ʵ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
                           COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType);

    /**
     * @brief ��ѯʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param dbid ���ݿ�ID
     * @param entitylog ʵ����־����
     * @param entityType ʵ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType);

    /**
     * @brief ɾ��ʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param dbid ���ݿ�ID
     * @param entityType ʵ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType);
    /**
     * @brief ɾ��ָ�������ʵ����־
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param componentID ���ID
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool eraseBaseappEntityLog(DBInterface * pdbi, COMPONENT_ID componentID);

protected:
	
};

/**
 * @brief ���ڴ����������־��Redisʵ��
 */
class KBEServerLogTableRedis : public KBEServerLogTable
{
public:
	KBEServerLogTableRedis(EntityTables* pEntityTables);
	virtual ~KBEServerLogTableRedis(){}
	
	/**
     * @brief ͬ�������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool syncToDB(DBInterface* pdbi);
    /**
     * @brief ͬ�����������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ���Ƿ���true
     */
    virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

    /**
     * @brief ���·�������Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool updateServer(DBInterface * pdbi);

    /**
     * @brief ��ѯ��������Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param serverlog ��������־����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog);
    /**
     * @brief ��ѯ���з�����ID
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ������ID�б�
     */
    virtual std::vector<COMPONENT_ID> queryServers(DBInterface * pdbi);

    /**
     * @brief ��ѯ��ʱ�ķ�����ID
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ��ʱ������ID�б�
     */
    virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi);

    /**
     * @brief ���ָ��������ID�ķ�������Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param cids ������ID�б�
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids);
	
protected:
	
};

/**
 * @brief kbeϵͳ�����ڴ����˻���Ϣ��Redisʵ��
 */
class KBEAccountTableRedis : public KBEAccountTable
{
public:
	KBEAccountTableRedis(EntityTables* pEntityTables);
	virtual ~KBEAccountTableRedis(){}
	
	/**
     * @brief ͬ�������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool syncToDB(DBInterface* pdbi);
    /**
     * @brief ͬ�����������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ���Ƿ���true
     */
    virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

    /**
     * @brief ��ѯ�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param info �˻���Ϣ����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
    /**
     * @brief ��ѯ�˻�������Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param info �˻���Ϣ����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
    /**
     * @brief ��¼�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param info �˻���Ϣ����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info);
    /**
     * @brief �����˻���־�ͽ�ֹʱ��
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param flags ��־
     * @param deadline ��ֹʱ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline);
    /**
     * @brief �����˻�����
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param dbid ���ݿ�ID
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid);
    /**
     * @brief �����˻�����
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param password ������
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool updatePassword(DBInterface * pdbi, const std::string& name, const std::string& password);
protected:
};

/**
 * @brief ���ڴ�������ʼ���֤��Redisʵ��
 */
class KBEEmailVerificationTableRedis : public KBEEmailVerificationTable
{
public:

	KBEEmailVerificationTableRedis(EntityTables* pEntityTables);
	virtual ~KBEEmailVerificationTableRedis();

	/**
     * @brief ͬ�������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool syncToDB(DBInterface* pdbi);
    /**
     * @brief ͬ�����������ݿ���
     * @param pdbi ���ݿ�ӿ�ָ��
     * @return ���Ƿ���true
     */
    virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

    /**
     * @brief ��ѯ�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param type �˻�����
     * @param name �˻�����
     * @param info �˻���Ϣ����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info);
    /**
     * @brief ��¼�˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param type �˻�����
     * @param name �˻�����
     * @param datas ��������
     * @param code ��֤��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code);
    /**
     * @brief ɾ���˻���Ϣ
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param type �˻�����
     * @param name �˻�����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name);
    /**
     * @brief �����˻�
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param code ��֤��
     * @param info �˻���Ϣ����
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info);
    /**
     * @brief �󶨵����ʼ�
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param code ��֤��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code);
    /**
     * @brief ��������
     * @param pdbi ���ݿ�ӿ�ָ��
     * @param name �˻�����
     * @param password ������
     * @param code ��֤��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool resetpassword(DBInterface * pdbi, const std::string& name, 
                               const std::string& password, const std::string& code);

    /**
     * @brief ��ȡ��ֹʱ��
     * @param type �˻�����
     * @return ��ֹʱ��
     */
    int getDeadline(int8 type);
	
protected:
};

}

#endif // KBE_KBE_TABLE_REDIS_H
