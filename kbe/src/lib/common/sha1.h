#ifndef _SHA1_H_
#define _SHA1_H_

// �����ռ� KBEngine �Ķ��壬���ڷ�װ��ͺ���
namespace KBEngine {

// ������һ����Ϊ KBE_SHA1 ���࣬����ʵ�� SHA-1 ��ϣ�㷨
class KBE_SHA1
{
public:

    // ���캯�������ڳ�ʼ������
    KBE_SHA1();

    // ������������ȷ���������������������ȷ����
    virtual ~KBE_SHA1();

    // ���³�ʼ���࣬ʹ��������¼����µĹ�ϣֵ
    void Reset();

    // ������ϢժҪ������洢�ڴ����������
    bool Result(unsigned *message_digest_array);

    // �ṩ�������ݸ� SHA1 �㷨������Ϊ�޷����ַ�����ͳ���
    void Input(const unsigned char *message_array, unsigned length);
    
    // �ṩ�������ݸ� SHA1 �㷨������Ϊ�ַ�����ͳ���
    void Input(const char *message_array, unsigned length);
    
    // �ṩ�����޷����ַ���Ϊ��������
    void Input(unsigned char message_element);
    
    // �ṩ�����ַ���Ϊ��������
    void Input(char message_element);
    
    // ���� << �����������ʹ����ʽ���뷽ʽ�ṩ�ַ�����
    KBE_SHA1& operator<<(const char *message_array);
    
    // ���� << �����������ʹ����ʽ���뷽ʽ�ṩ�޷����ַ�����
    KBE_SHA1& operator<<(const unsigned char *message_array);
    
    // ���� << �����������ʹ����ʽ���뷽ʽ�ṩ�����ַ�
    KBE_SHA1& operator<<(const char message_element);
    
    // ���� << �����������ʹ����ʽ���뷽ʽ�ṩ�����޷����ַ�
    KBE_SHA1& operator<<(const unsigned char message_element);

private:

    // ������һ�� 512 λ����Ϣ��
    void ProcessMessageBlock();

    // �Ե�ǰ��Ϣ�������䣬ʹ��ﵽ 512 λ
    void PadMessage();

    // ִ��ѭ�����Ʋ���
    inline unsigned CircularShift(int bits, unsigned word);

    unsigned H[5];                      // ��ϢժҪ������

    unsigned Length_Low;                // ��Ϣ���ȣ��� 32 λ��
    unsigned Length_High;               // ��Ϣ���ȣ��� 32 λ��

    unsigned char Message_Block[64];    // 512 λ����Ϣ��
    int Message_Block_Index;            // ��Ϣ�����������

    bool Computed;                      // ��ϢժҪ�Ƿ��Ѽ������
    bool Corrupted;                     // ��ϢժҪ�Ƿ�����
};

}

#endif // _SHA1_H_
