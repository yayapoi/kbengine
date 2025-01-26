// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_FIXED_NETWORK_MESSAGES_H
#define KBE_FIXED_NETWORK_MESSAGES_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "network/common.h"

namespace KBEngine { 
namespace Network
{
/*
	�������м�Э��(ǰ������֮��)����ǿ��Լ����
	û��ʹ�õ�kbe����Э���Զ��󶨻��Ƶ�ǰ�˿���ʹ�ô˴���ǿ��Լ��Э�顣
*/
class FixedMessages : public Singleton<FixedMessages>
{
public:

	// �̶���Э�����ݽṹ
	struct MSGInfo
	{
		MessageID msgid; //  ��ϢID
		std::string msgname; //  ��Ϣ����
		//std::wstring descr; //  ��Ϣ����
	};

public:
	FixedMessages();
	~FixedMessages();

	bool loadConfig(std::string fileName, bool notFoundError = true); //  ���������ļ�

	FixedMessages::MSGInfo* isFixed(const char* msgName); //  ������Ϣ�����ж��Ƿ�Ϊ�̶���Ϣ
	bool isFixed(MessageID msgid); //  ������ϢID�ж��Ƿ�Ϊ�̶���Ϣ

public:
	typedef KBEUnordered_map<std::string, MSGInfo> MSGINFO_MAP; //  ����һ���ַ�����MSGInfo��ӳ������

private:
	MSGINFO_MAP _infomap; //  �洢��Ϣ��Ϣ��ӳ��
	bool _loaded;	// �����Ƿ��Ѽ���
};

}
}
#endif // KBE_FIXED_NETWORK_MESSAGES_H
