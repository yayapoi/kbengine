// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_TIMESTAMP_H
#define KBE_TIMESTAMP_H

#include "common/platform.h"

namespace KBEngine {

// ָʾ�Ƿ����ͨ������RDTSC��ʱ���������������ʱ�����
// ʹ�ô˷����ĺô��ǣ����ܿ��ٺ;�ȷ�ط���ʵ�ʵ�ʱ�ӵδ�
// ����֮���ǣ��Ⲣ��������ʹ��SpeedStep�������ı�ʱ���ٶȵ�CPU��
#if KBE_PLATFORM == PLATFORM_UNIX
	//#define KBE_USE_RDTSC
#else // unix
	//#define KBE_USE_RDTSC
#endif // unix

// ����ʱ������㷽����ö��
enum KBETimingMethod
{
	RDTSC_TIMING_METHOD, // ��CPU�ϵ�������������ʱ�����������ﵽ���뼶�ļ�ʱ����
	GET_TIME_OF_DAY_TIMING_METHOD, // ʹ��gettimeofday������ȡʱ���
	GET_TIME_TIMING_METHOD, // ʹ��clock_gettime������ȡʱ���
	NO_TIMING_METHOD, // ��ʱ������㷽��
};

// ȫ�ֱ�������ʾ��ǰʹ�õ�ʱ������㷽��
extern KBETimingMethod g_timingMethod;

// ��ȡ��ǰʱ������㷽��������
const char* getTimingMethodName();

#if KBE_PLATFORM == PLATFORM_UNIX

// ʹ��RDTSCָ���ȡʱ���
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


// ʹ�� gettimeofday. ���Դ�ű�RDTSC20��-600����
// ���⣬��һ������
// 2.4�ں��£��������ε���gettimeofday�Ŀ���
// ����һ������ǵ����ߡ�
#include <sys/time.h>
inline uint64 timestamp_gettimeofday()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return 1000000ULL * uint64(tv.tv_sec) + uint64(tv.tv_usec);
}

// ʹ��clock_gettime������ȡʱ���
#include <time.h>
#include <asm/unistd.h>
inline uint64 timestamp_gettime()
{
	timespec tv;
	assert(syscall(__NR_clock_gettime, CLOCK_MONOTONIC, &tv) == 0);
	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

// ���ݵ�ǰ����ѡ����ʵ�ʱ�����ȡ����
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
// ʹ��RDTSCָ���ȡʱ���
inline uint64 timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // KBE_USE_RDTSC

#include <windows.h>

// ʹ��QueryPerformanceCounter������ȡʱ���
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

// ��ȡÿ���ʱ�������
uint64 stampsPerSecond();
double stampsPerSecondD();

// ��ȡRDTSC����ÿ���ʱ�������
uint64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

// ��ȡgettimeofday����ÿ���ʱ�������
uint64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

// ��ʱ���ת��Ϊ��
inline double stampsToSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

// ʱ�����
class TimeStamp
{
public:
	// ���캯������ʼ��ʱ���
	TimeStamp(uint64 stamps = 0) :
		stamp_(stamps)
	{
	}

	// ��ʽת��Ϊuint64
	operator uint64& ()
	{
		return stamp_;
	}

	// ��ʽת��Ϊuint64
	operator uint64() const
	{
		return stamp_;
	}

	// ��ȡʱ���
	inline uint64 stamp() { return stamp_; }

	// ��ȡʱ�����Ӧ������
	inline double inSeconds() const;

	// ����ʱ���Ϊָ��������
	inline void setInSeconds(double seconds);

	// ��ʱ���ת��Ϊ��
	inline static double toSeconds(uint64 stamps);

	// ����������ʱ���
	inline static TimeStamp fromSeconds(double seconds);

	// ʱ���ֵ
	uint64 stamp_;
};

// ��ʱ���ת��Ϊ��
inline double TimeStamp::toSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

// ����������ʱ���
inline TimeStamp TimeStamp::fromSeconds(double seconds)
{
	return uint64(seconds * stampsPerSecondD());
}

// ��ȡʱ�����Ӧ������
inline double TimeStamp::inSeconds() const
{
	return toSeconds(stamp_);
}

// ����ʱ���Ϊָ��������
inline void TimeStamp::setInSeconds(double seconds)
{
	stamp_ = fromSeconds(seconds);
}

}

#endif // KBE_TIMESTAMP_H