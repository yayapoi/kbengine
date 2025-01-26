// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_EVENT_POLLER_H
#define KBE_EVENT_POLLER_H

#include "common/common.h"
#include "common/timestamp.h"
#include "network/interfaces.h"
#include "thread/concurrency.h"
#include "network/common.h"
#include <map>

namespace KBEngine { 
namespace Network
{
	
class InputNotificationHandler;
typedef std::map<int, InputNotificationHandler *> FDReadHandlers;
typedef std::map<int, OutputNotificationHandler *> FDWriteHandlers;

// 是一个事件轮询器，用于管理文件描述符的读写事件。它允许注册和注销文件描述符的读写事件，并处理这些事件
class EventPoller
{
public:
	EventPoller();
	virtual ~EventPoller();

	/**
     * @brief 注册文件描述符的读事件。
     * 
     * @param fd 文件描述符。
     * @param handler 指向 InputNotificationHandler 对象的指针，用于处理读事件。
     * @return 注册成功返回 true，否则返回 false。
     */
	bool registerForRead(int fd, InputNotificationHandler * handler);
	bool registerForWrite(int fd, OutputNotificationHandler * handler);

	 /**
     * @brief 注销文件描述符的读事件。
     * 
     * @param fd 文件描述符。
     * @return 注销成功返回 true，否则返回 false。
     */
	bool deregisterForRead(int fd);
	bool deregisterForWrite(int fd);


	/**
     * @brief 处理待处理的事件。
     * 
     * @param maxWait 最大等待时间（秒）。
     * @return 处理的事件数量。
     */
	virtual int processPendingEvents(double maxWait) = 0;
	virtual int getFileDescriptor() const; //  虚函数，获取文件描述符

	// 清除空闲时间。
	void clearSpareTime()		{spareTime_ = 0;}
	// 获取空闲时间。
	uint64 spareTime() const	{return spareTime_;}

	static EventPoller * create(); //  静态函数，创建EventPoller对象

	// 查找已注册的读事件处理器。
	InputNotificationHandler* findForRead(int fd);
	// 查找已注册的写事件处理器。
	OutputNotificationHandler* findForWrite(int fd);

protected:
	virtual bool doRegisterForRead(int fd) = 0; //  虚函数，注册用于读取的文件描述符
	virtual bool doRegisterForWrite(int fd) = 0; //  虚函数，注册用于写入的文件描述符

	virtual bool doDeregisterForRead(int fd) = 0; //  虚函数，注销用于读取的文件描述符
	virtual bool doDeregisterForWrite(int fd) = 0; //  虚函数，注销用于写入的文件描述符

	bool triggerRead(int fd); //  触发读取
	bool triggerWrite(int fd); //  触发写事件，参数为文件描述符fd
	bool triggerError(int fd); //  触发错误事件，参数为文件描述符fd
	
	 /**
     * @brief 检查文件描述符是否已注册。
     * 
     * @param fd 文件描述符。
     * @param isForRead 是否是读事件。
     * @return 已注册返回 true，否则返回 false。
     */
	bool isRegistered(int fd, bool isForRead) const;

	int maxFD() const; //  获取最大文件描述符

private:
	FDReadHandlers fdReadHandlers_;	// 存储读事件处理器的映射表。
	FDWriteHandlers fdWriteHandlers_;	// 存储写事件处理器的映射表。

protected:
	uint64 spareTime_;	// 空闲时间（毫秒）
};

}
}
#endif // KBE_EVENT_POLLER_H
