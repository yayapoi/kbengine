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
	// �� 64 λ Windows �� AMD64 �ܹ��϶��� 32 λ�޷�������
	typedef unsigned int ISTDUINT32;
	// �� 64 λ Windows �� AMD64 �ܹ��϶��� 32 λ�з�������
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32; //  ��32λϵͳ�£�����32λ�޷���������32λ�з�������
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	// �� Apple �� Mach ϵͳ
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	// �� Microsoft Visual C++ �� Borland C++
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	// �� GCC ��������
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
	struct IQUEUEHEAD *next, *prev;	// ����˫������ڵ�ṹ
};

typedef struct IQUEUEHEAD iqueue_head;	// ����˫������ͷ����


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }	// ��ʼ��˫������ͷ
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)	// ���岢��ʼ��˫������ͷ

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))	// ��ʼ��˫������ͷ

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)	// �����Ա�ڽṹ�е�ƫ����

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))	// ���ڵ���ӵ�����ͷ��

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))	// ���ڵ���ӵ�����β��

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n)) //  ɾ���ڵ�p�ͽڵ�n֮��Ľڵ�

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)	// ��������ɾ���ڵ�

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)	 // ��������ɾ���ڵ㲢���³�ʼ��

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next) //  �����ж϶����Ƿ�Ϊ�յĺ�

#define iqueue_init        IQUEUE_INIT		// ��������ʼ��˫������ͷ
#define iqueue_entry       IQUEUE_ENTRY		// ��������ȡ������Ա�Ľṹָ��
#define iqueue_add         IQUEUE_ADD		// ���������ڵ���ӵ�����ͷ��
#define iqueue_add_tail    IQUEUE_ADD_TAIL	// ���������ڵ���ӵ�����β��
#define iqueue_del         IQUEUE_DEL		// ��������������ɾ���ڵ�
#define iqueue_del_init    IQUEUE_DEL_INIT	// ��������������ɾ���ڵ㲢���³�ʼ��
#define iqueue_is_empty    IQUEUE_IS_EMPTY	// ��������������Ƿ�Ϊ��

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))	// ��������

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)	// ��������������

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )	// ��������ڵ�
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)	// ��һ��������뵽��һ��������

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)	// �������Ϊ�գ�������뵽��һ��������

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0) // ��һ��������뵽��һ�������в����³�ʼ��


#ifdef _MSC_VER
#pragma warning(disable:4311)	// ���þ��棺4311 - ����ת�����ܵ��½ض�
#pragma warning(disable:4312)	// ���þ��棺4312 - ����ת�����ܵ������ݶ�ʧ
#pragma warning(disable:4996)	// ���þ��棺4996 - ���������������
#endif

#endif


//---------------------------------------------------------------------
// WORD ORDER
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
			// ���ϵͳ�Ǵ��ģʽ������ IWORDS_BIG_ENDIAN Ϊ 1
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
	struct IQUEUEHEAD node; // ˫������ڵ�
    IUINT32 conv; // ���ӱ�ʶ��
    IUINT32 cmd; // ��������
    IUINT32 frg; // ��Ƭ���
    IUINT32 wnd; // ���մ��ڴ�С
    IUINT32 ts; // ʱ���
    IUINT32 sn; // ���к�
    IUINT32 una; // ��ȷ�ϵ����к�
    IUINT32 len; // ���ݳ���
    IUINT32 resendts; // �ش�ʱ���
    IUINT32 rto; // �ش���ʱʱ��
    IUINT32 fastack; // ����ȷ�ϼ���
    IUINT32 xmit; // �������
    char data[1]; // ���ݻ�����
};


//---------------------------------------------------------------------
// IKCPCB
//---------------------------------------------------------------------
struct IKCPCB
{
	IUINT32 conv, mtu, mss, state; // ���ӱ�ʶ��������䵥Ԫ�����ֶδ�С��״̬
    IUINT32 snd_una, snd_nxt, rcv_nxt; // ����δȷ�����кš���һ���������кš���һ���������к�
    IUINT32 ts_recent, ts_lastack, ssthresh; // �����ʱ��������һ��ȷ�ϵ�ʱ�������������ֵ
    IINT32 rx_rttval, rx_srtt, rx_rto, rx_minrto; // ���շ�����ʱ��ƫ�ƽ������ʱ�䡢���շ��ش���ʱʱ�䡢��С�ش���ʱʱ��
    IUINT32 snd_wnd, rcv_wnd, rmt_wnd, cwnd, probe; // ���ʹ��ڴ�С�����մ��ڴ�С��Զ�̴��ڴ�С��ӵ�����ڴ�С��̽���־
    IUINT32 current, interval, ts_flush, xmit; // ��ǰʱ�䡢���¼����ˢ��ʱ������������
    IUINT32 nrcv_buf, nsnd_buf; // ���ջ�������С�����ͻ�������С
    IUINT32 nrcv_que, nsnd_que; // ���ն��д�С�����Ͷ��д�С
    IUINT32 nodelay, updated; // ���ӳٱ�־�����±�־
    IUINT32 ts_probe, probe_wait; // ̽��ʱ�����̽��ȴ�ʱ��
    IUINT32 dead_link, incr; // �����Ӽ���������
    struct IQUEUEHEAD snd_queue; // ���Ͷ���
    struct IQUEUEHEAD rcv_queue; // ���ն���
    struct IQUEUEHEAD snd_buf; // ���ͻ�����
    struct IQUEUEHEAD rcv_buf; // ���ջ�����
    IUINT32 *acklist; // ȷ���б�
    IUINT32 ackcount; // ȷ�ϼ���
    IUINT32 ackblock; // ȷ�Ͽ��С
    void *user; // �û�����
    char *buffer; // ������
    int fastresend; // �����ش���־
    int nocwnd; // ��ӵ�����Ʊ�־
    int stream; // ��ģʽ��־
    int logmask; // ��־����
    int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user); // ����ص�����
    void (*writelog)(const char *log, struct IKCPCB *kcp, void *user); // д��־�ص�����
};


typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1		// �����־����
#define IKCP_LOG_INPUT			2		// ������־����
#define IKCP_LOG_SEND			4		// ������־����
#define IKCP_LOG_RECV			8		// ������־����
#define IKCP_LOG_IN_DATA		16		// ����������־����
#define IKCP_LOG_IN_ACK			32		// ����ȷ����־����
#define IKCP_LOG_IN_PROBE		64		// ����̽����־����
#define IKCP_LOG_IN_WINS		128		// ���봰����־����
#define IKCP_LOG_OUT_DATA		256		// ���������־����
#define IKCP_LOG_OUT_ACK		512		// ���ȷ����־����
#define IKCP_LOG_OUT_PROBE		1024	// ���̽����־����
#define IKCP_LOG_OUT_WINS		2048	// ���������־����

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// �ӿ�
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
// ����һ���µ� KCP ���ƶ���'conv' ������ͬһ�����ӵ������˵�֮����ȡ�
// 'user' �����ݸ�����ص�����������ص����������������ã�'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
// �ͷ� KCP ���ƶ���
void ikcp_release(ikcpcb *kcp);

// set output callback, which will be invoked by kcp
// ��������ص��������ú������� KCP ����
void ikcp_setoutput(ikcpcb *kcp, int (*output)(const char *buf, int len, 
	ikcpcb *kcp, void *user));

// user/upper level recv: returns size, returns below zero for EAGAIN
// �û�/�ϲ���գ����ؽ��յ������ݴ�С���������ֵС�����ʾ EAGAIN����Դ��ʱ�����ã�
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

// user/upper level send, returns below zero for error
// �û�/�ϲ㷢�ͣ�����ֵС�����ʾ����
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
// ����״̬�����ڵ��ã�ÿ 10ms-100ms�������������ͨ�� ikcp_check ��ȷ����ʱ�ٴε��á�
// 'current' - ��ǰʱ��������룩
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
// ȷ����ʱӦ�õ��� ikcp_update��
// �����´ε��� ikcp_update ��ʱ�䣨���룩�����û�е��� ikcp_input/_send��
// ��Ҫ���Ǽ��ٲ���Ҫ�� ikcp_update ���á�ʹ���������� ikcp_update������ʵ������ epoll �Ļ��ƣ�
// �����ڴ������ KCP ����ʱ�Ż� ikcp_update��
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

// when you received a low level packet (eg. UDP packet), call it
// �����յ�һ���Ͳ����ݰ������� UDP ���ݰ���ʱ���ô˺���
int ikcp_input(ikcpcb *kcp, const char *data, long size);

// flush pending data
// ˢ�´����͵�����
void ikcp_flush(ikcpcb *kcp);

// check the size of next message in the recv queue
// �����ն�������һ����Ϣ�Ĵ�С
int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
// ���� MTU ��С��Ĭ��Ϊ 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
// ������󴰿ڴ�С��Ĭ�Ϸ��ʹ��ںͽ��մ��ڴ�СΪ 32
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
// ��ȡ�ȴ����͵����ݰ�����
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
// ����ٶȣ�ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0: ���ã�Ĭ�ϣ���1: ����
// interval: �ڲ����¶�ʱ����������룩��Ĭ��Ϊ 100ms
// resend: 0: ���ÿ����ش���Ĭ�ϣ���1: ���ÿ����ش�
// nc: 0: ����ӵ�����ƣ�Ĭ�ϣ���1: ����ӵ������
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);

// ��¼��־
void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

// setup allocator
// �����ڴ������
void ikcp_allocator(void* (*new_malloc)(size_t), void (*new_free)(void*));

// read conv
IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif


