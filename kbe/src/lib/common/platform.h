// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_PLATFORM_H
#define KBE_PLATFORM_H

// common include	
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> 
#include <math.h>
#include <assert.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>  
#include <cstring>  
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <limits>
#include <algorithm>
#include <utility>
#include <functional>
#include <cctype>
#include <iterator>
#include "common/strutil.h"
// windows include	
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4049)
#pragma warning(disable:4217)
#include <io.h>
#include <time.h> 
#include <chrono>
//#define FD_SETSIZE 1024
#ifndef WIN32_LEAN_AND_MEAN 
#include <winsock2.h>		// 必须在windows.h之前包含， 否则网络模块编译会出错
#include <mswsock.h> 
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> 
#include <unordered_map>
#include <functional>
#include <memory>
#define _SCL_SECURE_NO_WARNINGS
#else
// linux include
#include <errno.h>
#include <float.h>
#include <pthread.h>	
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <iconv.h>
#include <langinfo.h>   /* CODESET */
#include <stdint.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <tr1/unordered_map>
#include <tr1/functional>
#include <tr1/memory>
#include <linux/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/resource.h> 
#include <linux/errqueue.h>
#endif

#include <signal.h>

/** 通过条件编译确保在不同的操作系统上正确地定义信号常量。在非 Windows 系统中，它包含 pwd.h 头文件；
在 Windows 系统中，它手动定义了一些常见的信号常量，以确保代码在不同平台上的一致性和可移植性。*/
#if !defined( _WIN32 )
# include <pwd.h>	//这个头文件定义了与用户账户信息相关的函数和结构体。
#else
#define SIGHUP	1	//通常表示挂起信号，当终端连接断开时发送此信号
#define SIGINT	2	//通常表示中断信号，当用户按下 Ctrl+C 时发送此信号。
#define SIGQUIT 3	//通常表示退出信号，当用户按下 Ctrl+\ 时发送此信号。
#define SIGUSR1 10	//是用户自定义信号 1
#define SIGPIPE 13	//通常表示管道破裂信号，当向已关闭的管道写入数据时发送此信号
#define SIGCHLD 17	//通常表示子进程终止或停止信号
#define SIGSYS	32	//通常表示系统调用错误信号
#endif

/** 定义引擎名字空间 */
namespace KBEngine
{ 

/** 定义引擎字节序 */
#define KBENGINE_LITTLE_ENDIAN							0
#define KBENGINE_BIG_ENDIAN								1
#if !defined(KBENGINE_ENDIAN)
#  if defined (USE_BIG_ENDIAN)
#    define KBENGINE_ENDIAN KBENGINE_BIG_ENDIAN
#  else 
#    define KBENGINE_ENDIAN KBENGINE_LITTLE_ENDIAN
#  endif 
#endif


// current platform and compiler
#define PLATFORM_WIN32			0
#define PLATFORM_UNIX			1
#define PLATFORM_APPLE			2

#define UNIX_FLAVOUR_LINUX		1	//Linux 系统
#define UNIX_FLAVOUR_BSD		2	//BSD 是另一类 Unix 操作系统，包括 FreeBSD、OpenBSD 和 NetBSD 等变种。
#define UNIX_FLAVOUR_OTHER		3	//其他类型的 Unix 系统。这可能包括 Solaris、AIX、HP-UX 等商业 Unix 系统
#define UNIX_FLAVOUR_OSX		4	//表示 macOS（以前称为 OS X）

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define KBE_PLATFORM PLATFORM_WIN32
#elif defined( __INTEL_COMPILER )
#  define KBE_PLATFORM PLATFORM_INTEL
#elif defined( __APPLE_CC__ )
#  define KBE_PLATFORM PLATFORM_APPLE
#else
#  define KBE_PLATFORM PLATFORM_UNIX
#endif

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU	   1
#define COMPILER_BORLAND   2
#define COMPILER_INTEL     3
#define COMPILER_CLANG     4

#ifdef _MSC_VER
#  define KBE_COMPILER COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
#  define KBE_COMPILER COMPILER_INTEL
#elif defined( __BORLANDC__ )
#  define KBE_COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define KBE_COMPILER COMPILER_GNU
#elif defined( __clang__ )
#  define KBE_COMPILER COMPILER_CLANG
	
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if KBE_PLATFORM == PLATFORM_UNIX || KBE_PLATFORM == PLATFORM_APPLE
#ifdef HAVE_DARWIN
#define KBE_PLATFORM_TEXT "MacOSX"
#define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#else
#ifdef USE_KQUEUE
#define KBE_PLATFORM_TEXT "FreeBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#ifdef USE_KQUEUE_DFLY
#define KBE_PLATFORM_TEXT "DragonFlyBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#define KBE_PLATFORM_TEXT "Linux"
#define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#endif
#endif
#endif
#endif

#if KBE_PLATFORM == PLATFORM_WIN32
#define KBE_PLATFORM_TEXT "Win32"
#endif

#ifndef KBE_CONFIG
#ifdef _DEBUG
#define KBE_CONFIG "Debug"
#else
#define KBE_CONFIG "Release"
#endif
#endif

#ifndef X64
#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( __amd64 ) || defined( __LP64__ )
#define X64
#endif
#endif

#ifdef X64
#define KBE_ARCH "X64"
#else
#define KBE_ARCH "X86"
#endif

/*---------------------------------------------------------------------------------
	类型定义
---------------------------------------------------------------------------------*/
#ifndef TCHAR
#ifdef _UNICODE
	typedef wchar_t												TCHAR;
#else
	typedef char												TCHAR;
#endif
#endif

typedef unsigned char											uchar;
typedef unsigned short											ushort;
typedef unsigned int											uint;
typedef unsigned long											ulong;

#define charptr													char*
#define const_charptr											const char*
#define PyObject_ptr											PyObject*

#define KBEShared_ptr											std::tr1::shared_ptr
#define KBEUnordered_map										std::tr1::unordered_map

/* Use correct types for x64 platforms, too */
#if KBE_COMPILER != COMPILER_GNU
typedef signed __int64											int64;
typedef signed __int32											int32;
typedef signed __int16											int16;
typedef signed __int8											int8;
typedef unsigned __int64										uint64;
typedef unsigned __int32										uint32;
typedef unsigned __int16										uint16;
typedef unsigned __int8											uint8;
typedef INT_PTR													intptr;
typedef UINT_PTR        										uintptr;
#define PRI64													"lld"
#define PRIu64													"llu"
#define PRIx64													"llx"
#define PRIX64													"llX"
#define PRIzu													"lu"
#define PRIzd													"ld"
#define PRTime													PRI64
#else
typedef int64_t													int64;
typedef int32_t													int32;
typedef int16_t													int16;
typedef int8_t													int8;
typedef uint64_t												uint64;
typedef uint32_t												uint32;
typedef uint16_t												uint16;
typedef uint8_t													uint8;
typedef uint16_t												WORD;
typedef uint32_t												DWORD;

#ifdef _LP64
typedef int64													intptr;
typedef uint64													uintptr;

#ifndef PRI64
#define PRI64													"ld"
#endif

#ifndef PRIu64
#define PRIu64													"lu"
#endif

#ifndef PRIx64
#define PRIx64													"lx"
#endif

#ifndef PRIX64
#define PRIX64													"lX"
#endif

#ifndef PRTime
#define PRTime													PRI64
#endif

#else
typedef int32													intptr;
typedef uint32													uintptr;

#ifndef PRI64
#define PRI64													"lld"
#endif

#ifndef PRIu64
#define PRIu64													"llu"
#endif

#ifndef PRIx64
#define PRIx64													"llx"
#endif

#ifndef PRIX64
#define PRIX64													"llX"
#endif

#ifndef PRTime
#define PRTime													"ld"
#endif

#endif

#ifndef PRIzd
#define PRIzd													"zd"
#endif

#ifndef PRIzu
#define PRIzu													"zu"
#endif

#endif

#define PRAppID													PRIu64
#define PRDBID													PRIu64

typedef uint16													ENTITY_TYPE;											// entity的类别类型定义支持0-65535个类别
typedef int32													ENTITY_ID;												// entityID的类型
typedef uint32													SPACE_ID;												// 一个space的id
typedef uint32													CALLBACK_ID;											// 一个callback由CallbackMgr分配的id
typedef uint64													COMPONENT_ID;											// 一个服务器组件的id
typedef int32													COMPONENT_ORDER;										// 一个组件的启动顺序
typedef int32													COMPONENT_GUS;											// 一个组件的genuuid_sections产生随机数的区间段
typedef	uint32													TIMER_ID;												// 一个timer的id类型
typedef uint8													ENTITYCALL_CALL_TYPE;									// entityCall 所投递的call类别的类别
typedef uint32													GAME_TIME;
typedef uint32													GameTime;
typedef int32													ScriptID;
typedef uint32													ArraySize;												// 任何数组的大小都用这个描述
typedef uint64													DBID;													// 一个在数据库中的索引用来当做某ID
typedef uint32													CELL_ID;
typedef KBEUnordered_map< std::string, std::string >			SPACE_DATA;												// space中存储的数据

#if KBE_PLATFORM == PLATFORM_WIN32
	#define IFNAMSIZ											16
	typedef UINT_PTR											KBESOCKET;
#ifndef socklen_t
	typedef	int													socklen_t;
#endif
	typedef unsigned short										u_int16_t;
	typedef unsigned long										u_int32_t;
	
#ifndef IFF_UP
	enum
	{
		IFF_UP													= 0x1,	//表示网络接口已启用。如果这个标志被设置，表示该接口是活动的
		IFF_BROADCAST											= 0x2,	//表示网络接口支持广播地址。广播地址用于向网络中的所有设备发送数据
		IFF_DEBUG												= 0x4,	//表示网络接口处于调试模式。这通常用于开发和测试过程中，以便捕获和分析网络流量
		IFF_LOOPBACK											= 0x8,	//表示网络接口是回环接口（loopback interface）。回环接口用于在本地主机上进行网络通信，例如 lo 或 localhost
		IFF_POINTOPOINT											= 0x10,	//表示网络接口是一个点对点连接。这种连接通常用于直接连接两台设备
		IFF_NOTRAILERS											= 0x20,	//表示网络接口不使用拖车（trailers）。拖车是一种在网络数据包末尾添加额外信息的机制，主要用于优化某些类型的网络传输
		IFF_RUNNING												= 0x40,	//表示网络接口正在运行。这个标志通常由内核设置，表示接口已经成功初始化并可以正常工作
		IFF_NOARP												= 0x80,	//表示网络接口不使用 ARP（Address Resolution Protocol）。ARP 用于将 IP 地址解析为物理地址（如 MAC 地址）
		IFF_PROMISC												= 0x100,//表示网络接口处于混杂模式（promiscuous mode）。在这种模式下，接口会接收所有通过网络的数据包，而不仅仅是发往其自身的数据包。这通常用于网络监控和抓包工具
		IFF_MULTICAST											= 0x1000//表示网络接口支持多播（multicast）。多播允许将数据包发送到多个目标地址，而不是单个地址或所有地址
	};
#endif
#else
	typedef int													KBESOCKET;
#endif

/*---------------------------------------------------------------------------------
	定会多种平台上的多线程相关
---------------------------------------------------------------------------------*/
#if KBE_PLATFORM == PLATFORM_WIN32
	#define THREAD_ID											HANDLE										//在 Windows 中，线程标识符是一个 HANDLE 类型。HANDLE 是一个指向内核对象的句柄
	#define THREAD_SINGNAL										HANDLE										//在 Windows 中，线程信号量（用于线程间的同步）是一个 HANDLE 类型。通常使用事件对象（CreateEvent）来实现
	#define THREAD_SINGNAL_INIT(x)								x = CreateEvent(NULL, TRUE, FALSE, NULL)	//初始化一个事件对象。CreateEvent 函数创建一个事件对象，参数分别为：安全属性（NULL 表示默认）、是否手动重置（TRUE 表示手动重置）、初始状态（FALSE 表示未触发）、名称（NULL 表示无名）
	#define THREAD_SINGNAL_DELETE(x)							CloseHandle(x)								//关闭事件对象。CloseHandle 函数关闭一个内核对象的句柄
	#define THREAD_SINGNAL_SET(x)								SetEvent(x)									//设置事件对象的状态为已触发。SetEvent 函数将事件对象的状态设置为已触发，从而唤醒等待该事件的线程
	#define THREAD_MUTEX										CRITICAL_SECTION							//在 Windows 中，互斥锁是一个 CRITICAL_SECTION 类型。CRITICAL_SECTION 是一种轻量级的同步机制，用于保护临界区
	#define THREAD_MUTEX_INIT(x)								InitializeCriticalSection(&x)				//初始化一个临界区。InitializeCriticalSection 函数初始化一个 CRITICAL_SECTION 结构
	#define THREAD_MUTEX_DELETE(x)								DeleteCriticalSection(&x)					//删除一个临界区。DeleteCriticalSection 函数删除一个 CRITICAL_SECTION 结构
	#define THREAD_MUTEX_LOCK(x)								EnterCriticalSection(&x)					//锁定一个临界区。EnterCriticalSection 函数进入一个 CRITICAL_SECTION，确保只有一个线程可以访问临界区
	#define THREAD_MUTEX_UNLOCK(x)								LeaveCriticalSection(&x)					//解锁一个临界区。LeaveCriticalSection 函数离开一个 CRITICAL_SECTION，允许其他线程进入临界区
#else
	#define THREAD_ID											pthread_t									//在类 Unix 系统中，线程标识符是一个 pthread_t 类型。pthread_t 是 POSIX 线程库中定义的线程标识符类型
	#define THREAD_SINGNAL										pthread_cond_t								//在类 Unix 系统中，线程信号量是一个 pthread_cond_t 类型。pthread_cond_t 是 POSIX 线程库中定义的条件变量类型
	#define THREAD_SINGNAL_INIT(x)								pthread_cond_init(&x, NULL)					//初始化一个条件变量。pthread_cond_init 函数初始化一个 pthread_cond_t 结构，参数分别为：条件变量指针、属性（NULL 表示默认属性）
	#define THREAD_SINGNAL_DELETE(x)							pthread_cond_destroy(&x)					//销毁一个条件变量。pthread_cond_destroy 函数销毁一个 pthread_cond_t 结构
	#define THREAD_SINGNAL_SET(x)								pthread_cond_signal(&x);					//信号化一个条件变量。pthread_cond_signal 函数唤醒一个等待该条件变量的线程
	#define THREAD_MUTEX										pthread_mutex_t								//在类 Unix 系统中，互斥锁是一个 pthread_mutex_t 类型。pthread_mutex_t 是 POSIX 线程库中定义的互斥锁类型
	#define THREAD_MUTEX_INIT(x)								pthread_mutex_init (&x, NULL)				//初始化一个互斥锁。pthread_mutex_init 函数初始化一个 pthread_mutex_t 结构，参数分别为：互斥锁指针、属性（NULL 表示默认属性）
	#define THREAD_MUTEX_DELETE(x)								pthread_mutex_destroy(&x)					//销毁一个互斥锁。pthread_mutex_destroy 函数销毁一个 pthread_mutex_t 结构
	#define THREAD_MUTEX_LOCK(x)								pthread_mutex_lock(&x)						//锁定一个互斥锁。pthread_mutex_lock 函数锁定一个 pthread_mutex_t，确保只有一个线程可以访问临界区
	#define THREAD_MUTEX_UNLOCK(x)								pthread_mutex_unlock(&x)					//解锁一个互斥锁。pthread_mutex_unlock 函数解锁一个 pthread_mutex_t，允许其他线程进入临界区
#endif

/*---------------------------------------------------------------------------------
	跨平台宏定义
---------------------------------------------------------------------------------*/
#if 0
#define ARRAYCLR(v)					memset((v), 0x0, sizeof(v))
#define MEMCLR(v)					memset(&(v), 0x0, sizeof(v))
#define MEMCLRP(v)					memset((v), 0x0, sizeof(*v))
#endif

#define ARRAYSZ(v)					(sizeof(v) / sizeof(v[0]))
#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

#if 0
#define offsetof(type, field)		((uint32)&(((type *)NULL)->field))
#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)	offsetof(type, field)
#endif
#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field)		(sizeof(((type *)NULL)->field))
#endif
#endif

#define KBE_LITTLE_ENDIAN	//当前系统使用小端字节序
/*#define KBE_BIG_ENDIAN*/

#ifdef KBE_LITTLE_ENDIAN	//当前系统使用小端字节序
/* accessing individual bytes (int8) and words (int16) within
 * words and long words (int32).
 * Macros ending with W deal with words, L macros deal with longs
 * 用于在小端字节序（Little Endian）的系统中处理字节和字（16位）以及长字（32位）的数据
 */
 /// 返回一个word(16位字)的高字节,提取高8位并右移8位
#define HIBYTEW(b)		(((b) & 0xff00) >> 8)
/// 返回一个word(16位字)的低字节.提取低8位
#define LOBYTEW(b)		( (b) & 0xff)

/// 返回一个long(32位长字)的最高字节.提取最高8位并右移24位
#define HIBYTEL(b)		(((b) & 0xff000000L) >> 24)
/// 返回一个long(32位长字)的最低字节.提取最低8位
#define LOBYTEL(b)		( (b) & 0xffL)

/// long的第0字节（最低字节）.提取最低8位
#define BYTE0L(b)		( (b) & 0xffL)
/// Returns byte 1 of a long.提取第1字节并右移8位
#define BYTE1L(b)		(((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)		(((b) & 0xff0000L) >> 16)
/// long的第3字节（最高字节.
#define BYTE3L(b)		(((b) & 0xff000000L) >> 24)

/// 返回一个long(32位长字)的高16位字.提取高16位并右移16位
#define HIWORDL(b)		(((b) & 0xffff0000L) >> 16)
/// 返回一个long(32位长字)的低16位字.
#define LOWORDL(b)		( (b) & 0xffffL)

/**
 *	This macro takes a dword ordered 0123 and reorder it to 3210.
 * 将一个32位的双字（dword）从0123的顺序重新排列为3210的顺序。具体来说，这个宏通过位操作将每个字节的位置交换
 */
#define SWAP_DW(a)	  ( (((a) & 0xff000000)>>24) |	\
						(((a) & 0xff0000)>>8) |		\
						(((a) & 0xff00)<<8) |		\
						(((a) & 0xff)<<24) )

#else
/* big endian macros go here */
#endif

#if defined(_WIN32)

#undef min
#define min min
#undef max
#define max max

template <class T>
inline const T & min( const T & a, const T & b )
{
	return b < a ? b : a;
}

template <class T>
inline const T & max( const T & a, const T & b )
{
	return a < b ? b : a;
}

#define KBE_MIN min
#define KBE_MAX max

#define NOMINMAX

#else

#define KBE_MIN std::min
#define KBE_MAX std::max

#endif

// 所有名称字符串的最大长度
#define MAX_NAME 256	

// ip字符串的最大长度
#define MAX_IP 256

// 常规的buf长度
#define MAX_BUF 256

// 常规的buf长度
#define SQL_BUF 65535

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// 获得系统产生的最后一次错误描述
inline char* kbe_strerror(int ierrorno = 0)
{
#if KBE_PLATFORM == PLATFORM_WIN32
	if(ierrorno == 0)
		ierrorno = GetLastError();

	static char lpMsgBuf[256] = {0};
	
	/*
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ierrorno,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		1024,
		NULL
	); 
	*/
	kbe_snprintf(lpMsgBuf, 256, "errorno=%d",  ierrorno);
	return lpMsgBuf;
#else
	if(ierrorno != 0)
		return strerror(ierrorno);
	return strerror(errno);
#endif
}

inline int kbe_lasterror()
{
#if KBE_PLATFORM == PLATFORM_WIN32
	return GetLastError();
#else
	return errno;
#endif
}

/** 获取用户UID */
inline int32 getUserUID()
{
	static int32 iuid = 0;

	if(iuid == 0)
	{
#if KBE_PLATFORM == PLATFORM_WIN32
		// VS2005:
		#if _MSC_VER >= 1400
			char uid[16];
			size_t sz;
			iuid = getenv_s( &sz, uid, sizeof( uid ), "UID" ) == 0 ? atoi( uid ) : 0;	// 使用getenv_s函数获取环境变量"UID"的值，并将其转换为整数赋值给iuid。如果获取失败，iuid保持为0

		// VS2003:
		#elif _MSC_VER < 1400
			char * uid = getenv( "UID" );
			iuid = uid ? atoi( uid ) : 0;	 // 如果获取成功，将其转换为整数赋值给iuid；否则iuid保持为0
		#endif
#else
	// Linux:
		char * uid = getenv( "UID" );
		iuid = uid ? atoi( uid ) : getuid();	// 如果获取成功，将其转换为整数赋值给iuid；否则调用getuid函数获取当前用户的UID

		char * uuid = getenv("UUID");
		iuid = uuid ? atoi( uuid ) : iuid;	 // 如果获取成功，将其转换为整数赋值给iuid；否则iuid保持不变
#endif
	}

	return iuid;
}

/** 获取用户名 */
inline const char * getUsername()
{
#if KBE_PLATFORM == PLATFORM_WIN32
	DWORD dwSize = MAX_NAME;
	wchar_t wusername[MAX_NAME];
	::GetUserNameW(wusername, &dwSize);

	static char username[MAX_NAME];
	memset(username, 0, MAX_NAME);

	if (dwSize > 0)
	{
		size_t outsize = 0;

		char* ptest = strutil::wchar2char((wchar_t*)&wusername, &outsize);

		if (outsize == 0)
		{
			// 可能是中文名，不支持中文名称
			strcpy(username, "error_name");
		}
		else
		{
			if(ptest)
				kbe_snprintf(username, MAX_NAME, "%s", ptest);
		}

		if (ptest)
			free(ptest);
	}

	return username;
#else
	char * pUsername = cuserid(NULL);
	return pUsername ? pUsername : "";
#endif
}

/** 获取进程ID */
inline int32 getProcessPID()
{
#if KBE_PLATFORM != PLATFORM_WIN32
	return getpid();
#else
	return (int32) GetCurrentProcessId();
#endif
}

/** 获取系统时间(精确到毫秒) */
#if KBE_PLATFORM == PLATFORM_WIN32
	inline uint32 getSystemTime() 
	{ 
		// 注意这个函数windows上只能正确维持49天。
		return ::GetTickCount(); 
	};
#else
	inline uint32 getSystemTime()
	{
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	};
#endif

/** 获取2个系统时间差 */
inline uint32 getSystemTimeDiff(uint32 oldTime, uint32 newTime)
{
    // 防止getSystemTime()溢出的情况
    if (oldTime > newTime)
    {
        return (uint32)((int64)0xFFFFFFFF + 1 - (int64)oldTime) + newTime;
    }

	return newTime - oldTime;
}

/* get system time */
inline void kbe_timeofday(long *sec, long *usec)
{
#if defined(__unix)
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
#else
	static long mode = 0, addsec = 0;
	BOOL retval;
	static int64 freq = 1;
	int64 qpc;
	if (mode == 0) {
		retval = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		freq = (freq == 0) ? 1 : freq;
		retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
		addsec = (long)time(NULL);
		addsec = addsec - (long)((qpc / freq) & 0x7fffffff);
		mode = 1;
	}
	retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	retval = retval * 2;
	if (sec) *sec = (long)(qpc / freq) + addsec;
	if (usec) *usec = (long)((qpc % freq) * 1000000 / freq);
#endif
}

/* get clock in millisecond 64 */
inline int64 kbe_clock64(void)
{
	long s, u;
	int64 value;
	kbe_timeofday(&s, &u);
	value = ((int64)s) * 1000 + (u / 1000);
	return value;
}

inline uint32 kbe_clock()
{
	return (uint32)(kbe_clock64() & 0xfffffffful);
}

/* get time in millisecond 64 */
inline uint64 getTimeMs()
{
#if KBE_PLATFORM == PLATFORM_WIN32
	auto timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return timeNow.count();
#else
	timeval time;
	gettimeofday(&time, NULL);
	return (uint64)((time.tv_sec * 1000) + (time.tv_usec / 1000));
#endif
}

/* 产生一个64位的uuid 
*/
extern COMPONENT_ORDER g_componentGlobalOrder;
extern COMPONENT_ORDER g_componentGroupOrder;

extern COMPONENT_GUS g_genuuid_sections;

inline uint64 genUUID64()
{
	static uint64 tv = (uint64)(time(NULL));
	uint64 now = (uint64)(time(NULL));		// 获取当前时间戳

	static uint16 lastNum = 0;	// 用于存储迭代数，初始值为0

	if(now != tv)		// 如果当前时间戳与上次不同
	{
		tv = now;		// 更新tv为当前时间戳
		lastNum = 0;	// 重置迭代数为0
	}
	
	if(g_genuuid_sections <= 0)
	{
		// 时间戳32位，随机数16位，16位迭代数（最大为65535-1）
		static uint32 rnd = 0;
		if(rnd == 0)			// 如果随机数还没有生成
		{
			srand(getSystemTime());			// 使用系统时间初始化随机数生成器
			rnd = (uint32)(rand() << 16);	// 生成一个16位的随机数
		}
		
		assert(lastNum < 65535 && "genUUID64(): overflow!");	// 断言迭代数没有超过最大值65535
		
		return (tv << 32) | rnd | lastNum++;		// 返回64位的UUID，由时间戳、随机数和迭代数组成
	}
	else
	{
		// 时间戳32位，app组ID16位，16位迭代数（最大为65535-1）
		static uint32 sections = g_genuuid_sections << 16;
		
		assert(lastNum < 65535 && "genUUID64(): overflow!");
		
		return (tv << 32) | sections | lastNum++;		// 返回64位的UUID，由时间戳、app组ID和迭代数组成
	}
}

/** sleep 跨平台 */
#if KBE_PLATFORM == PLATFORM_WIN32
	inline void sleep(uint32 ms)
	{ 
		::Sleep(ms); 
	}
#else
	inline void sleep(uint32 ms)
	{ 
	  struct timeval tval;
	  tval.tv_sec	= ms / 1000;
	  tval.tv_usec	= ( ms * 1000) % 1000000;
	  select(0, NULL, NULL, NULL, &tval);
	}	
#endif

/** 判断平台是否为小端字节序 */
inline bool isPlatformLittleEndian()
{
   int n = 1;
   return *((char*)&n) ? true : false;
}

/** 设置环境变量 */
#if KBE_PLATFORM == PLATFORM_WIN32
	inline void setenv(const std::string& name, const std::string& value, int overwrite)
	{
		_putenv_s(name.c_str(), value.c_str());
	}
#else
	// Linux下面直接使用setenv
#endif

}
#endif // KBE_PLATFORM_H
