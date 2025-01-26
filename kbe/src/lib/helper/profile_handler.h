// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_PROFILE_HANDLER_H
#define KBE_PROFILE_HANDLER_H

#include "common/common.h"
#include "common/tasks.h"
#include "common/timer.h"
#include "helper/debug_helper.h"
#include "helper/eventhistory_stats.h"
#include "network/interfaces.h"

namespace KBEngine { 
namespace Network
{
class NetworkInterface;
class Address;
class MessageHandler;
}

class MemoryStream;

// ��ʱ�������������ڴ�������ӿڵ����ܷ�����
class ProfileHandler : public TimerHandler
{
public:
	ProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~ProfileHandler();
	
	// ��ʱ����������Ҫ����ʵ��
	virtual void timeout() = 0;
	// ��������������Ҫ����ʵ��
	virtual void sendStream(MemoryStream* s) = 0;

	static KBEUnordered_map<std::string, KBEShared_ptr< ProfileHandler > > profiles;

protected:
	/**
     * @brief ����ʱ�¼�
     * @param handle ��ʱ�����
     * @param arg �û��Զ������
     */
    virtual void handleTimeout(TimerHandle handle, void * arg);

    /**
     * @brief ����ӿ�����
     */
    Network::NetworkInterface& networkInterface_;

    /**
     * @brief �������ƶ�ʱ�����
     */
    TimerHandle reportLimitTimerHandle_;
    
    /**
     * @brief ����������
     */
    std::string name_;
    
    /**
     * @brief ��ַ
     */
    Network::Address addr_;

    /**
     * @brief ��ʱ�����ʱ�䣨���룩
     */
    uint32 timinglen_;
};

// ��Ϊ����Ķ�ʱ���ܷ������������̳��� Task �� ProfileHandler��
class CProfileHandler : public Task, 
						public ProfileHandler
{
public:
	CProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~CProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);
	bool process();

private:
	struct ProfileVal
	{
		// ����
		std::string		name;

		// startd���ʱ��.
		TimeStamp		lastTime;
		TimeStamp		diff_lastTime;

		// count_�ε���ʱ��
		TimeStamp		sumTime;
		TimeStamp		diff_sumTime;

		// ��¼���һ���ڲ�ʱ��Ƭ
		TimeStamp		lastIntTime;
		TimeStamp		diff_lastIntTime;

		// count_���ڲ���ʱ��
		TimeStamp		sumIntTime;
		TimeStamp		diff_sumIntTime;

		uint32			count; //  ������
		uint32			diff_count; //  ��ֵ������
	};

	// ��ProfileValֻ�ڼ�ʱ����ʼʱ��¼default.profiles�ĳ�ʼֵ
	// �ڽ���ʱȡ����ֵ�õ����
	typedef KBEUnordered_map<std::string,  ProfileVal> PROFILEVALS;
	PROFILEVALS profileVals_;
};

class EventProfileHandler : public ProfileHandler
{
public:
	EventProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~EventProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);

	// �����¼�ʱ�Ĵ�����
	void onTriggerEvent(const EventHistoryStats& eventHistory, const EventHistoryStats::Stats& stats, 
		uint32 size);

	// �����¼��ľ�̬����
	static void triggerEvent(const EventHistoryStats& eventHistory, const EventHistoryStats::Stats& stats, 
		uint32 size);

private:
	//�洢���ܷ���ֵ�Ľṹ��
	struct ProfileVal
	{
		ProfileVal()
		{
			name = "";
			size = 0;
			count = 0;
		}

		// ����
		std::string		name;

		uint32			size;
		uint32			count;
	};

	// ��ProfileValֻ�ڼ�ʱ����ʼʱ��¼default.profiles�ĳ�ʼֵ
	// �ڽ���ʱȡ����ֵ�õ����
	typedef KBEUnordered_map<std::string,  ProfileVal> PROFILEVALS;

	typedef KBEUnordered_map< std::string,  PROFILEVALS > PROFILEVALMAP;
	PROFILEVALMAP profileMaps_;
	
	static std::vector<EventProfileHandler*> eventProfileHandlers_; //  �¼���������б�
	int removeHandle_;
};

// ����ͳ�ƴ�����
class NetworkProfileHandler : public ProfileHandler, public Network::NetworkStatsHandler
{
public:
	NetworkProfileHandler(Network::NetworkInterface & networkInterface, uint32 timinglen, 
		std::string name, const Network::Address& addr);
	virtual ~NetworkProfileHandler();
	
	void timeout();
	void sendStream(MemoryStream* s);

	virtual void onSendMessage(const Network::MessageHandler& msgHandler, int size); //  ������Ϣʱ�Ĵ�����
	virtual void onRecvMessage(const Network::MessageHandler& msgHandler, int size); //  ������Ϣʱ�Ĵ�����

private:
	struct ProfileVal
	{
		ProfileVal()
		{
			name = ""; //  ��ʼ��nameΪ���ַ���
			send_size = 0; //  ��ʼ�����ʹ�СΪ0
			send_count = 0; //  ��ʼ�����ʹ���Ϊ0
			send_avgsize = 0; //  ��ʼ��ƽ�����ʹ�СΪ0
			total_send_size = 0; //  ��ʼ���ܷ��ʹ�СΪ0
			total_send_count = 0; //  ��ʼ���ܷ��ʹ���Ϊ0

			recv_size = 0;
			recv_count = 0;
			recv_avgsize = 0;
			total_recv_size = 0;
			total_recv_count = 0;
		}

		// ����
		std::string		name;

		uint32			send_size; //  �������ݴ�С
		uint32			send_avgsize; //  ��������ƽ����С
		uint32			send_count; //  �������ݴ���

		uint32			total_send_size; //  �ܷ������ݴ�С
		uint32			total_send_count; //  �ܷ������ݴ���

		uint32			recv_size; //  �������ݴ�С
		uint32			recv_count; //  �������ݴ���
		uint32			recv_avgsize; //  ��������ƽ����С

		uint32			total_recv_size; //  �ܽ������ݴ�С
		uint32			total_recv_count; //  �ܽ������ݴ���
	};

	typedef KBEUnordered_map<std::string,  ProfileVal> PROFILEVALS;
	PROFILEVALS profileVals_;
};

}

#endif
