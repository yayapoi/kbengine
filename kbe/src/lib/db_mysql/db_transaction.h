// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_MYSQL_TRANSACTION_HELPER_H
#define KBE_MYSQL_TRANSACTION_HELPER_H

namespace KBEngine { 
class DBInterface;
namespace mysql {

/**
 * @brief ���ݿ������࣬���ڹ������ݿ�����Ŀ�ʼ���������ύ��
 */
class DBTransaction
{
public:
	DBTransaction(DBInterface* pdbi, bool autostart = true);
	~DBTransaction();
	
	/**
     * @brief ��ʼ����
     */
    void start();
    /**
     * @brief ��������
     */
    void end();

    /**
     * @brief �ύ����
     */
    void commit();

    /**
     * @brief ����Ƿ���Ҫ��������
     * 
     * @return true ��Ҫ���ԣ�false ����Ҫ����
     */
    bool shouldRetry() const;

    /**
     * @brief �������ݿ�ӿ�ָ�롣
     * 
     * @param pdbi ���ݿ�ӿ�ָ��
     */
    void pdbi(DBInterface* pdbi) { pdbi_ = pdbi; }
	
private:
	DBInterface* pdbi_;  // ���ݿ�ӿ�ָ��
    bool committed_;     // �Ƿ����ύ
    bool autostart_;     // �Ƿ��Զ���������
};

}
}
#endif // KBE_MYSQL_TRANSACTION_HELPER_H

