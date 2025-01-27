// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_MYSQL_EXCEPTION_H
#define KBE_MYSQL_EXCEPTION_H

#include <string>

namespace KBEngine { 

class DBInterface;

/**
 * @brief ���ݿ��쳣�࣬���ڴ������ݿ�����з������쳣��
 * 
 * ����̳��Ա�׼���е� `std::exception` �࣬�ṩ����ϸ�Ĵ�����Ϣ��һЩ����������
 */
class DBException : public std::exception
{
public:
	DBException(DBInterface* pdbi);
	~DBException() throw();

	/**
     * @brief ��ȡ�쳣����ϸ����
     * 
     * @return const char* �쳣����ϸ�����ַ���
     */
    virtual const char * what() const throw() { return errStr_.c_str(); }

    /**
     * @brief �ж��Ƿ�Ӧ�����Ըò���
     * 
     * @return bool ���Ӧ�������򷵻�true�����򷵻�false
     */
    bool shouldRetry() const;
    /**
     * @brief �ж��Ƿ������Ӷ�ʧ���쳣
     * 
     * @return bool ��������Ӷ�ʧ���쳣�򷵻�true�����򷵻�false
     */
    bool isLostConnection() const;

    /**
     * @brief ���ô�����Ϣ�ʹ�����
     * 
     * @param errStr ������Ϣ�ַ���
     * @param errNum ������
     */
    void setError(const std::string& errStr, unsigned int errNum)
    {
        errStr_ = errStr;
        errNum_ = errNum;
    }

private:
    /**
     * @brief �洢������Ϣ���ַ���
     */
    std::string errStr_;
    /**
     * @brief �洢������
     */
    unsigned int errNum_;
};

}

#endif // KBE_MYSQL_EXCEPTION_H


