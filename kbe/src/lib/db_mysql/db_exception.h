// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_MYSQL_EXCEPTION_H
#define KBE_MYSQL_EXCEPTION_H

#include <string>

namespace KBEngine { 

class DBInterface;

/**
 * @brief 数据库异常类，用于处理数据库操作中发生的异常。
 * 
 * 该类继承自标准库中的 `std::exception` 类，提供了详细的错误信息和一些辅助方法。
 */
class DBException : public std::exception
{
public:
	DBException(DBInterface* pdbi);
	~DBException() throw();

	/**
     * @brief 获取异常的详细描述
     * 
     * @return const char* 异常的详细描述字符串
     */
    virtual const char * what() const throw() { return errStr_.c_str(); }

    /**
     * @brief 判断是否应该重试该操作
     * 
     * @return bool 如果应该重试则返回true，否则返回false
     */
    bool shouldRetry() const;
    /**
     * @brief 判断是否是连接丢失的异常
     * 
     * @return bool 如果是连接丢失的异常则返回true，否则返回false
     */
    bool isLostConnection() const;

    /**
     * @brief 设置错误信息和错误编号
     * 
     * @param errStr 错误信息字符串
     * @param errNum 错误编号
     */
    void setError(const std::string& errStr, unsigned int errNum)
    {
        errStr_ = errStr;
        errNum_ = errNum;
    }

private:
    /**
     * @brief 存储错误信息的字符串
     */
    std::string errStr_;
    /**
     * @brief 存储错误编号
     */
    unsigned int errNum_;
};

}

#endif // KBE_MYSQL_EXCEPTION_H


