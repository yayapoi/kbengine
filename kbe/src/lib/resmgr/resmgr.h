// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

/*
	��Դ��������
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

// ������Դ�ı�־λ
#define RESOURCE_NORMAL	0x00000000
#define RESOURCE_RESIDENT 0x00000001
#define RESOURCE_READ 0x00000002
#define RESOURCE_WRITE 0x00000004
#define RESOURCE_APPEND 0x00000008

// ��Դ�������࣬�̳��� Singleton �� TimerHandler
class Resmgr : public Singleton<Resmgr>, public TimerHandler
{
public:
	// ���滷�������ṹ��
	struct KBEEnv
	{
		std::string root_path; // ��·��
		std::string res_path;  // ��Դ·��
		std::string bin_path;  // �������ļ�·��
	};

	static uint64 respool_timeout; // ��Դ�س�ʱʱ��
	static uint32 respool_buffersize; // ��Դ�ػ�������С
	static uint32 respool_checktick; // ��Դ�ؼ����

public:
	Resmgr(); // ���캯��
	~Resmgr(); // ��������
	
	bool initialize(); // ��ʼ����Դ������

	void autoSetPaths(); // �Զ�����·��
	void updatePaths(); // ����·��

	const Resmgr::KBEEnv& getEnv() { return kb_env_; } // ��ȡ��������

	/*
		����Դ·����(����������ָ����)ƥ�䵽��������Դ��ַ
	*/
	std::string matchRes(const std::string& res);
	std::string matchRes(const char* res);
	
	bool hasRes(const std::string& res); // �����Դ�Ƿ����
	
	FILE* openRes(std::string res, const char* mode = "r"); // ����Դ�ļ�

	/*
		�г�Ŀ¼�����е���Դ�ļ�
	*/
	bool listPathRes(std::wstring path, const std::wstring& extendName, std::vector<std::wstring>& results);

	/*
		����Դ·����(����������ָ����)ƥ�䵽Ŀ¼
	*/
	std::string matchPath(const std::string& path);
	std::string matchPath(const char* path);

	const std::vector<std::string>& respaths() { 
		return respaths_; // ������Դ·���б�
	}

	void print(void); // ��ӡ��Դ��������Ϣ

	bool isInit(){ 
		return isInit_; // ����Ƿ��ѳ�ʼ��
	}

	/**
		�������ϵͳ����ԴĿ¼
		kbe\\res\\*
	*/
	std::string getPySysResPath();

	/**
		����û�����ԴĿ¼
		assets\\res\\*
	*/
	std::string getPyUserResPath();

	/**
		����û����ű�Ŀ¼
		assets\\scripts\\*
	*/
	std::string getPyUserScriptsPath();

	/**
		����û������̽ű�Ŀ¼
		assets\\scripts\\cell��base��client
	*/
	std::string getPyUserComponentScriptsPath(COMPONENT_TYPE componentType = UNKNOWN_COMPONENT_TYPE);

	/**
		����û�����Ŀ¼
		assets\\*
	*/
	std::string getPyUserAssetsPath();

	ResourceObjectPtr openResource(const char* res, const char* model, 
		uint32 flags = RESOURCE_NORMAL); // ����Դ����

	bool initializeWatcher(); // ��ʼ����Դ������

	void update(); // ������Դ������

private:

	virtual void handleTimeout(TimerHandle handle, void * arg); // ����ʱ�¼�

	KBEEnv kb_env_; // ��������
	std::vector<std::string> respaths_; // ��Դ·���б�
	bool isInit_; // �Ƿ��ѳ�ʼ��

	KBEUnordered_map< std::string, ResourceObjectPtr > respool_; // ��Դ��

	KBEngine::thread::ThreadMutex mutex_; // ������
};

}

#endif // KBE_RESMGR_H
