#ifndef _SHA1_H_
#define _SHA1_H_

// 命名空间 KBEngine 的定义，用于封装类和函数
namespace KBEngine {

// 定义了一个名为 KBE_SHA1 的类，用于实现 SHA-1 哈希算法
class KBE_SHA1
{
public:

    // 构造函数，用于初始化对象
    KBE_SHA1();

    // 虚析构函数，确保派生类的析构函数被正确调用
    virtual ~KBE_SHA1();

    // 重新初始化类，使其可以重新计算新的哈希值
    void Reset();

    // 返回消息摘要，结果存储在传入的数组中
    bool Result(unsigned *message_digest_array);

    // 提供输入数据给 SHA1 算法，参数为无符号字符数组和长度
    void Input(const unsigned char *message_array, unsigned length);
    
    // 提供输入数据给 SHA1 算法，参数为字符数组和长度
    void Input(const char *message_array, unsigned length);
    
    // 提供单个无符号字符作为输入数据
    void Input(unsigned char message_element);
    
    // 提供单个字符作为输入数据
    void Input(char message_element);
    
    // 重载 << 运算符，允许使用流式输入方式提供字符数组
    KBE_SHA1& operator<<(const char *message_array);
    
    // 重载 << 运算符，允许使用流式输入方式提供无符号字符数组
    KBE_SHA1& operator<<(const unsigned char *message_array);
    
    // 重载 << 运算符，允许使用流式输入方式提供单个字符
    KBE_SHA1& operator<<(const char message_element);
    
    // 重载 << 运算符，允许使用流式输入方式提供单个无符号字符
    KBE_SHA1& operator<<(const unsigned char message_element);

private:

    // 处理下一个 512 位的消息块
    void ProcessMessageBlock();

    // 对当前消息块进行填充，使其达到 512 位
    void PadMessage();

    // 执行循环左移操作
    inline unsigned CircularShift(int bits, unsigned word);

    unsigned H[5];                      // 消息摘要缓冲区

    unsigned Length_Low;                // 消息长度（低 32 位）
    unsigned Length_High;               // 消息长度（高 32 位）

    unsigned char Message_Block[64];    // 512 位的消息块
    int Message_Block_Index;            // 消息块数组的索引

    bool Computed;                      // 消息摘要是否已计算完成
    bool Corrupted;                     // 消息摘要是否已损坏
};

}

#endif // _SHA1_H_
