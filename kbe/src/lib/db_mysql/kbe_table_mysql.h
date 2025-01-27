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
	kbeϵͳ��
*/
class KBEEntityLogTableMysql : public KBEEntityLogTable
{
public:
	KBEEntityLogTableMysql(EntityTables* pEntityTables);
	virtual ~KBEEntityLogTableMysql(){}
	
	/**
		ͬ�������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		ͬ�����������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return ���Ƿ���true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		��¼ʵ����Ϣ�����ݿ�
		@param pdbi ���ݿ�ӿ�ָ��
		@param ip ʵ��IP��ַ
		@param port ʵ��˿�
		@param dbid ���ݿ�ID
		@param componentID ���ID
		@param entityID ʵ��ID
		@param entityType ʵ������
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool logEntity(DBInterface * pdbi, const char* ip, uint32 port, DBID dbid,
						COMPONENT_ID componentID, ENTITY_ID entityID, ENTITY_SCRIPT_UID entityType);

	/**
		��ѯʵ����Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param dbid ���ݿ�ID
		@param entitylog ʵ����־����
		@param entityType ʵ������
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool queryEntity(DBInterface * pdbi, DBID dbid, EntityLog& entitylog, ENTITY_SCRIPT_UID entityType);

	/**
		ɾ��ʵ����־
		@param pdbi ���ݿ�ӿ�ָ��
		@param dbid ���ݿ�ID
		@param entityType ʵ������
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool eraseEntityLog(DBInterface * pdbi, DBID dbid, ENTITY_SCRIPT_UID entityType);
	/**
		ɾ��ָ�������ʵ����־
		@param pdbi ���ݿ�ӿ�ָ��
		@param componentID ���ID
		@return �ɹ�����true��ʧ�ܷ���false
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
		ͬ�������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		ͬ�����������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return ���Ƿ���true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		���·�������Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool updateServer(DBInterface * pdbi);

	/**
		��ѯ��������Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param serverlog ��������־����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool queryServer(DBInterface * pdbi, ServerLog& serverlog);
	/**
		��ѯ���з�����ID
		@param pdbi ���ݿ�ӿ�ָ��
		@return ������ID�б�
	*/
	virtual std::vector<COMPONENT_ID> queryServers(DBInterface * pdbi);

	/**
		��ѯ��ʱ�ķ�����ID
		@param pdbi ���ݿ�ӿ�ָ��
		@return ��ʱ�ķ�����ID�б�
	*/
	virtual std::vector<COMPONENT_ID> queryTimeOutServers(DBInterface * pdbi);

	/**
		���ָ��������ID�ļ�¼
		@param pdbi ���ݿ�ӿ�ָ��
		@param cids ������ID�б�
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool clearServers(DBInterface * pdbi, const std::vector<COMPONENT_ID>& cids);

	/**
		��ѯ���з������Ĺ������ݿ�״̬
		@param pdbi ���ݿ�ӿ�ָ��
		@return ������ID���乲�����ݿ�״̬��ӳ��
	*/
	virtual std::map<COMPONENT_ID, bool> queryAllServerShareDBState(DBInterface * pdbi);

	/**
		���������Ƿ�ʹ�ù������ݿ�
		@param pdbi ���ݿ�ӿ�ָ��
		@return 1��ʾʹ�ù������ݿ⣬0��ʾ��ʹ��
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
		ͬ�������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		ͬ�����������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return ���Ƿ���true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		��ѯ�˻���Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param info �˻���Ϣ����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	bool queryAccount(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
	/**
		��ѯ�˻�������Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param info �˻���Ϣ����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	bool queryAccountAllInfos(DBInterface * pdbi, const std::string& name, ACCOUNT_INFOS& info);
	/**
		��¼�˻���Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param info �˻���Ϣ����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	bool logAccount(DBInterface * pdbi, ACCOUNT_INFOS& info);
	/**
		�����˻���־�ͽ�ֹ����
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param flags ��־
		@param deadline ��ֹ����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	bool setFlagsDeadline(DBInterface * pdbi, const std::string& name, uint32 flags, uint64 deadline);
	/**
		�����˻�����
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param dbid ���ݿ�ID
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool updateCount(DBInterface * pdbi, const std::string& name, DBID dbid);
	/**
		�����˻�����
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param password ������
		@return �ɹ�����true��ʧ�ܷ���false
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
		ͬ�������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool syncToDB(DBInterface* pdbi);
	/**
		ͬ�����������ݿ���
		@param pdbi ���ݿ�ӿ�ָ��
		@return ���Ƿ���true
	*/
	virtual bool syncIndexToDB(DBInterface* pdbi) { return true; }

	/**
		��ѯ�˻���Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param type �˻�����
		@param name �˻�����
		@param info �˻���Ϣ����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool queryAccount(DBInterface * pdbi, int8 type, const std::string& name, ACCOUNT_INFOS& info);
	/**
		��¼�˻���Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param type �˻�����
		@param name �˻�����
		@param datas ��������
		@param code ��֤��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool logAccount(DBInterface * pdbi, int8 type, const std::string& name, const std::string& datas, const std::string& code);
	/**
		ɾ���˻���Ϣ
		@param pdbi ���ݿ�ӿ�ָ��
		@param type �˻�����
		@param name �˻�����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool delAccount(DBInterface * pdbi, int8 type, const std::string& name);
	/**
		�����˻�
		@param pdbi ���ݿ�ӿ�ָ��
		@param code ��֤��
		@param info �˻���Ϣ����
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool activateAccount(DBInterface * pdbi, const std::string& code, ACCOUNT_INFOS& info);
	/**
		������
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param code ��֤��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool bindEMail(DBInterface * pdbi, const std::string& name, const std::string& code);
	/**
		��������
		@param pdbi ���ݿ�ӿ�ָ��
		@param name �˻�����
		@param password ������
		@param code ��֤��
		@return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual bool resetpassword(DBInterface * pdbi, const std::string& name, 
		const std::string& password, const std::string& code);

protected:
};

}

#endif // KBE_KBE_TABLE_MYSQL_H
