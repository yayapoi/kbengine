// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_DB_TASKS_H
#define KBE_DB_TASKS_H

#include "common/common.h"
#include "common/timer.h"
#include "thread/threadtask.h"

namespace KBEngine{ 

class MemoryStream;
class DBInterface;
class EntityTable;
class EntityTables;

/*
	���ݿ��߳����������
*/

class DBTaskBase : public thread::TPTask
{
public:

	DBTaskBase():
	initTime_(timestamp())
	{
	}

	virtual ~DBTaskBase(){}
	// ������������⺯��
    virtual bool process();
    // �����ݿ��߳��д�������Ĵ��麯��
    virtual bool db_thread_process() = 0;
    // ���Ի�ȡ��һ����������⺯��
    virtual DBTaskBase* tryGetNextTask() { return NULL; }
    // �����߳��г�������״̬�����⺯��
    virtual thread::TPTask::TPTaskState presentMainThread();

    // �������ݿ�ӿ�ָ������⺯��
    virtual void pdbi(DBInterface* ptr) { pdbi_ = ptr; }

    // ��ȡ���񴴽�ʱ��ĺ���
    uint64 initTime() const { return initTime_; }

protected:
    // ���ݿ�ӿ�ָ��
    DBInterface* pdbi_;
    // ���񴴽�ʱ��
    uint64 initTime_;
};

/**
	ִ��һ��sql���
*/
class DBTaskSyncTable : public DBTaskBase
{
public:
	DBTaskSyncTable(EntityTables* pEntityTables, KBEShared_ptr<EntityTable> pEntityTable);
	virtual ~DBTaskSyncTable();
	// �����ݿ��߳��д�������ĺ���
    virtual bool db_thread_process();
    // �����߳��г�������״̬�ĺ���
    virtual thread::TPTask::TPTaskState presentMainThread();

protected:
    // ʵ��������ָ��
    KBEShared_ptr<EntityTable> pEntityTable_;
    // �����Ƿ�ɹ��ı�־
    bool success_;
    // ʵ����ϵ�ָ��
    EntityTables* pEntityTables_;
};


}
#endif // KBE_DB_TASKS_H
