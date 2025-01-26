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
	// ע���ļ��������Խ��ж�����
    virtual bool doRegisterForRead(int fd);
    // ע���ļ��������Խ���д����
    virtual bool doRegisterForWrite(int fd);

    // ȡ��ע���ļ��������Ķ�����
    virtual bool doDeregisterForRead(int fd);
    // ȡ��ע���ļ���������д����
    virtual bool doDeregisterForWrite(int fd);

    // �����������¼����ȴ�� maxWait ��
    virtual int processPendingEvents(double maxWait);

private:
	// ����֪ͨ�������Ѿ������ļ�����������
	void handleNotifications(int &countReady,
			fd_set &readFDs, fd_set &writeFDs);

	// ���ڴ洢���������ļ�����������
	fd_set						fdReadSet_;
	fd_set						fdWriteSet_;

	// ���ע���socket������ ������д��
	int							fdLargest_;

	// ע��д��socket����������
	int							fdWriteCount_;
};


#endif // HAS_EPOLL

}
}
#endif // KBE_SELECT_POLLER_H
