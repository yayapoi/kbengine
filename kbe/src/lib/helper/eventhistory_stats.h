// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_EVENT_HISTORY_STATS_H
#define KBE_EVENT_HISTORY_STATS_H

#include "common/common.h"

namespace KBEngine { 

/*
	记录event_history流量
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
     * 统计信息的映射类型
     */
	typedef KBEUnordered_map<std::string, Stats> STATS;

	EventHistoryStats(std::string name);
	~EventHistoryStats();

	/**
     * 跟踪事件
     * 
     * @param type 事件类型
     * @param name 事件名称
     * @param size 事件大小（字节）
     * @param flags 事件标志（默认为"."）
     */
	void trackEvent(const std::string& type, const std::string& name, uint32 size, const char* flags = ".");

	/**
     * 获取统计信息
     * 
     * @return 统计信息的引用
     */
    EventHistoryStats::STATS& stats(){ return stats_; }

    /**
     * 获取统计对象的名称
     * 
     * @return 统计对象的名称
     */
    const char* name() const { return name_.c_str(); }

private:
    /**
     * 统计信息的映射
     */
    STATS stats_;

    /**
     * 统计对象的名称
     */
    std::string name_;
};

}

#endif // KBE_EVENT_HISTORY_STATS_H
