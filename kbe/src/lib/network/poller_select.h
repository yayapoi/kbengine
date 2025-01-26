// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_SELECT_POLLER_H
#define KBE_SELECT_POLLER_H

#include "event_poller.h"

namespace KBEngine { 
namespace Network
{

#ifndef HAS_EPOLL

class SelectPoller : public EventPoller
{
public:
	SelectPoller();

protected:
	// 注册文件描述符以进行读操作
    virtual bool doRegisterForRead(int fd);
    // 注册文件描述符以进行写操作
    virtual bool doRegisterForWrite(int fd);

    // 取消注册文件描述符的读操作
    virtual bool doDeregisterForRead(int fd);
    // 取消注册文件描述符的写操作
    virtual bool doDeregisterForWrite(int fd);

    // 处理待处理的事件，等待最长 maxWait 秒
    virtual int processPendingEvents(double maxWait);

private:
	// 处理通知，更新已就绪的文件描述符数量
	void handleNotifications(int &countReady,
			fd_set &readFDs, fd_set &writeFDs);

	// 用于存储读操作的文件描述符集合
	fd_set						fdReadSet_;
	fd_set						fdWriteSet_;

	// 最后注册的socket描述符 （读或写）
	int							fdLargest_;

	// 注册写的socket描述符数量
	int							fdWriteCount_;
};


#endif // HAS_EPOLL

}
}
#endif // KBE_SELECT_POLLER_H
