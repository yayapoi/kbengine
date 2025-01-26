// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

/*
	资源管理器。
*/
#ifndef KBE_RESMGR_H
#define KBE_RESMGR_H

#include "resourceobject.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/timer.h"
#include "xml/xml.h"	
#include "common/smartpointer.h"
	
namespace KBEngine{

// 定义资源的标志位
#define RESOURCE_NORMAL	0x00000000
#define RESOURCE_RESIDENT 0x00000001
#define RESOURCE_READ 0x00000002
#define RESOURCE_WRITE 0x00000004
#define RESOURCE_APPEND 0x00000008

// 资源管理器类，继承自 Singleton 和 TimerHandler
class Resmgr : public Singleton<Resmgr>, public TimerHandler
{
public:
	// 引擎环境变量结构体
	struct KBEEnv
	{
		std::string root_path; // 根路径
		std::string res_path;  // 资源路径
		std::string bin_path;  // 二进制文件路径
	};

	static uint64 respool_timeout; // 资源池超时时间
	static uint32 respool_buffersize; // 资源池缓冲区大小
	static uint32 respool_checktick; // 资源池检查间隔

public:
	Resmgr(); // 构造函数
	~Resmgr(); // 析构函数
	
	bool initialize(); // 初始化资源管理器

	void autoSetPaths(); // 自动设置路径
	void updatePaths(); // 更新路径

	const Resmgr::KBEEnv& getEnv() { return kb_env_; } // 获取环境变量

	/*
		从资源路径中(环境变量中指定的)匹配到完整的资源地址
	*/
	std::string matchRes(const std::string& res);
	std::string matchRes(const char* res);
	
	bool hasRes(const std::string& res); // 检查资源是否存在
	
	FILE* openRes(std::string res, const char* mode = "r"); // 打开资源文件

	/*
		列出目录下所有的资源文件
	*/
	bool listPathRes(std::wstring path, const std::wstring& extendName, std::vector<std::wstring>& results);

	/*
		从资源路径中(环境变量中指定的)匹配到目录
	*/
	std::string matchPath(const std::string& path);
	std::string matchPath(const char* path);

	const std::vector<std::string>& respaths() { 
		return respaths_; // 返回资源路径列表
	}

	void print(void); // 打印资源管理器信息

	bool isInit(){ 
		return isInit_; // 检查是否已初始化
	}

	/**
		获得引擎系统级资源目录
		kbe\\res\\*
	*/
	std::string getPySysResPath();

	/**
		获得用户级资源目录
		assets\\res\\*
	*/
	std::string getPyUserResPath();

	/**
		获得用户级脚本目录
		assets\\scripts\\*
	*/
	std::string getPyUserScriptsPath();

	/**
		获得用户级进程脚本目录
		assets\\scripts\\cell、base、client
	*/
	std::string getPyUserComponentScriptsPath(COMPONENT_TYPE componentType = UNKNOWN_COMPONENT_TYPE);

	/**
		获得用户级库目录
		assets\\*
	*/
	std::string getPyUserAssetsPath();

	ResourceObjectPtr openResource(const char* res, const char* model, 
		uint32 flags = RESOURCE_NORMAL); // 打开资源对象

	bool initializeWatcher(); // 初始化资源监视器

	void update(); // 更新资源管理器

private:

	virtual void handleTimeout(TimerHandle handle, void * arg); // 处理超时事件

	KBEEnv kb_env_; // 环境变量
	std::vector<std::string> respaths_; // 资源路径列表
	bool isInit_; // 是否已初始化

	KBEUnordered_map< std::string, ResourceObjectPtr > respool_; // 资源池

	KBEngine::thread::ThreadMutex mutex_; // 互斥锁
};

}

#endif // KBE_RESMGR_H
