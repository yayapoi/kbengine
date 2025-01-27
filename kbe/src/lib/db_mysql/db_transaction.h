// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_MYSQL_TRANSACTION_HELPER_H
#define KBE_MYSQL_TRANSACTION_HELPER_H

namespace KBEngine { 
class DBInterface;
namespace mysql {

/**
 * @brief 数据库事务类，用于管理数据库事务的开始、结束和提交。
 */
class DBTransaction
{
public:
	DBTransaction(DBInterface* pdbi, bool autostart = true);
	~DBTransaction();
	
	/**
     * @brief 开始事务。
     */
    void start();
    /**
     * @brief 结束事务。
     */
    void end();

    /**
     * @brief 提交事务。
     */
    void commit();

    /**
     * @brief 检查是否需要重试事务。
     * 
     * @return true 需要重试，false 不需要重试
     */
    bool shouldRetry() const;

    /**
     * @brief 设置数据库接口指针。
     * 
     * @param pdbi 数据库接口指针
     */
    void pdbi(DBInterface* pdbi) { pdbi_ = pdbi; }
	
private:
	DBInterface* pdbi_;  // 数据库接口指针
    bool committed_;     // 是否已提交
    bool autostart_;     // 是否自动启动事务
};

}
}
#endif // KBE_MYSQL_TRANSACTION_HELPER_H

