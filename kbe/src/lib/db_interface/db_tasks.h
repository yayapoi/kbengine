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
	数据库线程任务基础类
*/

class DBTaskBase : public thread::TPTask
{
public:

	DBTaskBase():
	initTime_(timestamp())
	{
	}

	virtual ~DBTaskBase(){}
	// 处理任务的虚拟函数
    virtual bool process();
    // 在数据库线程中处理任务的纯虚函数
    virtual bool db_thread_process() = 0;
    // 尝试获取下一个任务的虚拟函数
    virtual DBTaskBase* tryGetNextTask() { return NULL; }
    // 在主线程中呈现任务状态的虚拟函数
    virtual thread::TPTask::TPTaskState presentMainThread();

    // 设置数据库接口指针的虚拟函数
    virtual void pdbi(DBInterface* ptr) { pdbi_ = ptr; }

    // 获取任务创建时间的函数
    uint64 initTime() const { return initTime_; }

protected:
    // 数据库接口指针
    DBInterface* pdbi_;
    // 任务创建时间
    uint64 initTime_;
};

/**
	执行一条sql语句
*/
class DBTaskSyncTable : public DBTaskBase
{
public:
	DBTaskSyncTable(EntityTables* pEntityTables, KBEShared_ptr<EntityTable> pEntityTable);
	virtual ~DBTaskSyncTable();
	// 在数据库线程中处理任务的函数
    virtual bool db_thread_process();
    // 在主线程中呈现任务状态的函数
    virtual thread::TPTask::TPTaskState presentMainThread();

protected:
    // 实体表的智能指针
    KBEShared_ptr<EntityTable> pEntityTable_;
    // 操作是否成功的标志
    bool success_;
    // 实体表集合的指针
    EntityTables* pEntityTables_;
};


}
#endif // KBE_DB_TASKS_H
