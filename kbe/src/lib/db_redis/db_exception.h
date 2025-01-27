// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_REDIS_EXCEPTION_H
#define KBE_REDIS_EXCEPTION_H

#include <string>

namespace KBEngine { 

/**
 * @brief DBException �����ڴ������ݿ��쳣��
 * 
 * ����̳��Ա�׼���е� std::exception �࣬���ڲ���ʹ��������ݿ���ص��쳣��
 * ���ṩ�˻�ȡ�쳣��Ϣ�ķ��������ҿ��Լ���쳣�Ƿ���Ҫ���Ի��Ƿ������Ӷ�ʧ��
 */
class DBInterface;
class DBException : public std::exception
{
public:
	DBException(DBInterface* pdbi);
	~DBException() throw();

	/**
     * @brief ��ȡ�쳣��Ϣ
     * 
     * @return �쳣��Ϣ�� C �ַ���
     */
    virtual const char * what() const throw() { return errStr_.c_str(); }

    /**
     * @brief ����쳣�Ƿ���Ҫ����
     * 
     * @return ����쳣��Ҫ���Է��� true�����򷵻� false
     */
    bool shouldRetry() const;
    /**
     * @brief ����쳣�Ƿ������Ӷ�ʧ
     * 
     * @return ����쳣�����Ӷ�ʧ���� true�����򷵻� false
     */
    bool isLostConnection() const;

private:
	std::string errStr_; //  ������Ϣ�ַ���
	unsigned int errNum_; //  ������
};

}

#endif // KBE_DB_EXCEPTION_H


