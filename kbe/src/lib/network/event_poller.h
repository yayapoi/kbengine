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

// ��һ���¼���ѯ�������ڹ����ļ��������Ķ�д�¼���������ע���ע���ļ��������Ķ�д�¼�����������Щ�¼�
class EventPoller
{
public:
	EventPoller();
	virtual ~EventPoller();

	/**
     * @brief ע���ļ��������Ķ��¼���
     * 
     * @param fd �ļ���������
     * @param handler ָ�� InputNotificationHandler �����ָ�룬���ڴ�����¼���
     * @return ע��ɹ����� true�����򷵻� false��
     */
	bool registerForRead(int fd, InputNotificationHandler * handler);
	bool registerForWrite(int fd, OutputNotificationHandler * handler);

	 /**
     * @brief ע���ļ��������Ķ��¼���
     * 
     * @param fd �ļ���������
     * @return ע���ɹ����� true�����򷵻� false��
     */
	bool deregisterForRead(int fd);
	bool deregisterForWrite(int fd);


	/**
     * @brief �����������¼���
     * 
     * @param maxWait ���ȴ�ʱ�䣨�룩��
     * @return ������¼�������
     */
	virtual int processPendingEvents(double maxWait) = 0;
	virtual int getFileDescriptor() const; //  �麯������ȡ�ļ�������

	// �������ʱ�䡣
	void clearSpareTime()		{spareTime_ = 0;}
	// ��ȡ����ʱ�䡣
	uint64 spareTime() const	{return spareTime_;}

	static EventPoller * create(); //  ��̬����������EventPoller����

	// ������ע��Ķ��¼���������
	InputNotificationHandler* findForRead(int fd);
	// ������ע���д�¼���������
	OutputNotificationHandler* findForWrite(int fd);

protected:
	virtual bool doRegisterForRead(int fd) = 0; //  �麯����ע�����ڶ�ȡ���ļ�������
	virtual bool doRegisterForWrite(int fd) = 0; //  �麯����ע������д����ļ�������

	virtual bool doDeregisterForRead(int fd) = 0; //  �麯����ע�����ڶ�ȡ���ļ�������
	virtual bool doDeregisterForWrite(int fd) = 0; //  �麯����ע������д����ļ�������

	bool triggerRead(int fd); //  ������ȡ
	bool triggerWrite(int fd); //  ����д�¼�������Ϊ�ļ�������fd
	bool triggerError(int fd); //  ���������¼�������Ϊ�ļ�������fd
	
	 /**
     * @brief ����ļ��������Ƿ���ע�ᡣ
     * 
     * @param fd �ļ���������
     * @param isForRead �Ƿ��Ƕ��¼���
     * @return ��ע�᷵�� true�����򷵻� false��
     */
	bool isRegistered(int fd, bool isForRead) const;

	int maxFD() const; //  ��ȡ����ļ�������

private:
	FDReadHandlers fdReadHandlers_;	// �洢���¼���������ӳ���
	FDWriteHandlers fdWriteHandlers_;	// �洢д�¼���������ӳ���

protected:
	uint64 spareTime_;	// ����ʱ�䣨���룩
};

}
}
#endif // KBE_EVENT_POLLER_H
