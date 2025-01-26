//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
/*
MIT License

Copyright (c) 2017 Lin Wei (skywind3000 at gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	// 在 64 位 Windows 或 AMD64 架构上定义 32 位无符号整型
	typedef unsigned int ISTDUINT32;
	// 在 64 位 Windows 或 AMD64 架构上定义 32 位有符号整型
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32; //  在32位系统下，定义32位无符号整数和32位有符号整数
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	// 在 Apple 和 Mach 系统
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	// 在 Microsoft Visual C++ 或 Borland C++
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	// 在 GCC 编译器上
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;	// 定义双向链表节点结构
};

typedef struct IQUEUEHEAD iqueue_head;	// 定义双向链表头类型


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }	// 初始化双向链表头
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)	// 定义并初始化双向链表头

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))	// 初始化双向链表头

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)	// 计算成员在结构中的偏移量

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))	// 将节点添加到链表头部

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))	// 将节点添加到链表尾部

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n)) //  删除节点p和节点n之间的节点

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)	// 从链表中删除节点

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)	 // 从链表中删除节点并重新初始化

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next) //  定义判断队列是否为空的宏

#define iqueue_init        IQUEUE_INIT		// 别名：初始化双向链表头
#define iqueue_entry       IQUEUE_ENTRY		// 别名：获取包含成员的结构指针
#define iqueue_add         IQUEUE_ADD		// 别名：将节点添加到链表头部
#define iqueue_add_tail    IQUEUE_ADD_TAIL	// 别名：将节点添加到链表尾部
#define iqueue_del         IQUEUE_DEL		// 别名：从链表中删除节点
#define iqueue_del_init    IQUEUE_DEL_INIT	// 别名：从链表中删除节点并重新初始化
#define iqueue_is_empty    IQUEUE_IS_EMPTY	// 别名：检查链表是否为空

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))	// 遍历链表

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)	// 别名：遍历链表

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )	// 遍历链表节点
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)	// 将一个链表插入到另一个链表中

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)	// 如果链表不为空，则将其插入到另一个链表中

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0) // 将一个链表插入到另一个链表中并重新初始化


#ifdef _MSC_VER
#pragma warning(disable:4311)	// 禁用警告：4311 - 类型转换可能导致截断
#pragma warning(disable:4312)	// 禁用警告：4312 - 类型转换可能导致数据丢失
#pragma warning(disable:4996)	// 禁用警告：4996 - 函数或变量已弃用
#endif

#endif


//---------------------------------------------------------------------
// WORD ORDER
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
			// 如果系统是大端模式，则定义 IWORDS_BIG_ENDIAN 为 1
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif



//=====================================================================
// SEGMENT
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node; // 双向链表节点
    IUINT32 conv; // 连接标识符
    IUINT32 cmd; // 命令类型
    IUINT32 frg; // 分片编号
    IUINT32 wnd; // 接收窗口大小
    IUINT32 ts; // 时间戳
    IUINT32 sn; // 序列号
    IUINT32 una; // 已确认的序列号
    IUINT32 len; // 数据长度
    IUINT32 resendts; // 重传时间戳
    IUINT32 rto; // 重传超时时间
    IUINT32 fastack; // 快速确认计数
    IUINT32 xmit; // 传输次数
    char data[1]; // 数据缓冲区
};


//---------------------------------------------------------------------
// IKCPCB
//---------------------------------------------------------------------
struct IKCPCB
{
	IUINT32 conv, mtu, mss, state; // 连接标识符、最大传输单元、最大分段大小、状态
    IUINT32 snd_una, snd_nxt, rcv_nxt; // 发送未确认序列号、下一个发送序列号、下一个接收序列号
    IUINT32 ts_recent, ts_lastack, ssthresh; // 最近的时间戳、最后一个确认的时间戳、慢启动阈值
    IINT32 rx_rttval, rx_srtt, rx_rto, rx_minrto; // 接收方往返时间偏差、平滑往返时间、接收方重传超时时间、最小重传超时时间
    IUINT32 snd_wnd, rcv_wnd, rmt_wnd, cwnd, probe; // 发送窗口大小、接收窗口大小、远程窗口大小、拥塞窗口大小、探测标志
    IUINT32 current, interval, ts_flush, xmit; // 当前时间、更新间隔、刷新时间戳、传输次数
    IUINT32 nrcv_buf, nsnd_buf; // 接收缓冲区大小、发送缓冲区大小
    IUINT32 nrcv_que, nsnd_que; // 接收队列大小、发送队列大小
    IUINT32 nodelay, updated; // 无延迟标志、更新标志
    IUINT32 ts_probe, probe_wait; // 探测时间戳、探测等待时间
    IUINT32 dead_link, incr; // 死链接计数、增量
    struct IQUEUEHEAD snd_queue; // 发送队列
    struct IQUEUEHEAD rcv_queue; // 接收队列
    struct IQUEUEHEAD snd_buf; // 发送缓冲区
    struct IQUEUEHEAD rcv_buf; // 接收缓冲区
    IUINT32 *acklist; // 确认列表
    IUINT32 ackcount; // 确认计数
    IUINT32 ackblock; // 确认块大小
    void *user; // 用户数据
    char *buffer; // 缓冲区
    int fastresend; // 快速重传标志
    int nocwnd; // 无拥塞控制标志
    int stream; // 流模式标志
    int logmask; // 日志掩码
    int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user); // 输出回调函数
    void (*writelog)(const char *log, struct IKCPCB *kcp, void *user); // 写日志回调函数
};


typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1		// 输出日志掩码
#define IKCP_LOG_INPUT			2		// 输入日志掩码
#define IKCP_LOG_SEND			4		// 发送日志掩码
#define IKCP_LOG_RECV			8		// 接收日志掩码
#define IKCP_LOG_IN_DATA		16		// 输入数据日志掩码
#define IKCP_LOG_IN_ACK			32		// 输入确认日志掩码
#define IKCP_LOG_IN_PROBE		64		// 输入探测日志掩码
#define IKCP_LOG_IN_WINS		128		// 输入窗口日志掩码
#define IKCP_LOG_OUT_DATA		256		// 输出数据日志掩码
#define IKCP_LOG_OUT_ACK		512		// 输出确认日志掩码
#define IKCP_LOG_OUT_PROBE		1024	// 输出探测日志掩码
#define IKCP_LOG_OUT_WINS		2048	// 输出窗口日志掩码

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// 接口
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
// 创建一个新的 KCP 控制对象，'conv' 必须在同一个连接的两个端点之间相等。
// 'user' 将传递给输出回调函数。输出回调函数可以这样设置：'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
// 释放 KCP 控制对象
void ikcp_release(ikcpcb *kcp);

// set output callback, which will be invoked by kcp
// 设置输出回调函数，该函数将由 KCP 调用
void ikcp_setoutput(ikcpcb *kcp, int (*output)(const char *buf, int len, 
	ikcpcb *kcp, void *user));

// user/upper level recv: returns size, returns below zero for EAGAIN
// 用户/上层接收：返回接收到的数据大小，如果返回值小于零表示 EAGAIN（资源暂时不可用）
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

// user/upper level send, returns below zero for error
// 用户/上层发送，返回值小于零表示错误
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
// 更新状态（定期调用，每 10ms-100ms），或者你可以通过 ikcp_check 来确定何时再次调用。
// 'current' - 当前时间戳（毫秒）
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
// 确定何时应该调用 ikcp_update：
// 返回下次调用 ikcp_update 的时间（毫秒），如果没有调用 ikcp_input/_send。
// 重要的是减少不必要的 ikcp_update 调用。使用它来调度 ikcp_update（例如实现类似 epoll 的机制，
// 或者在处理大量 KCP 连接时优化 ikcp_update）
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

// when you received a low level packet (eg. UDP packet), call it
// 当你收到一个低层数据包（例如 UDP 数据包）时调用此函数
int ikcp_input(ikcpcb *kcp, const char *data, long size);

// flush pending data
// 刷新待发送的数据
void ikcp_flush(ikcpcb *kcp);

// check the size of next message in the recv queue
// 检查接收队列中下一个消息的大小
int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
// 更改 MTU 大小，默认为 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
// 设置最大窗口大小，默认发送窗口和接收窗口大小为 32
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
// 获取等待发送的数据包数量
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
// 最快速度：ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0: 禁用（默认），1: 启用
// interval: 内部更新定时器间隔（毫秒），默认为 100ms
// resend: 0: 禁用快速重传（默认），1: 启用快速重传
// nc: 0: 正常拥塞控制（默认），1: 禁用拥塞控制
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);

// 记录日志
void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

// setup allocator
// 设置内存分配器
void ikcp_allocator(void* (*new_malloc)(size_t), void (*new_free)(void*));

// read conv
IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif


