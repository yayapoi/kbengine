// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_DEBUG_H
#define KBE_DEBUG_H

#include "common/platform.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning(disable:4819)
#endif
#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "network/common.h"
#include "network/address.h"

namespace KBEngine{

namespace Network{
	class Channel;
	class Bundle;
	class EventDispatcher;
	class NetworkInterface;
	class Packet;
}

/** 
	支持uft-8编码字符串输出 
*/
void vutf8printf(FILE *out, const char *str, va_list* ap);
void utf8printf(FILE *out, const char *str, ...);


#define	KBELOG_UNKNOWN			0x00000000
#define	KBELOG_PRINT			0x00000001
#define	KBELOG_ERROR			0x00000002
#define	KBELOG_WARNING			0x00000004
#define	KBELOG_DEBUG			0x00000008
#define	KBELOG_INFO				0x00000010
#define	KBELOG_CRITICAL			0x00000020
#define KBELOG_SCRIPT_INFO		0x00000040
#define KBELOG_SCRIPT_ERROR		0x00000080
#define KBELOG_SCRIPT_DEBUG		0x00000100
#define KBELOG_SCRIPT_WARNING	0x00000200
#define KBELOG_SCRIPT_NORMAL	0x00000400
//未知、打印、错误、警告、调试、信息、关键、脚本信息、脚本错误、脚本调试、脚本警告、脚本正常
#define KBELOG_TYPES KBELOG_UNKNOWN | KBELOG_PRINT | KBELOG_ERROR | KBELOG_WARNING | \
	KBELOG_DEBUG | KBELOG_INFO | KBELOG_CRITICAL | KBELOG_SCRIPT_INFO | KBELOG_SCRIPT_ERROR | KBELOG_SCRIPT_DEBUG | \
	KBELOG_SCRIPT_WARNING | KBELOG_SCRIPT_NORMAL

const char KBELOG_TYPE_NAME[][255] = {
	" UNKNOWN",
	"        ",
	"   ERROR",
	" WARNING",
	"   DEBUG",
	"    INFO",
	"CRITICAL",
	"  S_INFO",
	"  S_ERR",
	"  S_DBG",
	"  S_WARN",
	"  S_NORM",
};

inline const char* KBELOG_TYPE_NAME_EX(uint32 CTYPE)
{									
	if(CTYPE < 0 || ((CTYPE) & (KBELOG_TYPES)) <= 0)
	{
		return " UNKNOWN";
	}
	
	switch(CTYPE)
	{
	case KBELOG_PRINT:
		return "        ";
	case KBELOG_ERROR:
		return "   ERROR";
	case KBELOG_WARNING:
		return " WARNING";
	case KBELOG_DEBUG:
		return "   DEBUG";
	case KBELOG_INFO:
		return "    INFO";
	case KBELOG_CRITICAL:
		return "CRITICAL";
	case KBELOG_SCRIPT_INFO:
		return "  S_INFO";
	case KBELOG_SCRIPT_ERROR:
		return "   S_ERR";
	case KBELOG_SCRIPT_DEBUG:
		return "   S_DBG";
	case KBELOG_SCRIPT_WARNING:
		return "  S_WARN";
	case KBELOG_SCRIPT_NORMAL:
		return "  S_NORM";
	};

	return " UNKNOWN";
}

int KBELOG_TYPE_MAPPING(int type);

class DebugHelper  : public Singleton<DebugHelper>
{
public:
	DebugHelper();

	~DebugHelper();
	
	static bool isInit() { return getSingletonPtr() != 0; }

	static void initialize(COMPONENT_TYPE componentType);	//初始化日志系统
	static void finalise(bool destroy = false);		//销毁日志系统?

	void setFile(std::string funcname, std::string file, uint32 line){
		_currFile = file;
		_currLine = line;
		_currFuncName = funcname;
	}
	
	std::string getLogName();

	void lockthread();
	void unlockthread();
    
	void pNetworkInterface(Network::NetworkInterface* networkInterface);
	void pDispatcher(Network::EventDispatcher* dispatcher);
	
	Network::EventDispatcher* pDispatcher() const{ return pDispatcher_; }
	Network::NetworkInterface* pNetworkInterface() const{ return pNetworkInterface_; }

	void print_msg(const std::string& s);
	void debug_msg(const std::string& s);
	void error_msg(const std::string& s);
	void info_msg(const std::string& s);
	void warning_msg(const std::string& s);
	void critical_msg(const std::string& s);
	void script_info_msg(const std::string& s);
	void script_error_msg(const std::string& s);
	void backtrace_msg();

	void onMessage(uint32 logType, const char * str, uint32 length);

	void registerLogger(Network::MessageID msgID, Network::Address* pAddr);	//注册和注销日志记录器
	void unregisterLogger(Network::MessageID msgID, Network::Address* pAddr);

	void onNoLogger();		//处理没有日志记录器的情况

	void changeLogger(const std::string& name);		//切换和关闭日志记录器
	void closeLogger();  // close logger for fork + execv

	void clearBufferedLog(bool destroy = false);	//清除缓冲的日志

	void set_errorcolor();
	void set_normalcolor();
	void set_warningcolor();

	void setScriptMsgType(int msgtype);		//设置脚本消息类型
	void resetScriptMsgType();				//重置脚本消息类型

	void shouldWriteToSyslog(bool v = true);	//设置是否将日志写入系统日志

	/** 
		同步日志到logger
	*/
	void sync();

	void printBufferedLogs();		//打印缓冲的日志

	size_t hasBufferedLogPackets() const{ return hasBufferedLogPackets_; }	//获取缓冲的日志包数量

	Network::Channel* pLoggerChannel();		//获取日志通道

	bool canLog(int level);		//检查是否可以记录指定级别的日志

private:
	FILE* _logfile;
	std::string _currFile, _currFuncName;	//当前文件、函数名和行号
	uint32 _currLine;

	Network::Address loggerAddr_;				//日志记录器的地址
	KBEngine::thread::ThreadMutex logMutex;

	std::queue< Network::Bundle* > bufferedLogPackets_;	//缓冲的日志包队列和数量
	size_t hasBufferedLogPackets_;

	Network::NetworkInterface* pNetworkInterface_;		//网络接口和事件调度器指针
	Network::EventDispatcher* pDispatcher_;

	int scriptMsgType_;			//脚本消息类型

	bool noSyncLog_;			//是否禁止同步日志

	bool canLogFile_;			//是否可以写入日志文件

	uint64 loseLoggerTime_;		//丢失日志记录器的时间

	// 记录下主线程ID，用于判断是否是子线程输出日志
	// 当子线程输出日志时，对相关日志进行缓存到主线程时再同步给logger
#if KBE_PLATFORM == PLATFORM_WIN32
	DWORD mainThreadID_;
#else
	THREAD_ID mainThreadID_;
#endif

	ObjectPool<MemoryStream> memoryStreamPool_;						//内存流池和子线程缓冲的日志包队列
	std::queue< MemoryStream* > childThreadBufferedLogPackets_;
};

/*---------------------------------------------------------------------------------
	调试信息输出接口
---------------------------------------------------------------------------------*/
#define SCRIPT_INFO_MSG(m)				DebugHelper::getSingleton().script_info_msg((m))							// 输出info信息
#define SCRIPT_ERROR_MSG(m)				DebugHelper::getSingleton().script_error_msg((m))							// 输出错误信息

#define PRINT_MSG(m)					DebugHelper::getSingleton().print_msg((m))									// 输出任何信息
#define ERROR_MSG(m)					DebugHelper::getSingleton().error_msg((m))									// 输出一个错误
#define DEBUG_MSG(m)					DebugHelper::getSingleton().debug_msg((m))									// 输出一个debug信息
#define INFO_MSG(m)						DebugHelper::getSingleton().info_msg((m))									// 输出一个info信息
#define WARNING_MSG(m)					DebugHelper::getSingleton().warning_msg((m))								// 输出一个警告信息
#define CRITICAL_MSG(m)					DebugHelper::getSingleton().setFile(__FUNCTION__, \
										__FILE__, __LINE__); \
										DebugHelper::getSingleton().critical_msg((m))

/*---------------------------------------------------------------------------------
	调试宏
---------------------------------------------------------------------------------*/
#ifdef KBE_USE_ASSERTS
void myassert(const char* exp, const char * func, const char * file, unsigned int line);
#define KBE_ASSERT(exp) if(!(exp))myassert(#exp, __FUNCTION__, __FILE__, __LINE__);
#define KBE_REAL_ASSERT assert(0);
#else
#define KBE_ASSERT(exp) assert((exp));
#define KBE_REAL_ASSERT assert(0);
#endif

#ifdef _DEBUG
#define KBE_VERIFY KBE_ASSERT
#else
#define KBE_VERIFY(exp) (exp)
#endif

#define KBE_EXIT(msg) {														\
			CRITICAL_MSG(msg);												\
			KBE_REAL_ASSERT	}												\


}

#endif // KBE_DEBUG_H
