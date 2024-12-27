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
#include <winsock2.h>		// ������windows.h֮ǰ������ ��������ģ���������
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

/** ͨ����������ȷ���ڲ�ͬ�Ĳ���ϵͳ����ȷ�ض����źų������ڷ� Windows ϵͳ�У������� pwd.h ͷ�ļ���
�� Windows ϵͳ�У����ֶ�������һЩ�������źų�������ȷ�������ڲ�ͬƽ̨�ϵ�һ���ԺͿ���ֲ�ԡ�*/
#if !defined( _WIN32 )
# include <pwd.h>	//���ͷ�ļ����������û��˻���Ϣ��صĺ����ͽṹ�塣
#else
#define SIGHUP	1	//ͨ����ʾ�����źţ����ն����ӶϿ�ʱ���ʹ��ź�
#define SIGINT	2	//ͨ����ʾ�ж��źţ����û����� Ctrl+C ʱ���ʹ��źš�
#define SIGQUIT 3	//ͨ����ʾ�˳��źţ����û����� Ctrl+\ ʱ���ʹ��źš�
#define SIGUSR1 10	//���û��Զ����ź� 1
#define SIGPIPE 13	//ͨ����ʾ�ܵ������źţ������ѹرյĹܵ�д������ʱ���ʹ��ź�
#define SIGCHLD 17	//ͨ����ʾ�ӽ�����ֹ��ֹͣ�ź�
#define SIGSYS	32	//ͨ����ʾϵͳ���ô����ź�
#endif

/** �����������ֿռ� */
namespace KBEngine
{ 

/** ���������ֽ��� */
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

#define UNIX_FLAVOUR_LINUX		1	//Linux ϵͳ
#define UNIX_FLAVOUR_BSD		2	//BSD ����һ�� Unix ����ϵͳ������ FreeBSD��OpenBSD �� NetBSD �ȱ��֡�
#define UNIX_FLAVOUR_OTHER		3	//�������͵� Unix ϵͳ������ܰ��� Solaris��AIX��HP-UX ����ҵ Unix ϵͳ
#define UNIX_FLAVOUR_OSX		4	//��ʾ macOS����ǰ��Ϊ OS X��

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
	���Ͷ���
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

typedef uint16													ENTITY_TYPE;											// entity��������Ͷ���֧��0-65535�����
typedef int32													ENTITY_ID;												// entityID������
typedef uint32													SPACE_ID;												// һ��space��id
typedef uint32													CALLBACK_ID;											// һ��callback��CallbackMgr�����id
typedef uint64													COMPONENT_ID;											// һ�������������id
typedef int32													COMPONENT_ORDER;										// һ�����������˳��
typedef int32													COMPONENT_GUS;											// һ�������genuuid_sections����������������
typedef	uint32													TIMER_ID;												// һ��timer��id����
typedef uint8													ENTITYCALL_CALL_TYPE;									// entityCall ��Ͷ�ݵ�call�������
typedef uint32													GAME_TIME;
typedef uint32													GameTime;
typedef int32													ScriptID;
typedef uint32													ArraySize;												// �κ�����Ĵ�С�����������
typedef uint64													DBID;													// һ�������ݿ��е�������������ĳID
typedef uint32													CELL_ID;
typedef KBEUnordered_map< std::string, std::string >			SPACE_DATA;												// space�д洢������

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
		IFF_UP													= 0x1,	//��ʾ����ӿ������á���������־�����ã���ʾ�ýӿ��ǻ��
		IFF_BROADCAST											= 0x2,	//��ʾ����ӿ�֧�ֹ㲥��ַ���㲥��ַ�����������е������豸��������
		IFF_DEBUG												= 0x4,	//��ʾ����ӿڴ��ڵ���ģʽ����ͨ�����ڿ����Ͳ��Թ����У��Ա㲶��ͷ�����������
		IFF_LOOPBACK											= 0x8,	//��ʾ����ӿ��ǻػ��ӿڣ�loopback interface�����ػ��ӿ������ڱ��������Ͻ�������ͨ�ţ����� lo �� localhost
		IFF_POINTOPOINT											= 0x10,	//��ʾ����ӿ���һ����Ե����ӡ���������ͨ������ֱ��������̨�豸
		IFF_NOTRAILERS											= 0x20,	//��ʾ����ӿڲ�ʹ���ϳ���trailers�����ϳ���һ�����������ݰ�ĩβ��Ӷ�����Ϣ�Ļ��ƣ���Ҫ�����Ż�ĳЩ���͵����紫��
		IFF_RUNNING												= 0x40,	//��ʾ����ӿ��������С������־ͨ�����ں����ã���ʾ�ӿ��Ѿ��ɹ���ʼ����������������
		IFF_NOARP												= 0x80,	//��ʾ����ӿڲ�ʹ�� ARP��Address Resolution Protocol����ARP ���ڽ� IP ��ַ����Ϊ�����ַ���� MAC ��ַ��
		IFF_PROMISC												= 0x100,//��ʾ����ӿڴ��ڻ���ģʽ��promiscuous mode����������ģʽ�£��ӿڻ��������ͨ����������ݰ������������Ƿ�������������ݰ�����ͨ�����������غ�ץ������
		IFF_MULTICAST											= 0x1000//��ʾ����ӿ�֧�ֶಥ��multicast�����ಥ�������ݰ����͵����Ŀ���ַ�������ǵ�����ַ�����е�ַ
	};
#endif
#else
	typedef int													KBESOCKET;
#endif

/*---------------------------------------------------------------------------------
	�������ƽ̨�ϵĶ��߳����
---------------------------------------------------------------------------------*/
#if KBE_PLATFORM == PLATFORM_WIN32
	#define THREAD_ID											HANDLE										//�� Windows �У��̱߳�ʶ����һ�� HANDLE ���͡�HANDLE ��һ��ָ���ں˶���ľ��
	#define THREAD_SINGNAL										HANDLE										//�� Windows �У��߳��ź����������̼߳��ͬ������һ�� HANDLE ���͡�ͨ��ʹ���¼�����CreateEvent����ʵ��
	#define THREAD_SINGNAL_INIT(x)								x = CreateEvent(NULL, TRUE, FALSE, NULL)	//��ʼ��һ���¼�����CreateEvent ��������һ���¼����󣬲����ֱ�Ϊ����ȫ���ԣ�NULL ��ʾĬ�ϣ����Ƿ��ֶ����ã�TRUE ��ʾ�ֶ����ã�����ʼ״̬��FALSE ��ʾδ�����������ƣ�NULL ��ʾ������
	#define THREAD_SINGNAL_DELETE(x)							CloseHandle(x)								//�ر��¼�����CloseHandle �����ر�һ���ں˶���ľ��
	#define THREAD_SINGNAL_SET(x)								SetEvent(x)									//�����¼������״̬Ϊ�Ѵ�����SetEvent �������¼������״̬����Ϊ�Ѵ������Ӷ����ѵȴ����¼����߳�
	#define THREAD_MUTEX										CRITICAL_SECTION							//�� Windows �У���������һ�� CRITICAL_SECTION ���͡�CRITICAL_SECTION ��һ����������ͬ�����ƣ����ڱ����ٽ���
	#define THREAD_MUTEX_INIT(x)								InitializeCriticalSection(&x)				//��ʼ��һ���ٽ�����InitializeCriticalSection ������ʼ��һ�� CRITICAL_SECTION �ṹ
	#define THREAD_MUTEX_DELETE(x)								DeleteCriticalSection(&x)					//ɾ��һ���ٽ�����DeleteCriticalSection ����ɾ��һ�� CRITICAL_SECTION �ṹ
	#define THREAD_MUTEX_LOCK(x)								EnterCriticalSection(&x)					//����һ���ٽ�����EnterCriticalSection ��������һ�� CRITICAL_SECTION��ȷ��ֻ��һ���߳̿��Է����ٽ���
	#define THREAD_MUTEX_UNLOCK(x)								LeaveCriticalSection(&x)					//����һ���ٽ�����LeaveCriticalSection �����뿪һ�� CRITICAL_SECTION�����������߳̽����ٽ���
#else
	#define THREAD_ID											pthread_t									//���� Unix ϵͳ�У��̱߳�ʶ����һ�� pthread_t ���͡�pthread_t �� POSIX �߳̿��ж�����̱߳�ʶ������
	#define THREAD_SINGNAL										pthread_cond_t								//���� Unix ϵͳ�У��߳��ź�����һ�� pthread_cond_t ���͡�pthread_cond_t �� POSIX �߳̿��ж����������������
	#define THREAD_SINGNAL_INIT(x)								pthread_cond_init(&x, NULL)					//��ʼ��һ������������pthread_cond_init ������ʼ��һ�� pthread_cond_t �ṹ�������ֱ�Ϊ����������ָ�롢���ԣ�NULL ��ʾĬ�����ԣ�
	#define THREAD_SINGNAL_DELETE(x)							pthread_cond_destroy(&x)					//����һ������������pthread_cond_destroy ��������һ�� pthread_cond_t �ṹ
	#define THREAD_SINGNAL_SET(x)								pthread_cond_signal(&x);					//�źŻ�һ������������pthread_cond_signal ��������һ���ȴ��������������߳�
	#define THREAD_MUTEX										pthread_mutex_t								//���� Unix ϵͳ�У���������һ�� pthread_mutex_t ���͡�pthread_mutex_t �� POSIX �߳̿��ж���Ļ���������
	#define THREAD_MUTEX_INIT(x)								pthread_mutex_init (&x, NULL)				//��ʼ��һ����������pthread_mutex_init ������ʼ��һ�� pthread_mutex_t �ṹ�������ֱ�Ϊ��������ָ�롢���ԣ�NULL ��ʾĬ�����ԣ�
	#define THREAD_MUTEX_DELETE(x)								pthread_mutex_destroy(&x)					//����һ����������pthread_mutex_destroy ��������һ�� pthread_mutex_t �ṹ
	#define THREAD_MUTEX_LOCK(x)								pthread_mutex_lock(&x)						//����һ����������pthread_mutex_lock ��������һ�� pthread_mutex_t��ȷ��ֻ��һ���߳̿��Է����ٽ���
	#define THREAD_MUTEX_UNLOCK(x)								pthread_mutex_unlock(&x)					//����һ����������pthread_mutex_unlock ��������һ�� pthread_mutex_t�����������߳̽����ٽ���
#endif

/*---------------------------------------------------------------------------------
	��ƽ̨�궨��
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

#define KBE_LITTLE_ENDIAN	//��ǰϵͳʹ��С���ֽ���
/*#define KBE_BIG_ENDIAN*/

#ifdef KBE_LITTLE_ENDIAN	//��ǰϵͳʹ��С���ֽ���
/* accessing individual bytes (int8) and words (int16) within
 * words and long words (int32).
 * Macros ending with W deal with words, L macros deal with longs
 * ������С���ֽ���Little Endian����ϵͳ�д����ֽں��֣�16λ���Լ����֣�32λ��������
 */
 /// ����һ��word(16λ��)�ĸ��ֽ�,��ȡ��8λ������8λ
#define HIBYTEW(b)		(((b) & 0xff00) >> 8)
/// ����һ��word(16λ��)�ĵ��ֽ�.��ȡ��8λ
#define LOBYTEW(b)		( (b) & 0xff)

/// ����һ��long(32λ����)������ֽ�.��ȡ���8λ������24λ
#define HIBYTEL(b)		(((b) & 0xff000000L) >> 24)
/// ����һ��long(32λ����)������ֽ�.��ȡ���8λ
#define LOBYTEL(b)		( (b) & 0xffL)

/// long�ĵ�0�ֽڣ�����ֽڣ�.��ȡ���8λ
#define BYTE0L(b)		( (b) & 0xffL)
/// Returns byte 1 of a long.��ȡ��1�ֽڲ�����8λ
#define BYTE1L(b)		(((b) & 0xff00L) >> 8)
/// Returns byte 2 of a long.
#define BYTE2L(b)		(((b) & 0xff0000L) >> 16)
/// long�ĵ�3�ֽڣ�����ֽ�.
#define BYTE3L(b)		(((b) & 0xff000000L) >> 24)

/// ����һ��long(32λ����)�ĸ�16λ��.��ȡ��16λ������16λ
#define HIWORDL(b)		(((b) & 0xffff0000L) >> 16)
/// ����һ��long(32λ����)�ĵ�16λ��.
#define LOWORDL(b)		( (b) & 0xffffL)

/**
 *	This macro takes a dword ordered 0123 and reorder it to 3210.
 * ��һ��32λ��˫�֣�dword����0123��˳����������Ϊ3210��˳�򡣾�����˵�������ͨ��λ������ÿ���ֽڵ�λ�ý���
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

// ���������ַ�������󳤶�
#define MAX_NAME 256	

// ip�ַ�������󳤶�
#define MAX_IP 256

// �����buf����
#define MAX_BUF 256

// �����buf����
#define SQL_BUF 65535

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// ���ϵͳ���������һ�δ�������
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

/** ��ȡ�û�UID */
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
			iuid = getenv_s( &sz, uid, sizeof( uid ), "UID" ) == 0 ? atoi( uid ) : 0;	// ʹ��getenv_s������ȡ��������"UID"��ֵ��������ת��Ϊ������ֵ��iuid�������ȡʧ�ܣ�iuid����Ϊ0

		// VS2003:
		#elif _MSC_VER < 1400
			char * uid = getenv( "UID" );
			iuid = uid ? atoi( uid ) : 0;	 // �����ȡ�ɹ�������ת��Ϊ������ֵ��iuid������iuid����Ϊ0
		#endif
#else
	// Linux:
		char * uid = getenv( "UID" );
		iuid = uid ? atoi( uid ) : getuid();	// �����ȡ�ɹ�������ת��Ϊ������ֵ��iuid���������getuid������ȡ��ǰ�û���UID

		char * uuid = getenv("UUID");
		iuid = uuid ? atoi( uuid ) : iuid;	 // �����ȡ�ɹ�������ת��Ϊ������ֵ��iuid������iuid���ֲ���
#endif
	}

	return iuid;
}

/** ��ȡ�û��� */
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
			// ����������������֧����������
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

/** ��ȡ����ID */
inline int32 getProcessPID()
{
#if KBE_PLATFORM != PLATFORM_WIN32
	return getpid();
#else
	return (int32) GetCurrentProcessId();
#endif
}

/** ��ȡϵͳʱ��(��ȷ������) */
#if KBE_PLATFORM == PLATFORM_WIN32
	inline uint32 getSystemTime() 
	{ 
		// ע���������windows��ֻ����ȷά��49�졣
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

/** ��ȡ2��ϵͳʱ��� */
inline uint32 getSystemTimeDiff(uint32 oldTime, uint32 newTime)
{
    // ��ֹgetSystemTime()��������
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

/* ����һ��64λ��uuid 
*/
extern COMPONENT_ORDER g_componentGlobalOrder;
extern COMPONENT_ORDER g_componentGroupOrder;

extern COMPONENT_GUS g_genuuid_sections;

inline uint64 genUUID64()
{
	static uint64 tv = (uint64)(time(NULL));
	uint64 now = (uint64)(time(NULL));		// ��ȡ��ǰʱ���

	static uint16 lastNum = 0;	// ���ڴ洢����������ʼֵΪ0

	if(now != tv)		// �����ǰʱ������ϴβ�ͬ
	{
		tv = now;		// ����tvΪ��ǰʱ���
		lastNum = 0;	// ���õ�����Ϊ0
	}
	
	if(g_genuuid_sections <= 0)
	{
		// ʱ���32λ�������16λ��16λ�����������Ϊ65535-1��
		static uint32 rnd = 0;
		if(rnd == 0)			// ����������û������
		{
			srand(getSystemTime());			// ʹ��ϵͳʱ���ʼ�������������
			rnd = (uint32)(rand() << 16);	// ����һ��16λ�������
		}
		
		assert(lastNum < 65535 && "genUUID64(): overflow!");	// ���Ե�����û�г������ֵ65535
		
		return (tv << 32) | rnd | lastNum++;		// ����64λ��UUID����ʱ�����������͵��������
	}
	else
	{
		// ʱ���32λ��app��ID16λ��16λ�����������Ϊ65535-1��
		static uint32 sections = g_genuuid_sections << 16;
		
		assert(lastNum < 65535 && "genUUID64(): overflow!");
		
		return (tv << 32) | sections | lastNum++;		// ����64λ��UUID����ʱ�����app��ID�͵��������
	}
}

/** sleep ��ƽ̨ */
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

/** �ж�ƽ̨�Ƿ�ΪС���ֽ��� */
inline bool isPlatformLittleEndian()
{
   int n = 1;
   return *((char*)&n) ? true : false;
}

/** ���û������� */
#if KBE_PLATFORM == PLATFORM_WIN32
	inline void setenv(const std::string& name, const std::string& value, int overwrite)
	{
		_putenv_s(name.c_str(), value.c_str());
	}
#else
	// Linux����ֱ��ʹ��setenv
#endif

}
#endif // KBE_PLATFORM_H
