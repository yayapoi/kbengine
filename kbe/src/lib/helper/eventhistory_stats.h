// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_EVENT_HISTORY_STATS_H
#define KBE_EVENT_HISTORY_STATS_H

#include "common/common.h"

namespace KBEngine { 

/*
	��¼event_history����
*/
class EventHistoryStats
{
public:

	struct Stats
	{
		Stats()
		{
			name = "";
			size = 0;
			count = 0;
		}

		std::string name;
		uint32 size;
		uint32 count;
	};

	/**
     * ͳ����Ϣ��ӳ������
     */
	typedef KBEUnordered_map<std::string, Stats> STATS;

	EventHistoryStats(std::string name);
	~EventHistoryStats();

	/**
     * �����¼�
     * 
     * @param type �¼�����
     * @param name �¼�����
     * @param size �¼���С���ֽڣ�
     * @param flags �¼���־��Ĭ��Ϊ"."��
     */
	void trackEvent(const std::string& type, const std::string& name, uint32 size, const char* flags = ".");

	/**
     * ��ȡͳ����Ϣ
     * 
     * @return ͳ����Ϣ������
     */
    EventHistoryStats::STATS& stats(){ return stats_; }

    /**
     * ��ȡͳ�ƶ��������
     * 
     * @return ͳ�ƶ��������
     */
    const char* name() const { return name_.c_str(); }

private:
    /**
     * ͳ����Ϣ��ӳ��
     */
    STATS stats_;

    /**
     * ͳ�ƶ��������
     */
    std::string name_;
};

}

#endif // KBE_EVENT_HISTORY_STATS_H
