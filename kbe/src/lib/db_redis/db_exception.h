// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_REDIS_EXCEPTION_H
#define KBE_REDIS_EXCEPTION_H

#include <string>

namespace KBEngine { 

/**
 * @brief DBException 类用于处理数据库异常。
 * 
 * 该类继承自标准库中的 std::exception 类，用于捕获和处理与数据库相关的异常。
 * 它提供了获取异常信息的方法，并且可以检查异常是否需要重试或是否是连接丢失。
 */
class DBInterface;
class DBException : public std::exception
{
public:
	DBException(DBInterface* pdbi);
	~DBException() throw();

	/**
     * @brief 获取异常信息
     * 
     * @return 异常信息的 C 字符串
     */
    virtual const char * what() const throw() { return errStr_.c_str(); }

    /**
     * @brief 检查异常是否需要重试
     * 
     * @return 如果异常需要重试返回 true，否则返回 false
     */
    bool shouldRetry() const;
    /**
     * @brief 检查异常是否是连接丢失
     * 
     * @return 如果异常是连接丢失返回 true，否则返回 false
     */
    bool isLostConnection() const;

private:
	std::string errStr_; //  错误信息字符串
	unsigned int errNum_; //  错误编号
};

}

#endif // KBE_DB_EXCEPTION_H


