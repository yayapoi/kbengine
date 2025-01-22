// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_TIMESTAMP_H
#define KBE_TIMESTAMP_H

#include "common/platform.h"

namespace KBEngine {

// 指示是否可以通过调用RDTSC（时间戳计数器）计算时间戳。
// 使用此方法的好处是，它能快速和精确地返回实际的时钟滴答。
// 不足之处是，这并不适用于使用SpeedStep技术来改变时钟速度的CPU。
#if KBE_PLATFORM == PLATFORM_UNIX
	//#define KBE_USE_RDTSC
#else // unix
	//#define KBE_USE_RDTSC
#endif // unix

// 定义时间戳计算方法的枚举
enum KBETimingMethod
{
	RDTSC_TIMING_METHOD, // 自CPU上电以来所经过的时钟周期数，达到纳秒级的计时精度
	GET_TIME_OF_DAY_TIMING_METHOD, // 使用gettimeofday函数获取时间戳
	GET_TIME_TIMING_METHOD, // 使用clock_gettime函数获取时间戳
	NO_TIMING_METHOD, // 无时间戳计算方法
};

// 全局变量，表示当前使用的时间戳计算方法
extern KBETimingMethod g_timingMethod;

// 获取当前时间戳计算方法的名称
const char* getTimingMethodName();

#if KBE_PLATFORM == PLATFORM_UNIX

// 使用RDTSC指令获取时间戳
inline uint64 timestamp_rdtsc()
{
	uint32 rethi, retlo;
	__asm__ __volatile__(
		"rdtsc":
		"=d" (rethi),
		"=a" (retlo)
	);
	return uint64(rethi) << 32 | retlo;
}


// 使用 gettimeofday. 测试大概比RDTSC20倍-600倍。
// 此外，有一个问题
// 2.4内核下，连续两次调用gettimeofday的可能
// 返回一个结果是倒着走。
#include <sys/time.h>
inline uint64 timestamp_gettimeofday()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return 1000000ULL * uint64(tv.tv_sec) + uint64(tv.tv_usec);
}

// 使用clock_gettime函数获取时间戳
#include <time.h>
#include <asm/unistd.h>
inline uint64 timestamp_gettime()
{
	timespec tv;
	assert(syscall(__NR_clock_gettime, CLOCK_MONOTONIC, &tv) == 0);
	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

// 根据当前配置选择合适的时间戳获取方法
inline uint64 timestamp()
{
#ifdef KBE_USE_RDTSC
	return timestamp_rdtsc();
#else // KBE_USE_RDTSC
	if (g_timingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if (g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else // GET_TIME_TIMING_METHOD
		return timestamp_gettime();
#endif // KBE_USE_RDTSC
}

#elif defined(_WIN32)

#ifdef KBE_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
// 使用RDTSC指令获取时间戳
inline uint64 timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // KBE_USE_RDTSC

#include <windows.h>

// 使用QueryPerformanceCounter函数获取时间戳
inline uint64 timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

#endif // KBE_USE_RDTSC

#else
#error Unsupported platform!
#endif

// 获取每秒的时间戳数量
uint64 stampsPerSecond();
double stampsPerSecondD();

// 获取RDTSC方法每秒的时间戳数量
uint64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

// 获取gettimeofday方法每秒的时间戳数量
uint64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

// 将时间戳转换为秒
inline double stampsToSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

// 时间戳类
class TimeStamp
{
public:
	// 构造函数，初始化时间戳
	TimeStamp(uint64 stamps = 0) :
		stamp_(stamps)
	{
	}

	// 隐式转换为uint64
	operator uint64& ()
	{
		return stamp_;
	}

	// 隐式转换为uint64
	operator uint64() const
	{
		return stamp_;
	}

	// 获取时间戳
	inline uint64 stamp() { return stamp_; }

	// 获取时间戳对应的秒数
	inline double inSeconds() const;

	// 设置时间戳为指定的秒数
	inline void setInSeconds(double seconds);

	// 将时间戳转换为秒
	inline static double toSeconds(uint64 stamps);

	// 从秒数创建时间戳
	inline static TimeStamp fromSeconds(double seconds);

	// 时间戳值
	uint64 stamp_;
};

// 将时间戳转换为秒
inline double TimeStamp::toSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

// 从秒数创建时间戳
inline TimeStamp TimeStamp::fromSeconds(double seconds)
{
	return uint64(seconds * stampsPerSecondD());
}

// 获取时间戳对应的秒数
inline double TimeStamp::inSeconds() const
{
	return toSeconds(stamp_);
}

// 设置时间戳为指定的秒数
inline void TimeStamp::setInSeconds(double seconds)
{
	stamp_ = fromSeconds(seconds);
}

}

#endif // KBE_TIMESTAMP_H