// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com
#ifndef KBE_TIMER_H
#define KBE_TIMER_H

#include "common/common.h"
#include "common/timestamp.h"
#include "helper/debug_helper.h"

namespace KBEngine
{
    // TimerHandle 类用于管理定时器句柄
    class TimersBase;
    class TimeBase;

    class TimerHandle
    {
    public:
        // 构造函数，初始化定时器句柄
        explicit TimerHandle(TimeBase * pTime = NULL) : pTime_( pTime ) {}

        // 取消定时器
        void cancel();

        // 清除定时器句柄而不取消定时器
        void clearWithoutCancel() { pTime_ = NULL; }

        // 检查定时器是否已设置
        bool isSet() const { return pTime_ != NULL; }

        // 比较两个 TimerHandle 是否相等
        friend bool operator==( TimerHandle h1, TimerHandle h2 );

        // 获取关联的 TimeBase 对象
        TimeBase * time() const { return pTime_; }

    private:
        // 关联的 TimeBase 对象指针
        TimeBase * pTime_;
    };

    // 比较两个 TimerHandle 是否相等
    inline bool operator==( TimerHandle h1, TimerHandle h2 )
    {
        return h1.pTime_ == h2.pTime_;
    }

    // 必须继承这个接口,来接收 timer->handleTimeout 事件
    class TimerHandler
    {
    public:
        // 构造函数
        TimerHandler() : numTimesRegistered_( 0 ) {}

        // 析构函数，确保所有定时器已取消
        virtual ~TimerHandler()
        {
            KBE_ASSERT( numTimesRegistered_ == 0 );
        };

        // 处理超时事件
        virtual void handleTimeout(TimerHandle handle, void * pUser) = 0;

    protected:
        // 当定时器被释放时调用
        virtual void onRelease( TimerHandle handle, void * pUser ) {
        }

    private:
        // 友元类 TimeBase
        friend class TimeBase;

        // 增加定时器注册计数
        void incTimerRegisterCount() { ++numTimesRegistered_; }

        // 减少定时器注册计数
        void decTimerRegisterCount() { --numTimesRegistered_; }

        // 释放定时器
        void release( TimerHandle handle, void * pUser )
        {
            this->decTimerRegisterCount();
            this->onRelease( handle, pUser );
        }

        // 定时器注册计数
        int numTimesRegistered_;
    };

    // TimeBase 类是定时器的基本类
    class TimeBase
    {
    public:
        // 构造函数
        TimeBase(TimersBase &owner, TimerHandler* pHandler, void* pUserData);

        // 析构函数
        virtual ~TimeBase() {}

        // 取消定时器
        void cancel();

        // 获取用户数据
        void * getUserData() const { return pUserData_; }

        // 检查定时器是否已取消
        bool isCancelled() const { return state_ == TIME_CANCELLED; }

        // 检查定时器是否正在执行
        bool isExecuting() const { return state_ == TIME_EXECUTING; }

    protected:
        // 定时器状态枚举
        enum TimeState
        {
            TIME_PENDING,
            TIME_EXECUTING,
            TIME_CANCELLED
        };

        // 定时器所属的 TimersBase 对象
        TimersBase& owner_;

        // 定时器处理对象
        TimerHandler * pHandler_;

        // 用户数据
        void *pUserData_;

        // 定时器状态
        TimeState state_;
    };

    // TimersBase 类是定时器集合的基类
    class TimersBase
    {
    public:
        // 当定时器被取消时调用
        virtual void onCancel() = 0;
    };

    // TimersT 类是基于时间戳的定时器集合模板类
    template<class TIME_STAMP>
    class TimersT : public TimersBase
    {
    public:
        // 时间戳类型
        typedef TIME_STAMP TimeStamp;

        // 构造函数
        TimersT();

        // 析构函数
        virtual ~TimersT();

        // 获取定时器集合的大小
        inline uint32 size() const { return timeQueue_.size(); }

        // 检查定时器集合是否为空
        inline bool empty() const { return timeQueue_.empty(); }

        // 处理定时器
        int process(TimeStamp now);

        // 检查定时器句柄是否合法
        bool legal( TimerHandle handle ) const;

        // 获取下一个到期的时间
        TIME_STAMP nextExp( TimeStamp now ) const;

        // 清空定时器集合
        void clear( bool shouldCallCancel = true );

        // 获取定时器信息
        bool getTimerInfo( TimerHandle handle, 
                           TimeStamp& time, 
                           TimeStamp& interval,
                           void *& pUser ) const;

        // 添加定时器
        TimerHandle add(TimeStamp startTime, TimeStamp interval,
                        TimerHandler* pHandler, void * pUser);

    private:
        // 定时器集合容器
        typedef std::vector<KBEngine::TimeBase *> Container;
        Container container_;

        // 清除已取消的定时器
        void purgeCancelledTimes();

        // 当定时器被取消时调用
        void onCancel();

        // Time 类是定时器的具体实现
        class Time : public TimeBase
        {
        public:
            // 构造函数
            Time( TimersBase & owner, TimeStamp startTime, TimeStamp interval,
                  TimerHandler * pHandler, void * pUser );

            // 获取定时器的到期时间
            TIME_STAMP time() const { return time_; }

            // 获取定时器的间隔时间
            TIME_STAMP interval() const { return interval_; }

            // 触发定时器
            void triggerTimer();

        private:
            // 定时器的到期时间
            TimeStamp time_;

            // 定时器的间隔时间
            TimeStamp interval_;

            // 禁用拷贝构造函数和赋值操作符
            Time( const Time & );
            Time & operator=( const Time & );
        };

        // 比较器类，用于优先队列排序
        class Comparator
        {
        public:
            // 比较两个定时器的到期时间
            bool operator()(const Time* a, const Time* b)
            {
                return a->time() > b->time();
            }
        };

        // 优先队列类，用于管理定时器
        class PriorityQueue
        {
        public:
            // 定时器集合容器
            typedef std::vector<Time *> Container;

            // 定时器集合容器的值类型
            typedef typename Container::value_type value_type;

            // 定时器集合容器的大小类型
            typedef typename Container::size_type size_type;

            // 检查优先队列是否为空
            bool empty() const { return container_.empty(); }

            // 获取优先队列的大小
            size_type size() const { return container_.size(); }

            // 获取优先队列的顶部元素
            const value_type & top() const { return container_.front(); }

            // 向优先队列中添加元素
            void push( const value_type & x )
            {
                container_.push_back( x );
                std::push_heap( container_.begin(), container_.end(),
                                Comparator() );
            }

            // 从优先队列中移除顶部元素
            void pop()
            {
                std::pop_heap( container_.begin(), container_.end(), Comparator() );
                container_.pop_back();
            }

            // 从优先队列中不安全地移除最后一个元素
            Time * unsafePopBack()
            {
                Time * pTime = container_.back();
                container_.pop_back();
                return pTime;
            }

            // 获取优先队列的容器
            Container & container() { return container_; }

            // 重新构建优先队列
            void make_heap()
            {
                std::make_heap( container_.begin(), container_.end(),
                                Comparator() );
            }

        private:
            // 定时器集合容器
            Container container_;
        };

        // 优先队列对象
        PriorityQueue timeQueue_;

        // 当前处理的定时器节点
        Time * pProcessingNode_;

        // 上次处理的时间
        TimeStamp lastProcessTime_;

        // 已取消的定时器数量
        int numCancelled_;

        // 禁用拷贝构造函数和赋值操作符
        TimersT( const TimersT & );
        TimersT & operator=( const TimersT & );
    };

    // 定义基于 uint32 的定时器集合
    typedef TimersT<uint32> Timers;

    // 定义基于 uint64 的定时器集合
    typedef TimersT<uint64> Timers64;
}

// 包含内联函数定义
#include "timer.inl"

#endif // KBE_TIMER_H