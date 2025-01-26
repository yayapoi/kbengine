// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_NETWORK_STATS_H
#define KBE_NETWORK_STATS_H

#include "network/interfaces.h"
#include "common/common.h"
#include "common/singleton.h"

namespace KBEngine { 
namespace Network
{

class MessageHandler;

/*
	��¼network��������Ϣ
	��һ�������࣬���ڸ���������Ϣ�ķ��ͺͽ���ͳ����Ϣ���������û���¼��Ϣ�ķ��ͺͽ��մ����Լ���С����֪ͨע��Ĵ�������
*/
class NetworkStats : public Singleton<NetworkStats>
{
public:
	// ��������ö�٣��������ַ��ͺͽ��ղ���
	enum S_OP{
		SEND,	// ���Ͳ���
		RECV	// ���ղ���
	};

	// ͳ����Ϣ�ṹ��
	struct Stats
	{
		Stats()
		{
			name = "";
			send_count = 0;
			send_size = 0;
			recv_size = 0;
			recv_count = 0;
		}

		std::string name;	// ��Ϣ����
		uint32 send_size; //  ���͵����ֽ���
		uint32 send_count; //  ���ʹ���
		uint32 recv_size; //  ���յ����ֽ���
		uint32 recv_count; //  ���մ���
	};

	typedef KBEUnordered_map<std::string, Stats> STATS; //  ����һ��unordered_map�����ڴ洢��Ϣ���ƺͶ�Ӧ��ͳ����Ϣ

	NetworkStats();
	~NetworkStats();

	void trackMessage(S_OP op, const MessageHandler& msgHandler, uint32 size);	// ������Ϣ�ķ��ͻ����

	NetworkStats::STATS& stats(){ return stats_; }	// ��ȡͳ����Ϣ

	void addHandler(NetworkStatsHandler* pHandler); //  ��Ӵ�����
	void removeHandler(NetworkStatsHandler* pHandler); //  �Ƴ�������

private:
	STATS stats_; //  ͳ����Ϣ

	std::vector<NetworkStatsHandler*> handlers_; //  �������б�
};

}
}
#endif // KBE_NETWORK_STATS_H
