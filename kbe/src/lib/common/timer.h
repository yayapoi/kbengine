// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com
#ifndef KBE_TIMER_H
#define KBE_TIMER_H

#include "common/common.h"
#include "common/timestamp.h"
#include "helper/debug_helper.h"

namespace KBEngine
{
    // TimerHandle �����ڹ���ʱ�����
    class TimersBase;
    class TimeBase;

    class TimerHandle
    {
    public:
        // ���캯������ʼ����ʱ�����
        explicit TimerHandle(TimeBase * pTime = NULL) : pTime_( pTime ) {}

        // ȡ����ʱ��
        void cancel();

        // �����ʱ���������ȡ����ʱ��
        void clearWithoutCancel() { pTime_ = NULL; }

        // ��鶨ʱ���Ƿ�������
        bool isSet() const { return pTime_ != NULL; }

        // �Ƚ����� TimerHandle �Ƿ����
        friend bool operator==( TimerHandle h1, TimerHandle h2 );

        // ��ȡ������ TimeBase ����
        TimeBase * time() const { return pTime_; }

    private:
        // ������ TimeBase ����ָ��
        TimeBase * pTime_;
    };

    // �Ƚ����� TimerHandle �Ƿ����
    inline bool operator==( TimerHandle h1, TimerHandle h2 )
    {
        return h1.pTime_ == h2.pTime_;
    }

    // ����̳�����ӿ�,������ timer->handleTimeout �¼�
    class TimerHandler
    {
    public:
        // ���캯��
        TimerHandler() : numTimesRegistered_( 0 ) {}

        // ����������ȷ�����ж�ʱ����ȡ��
        virtual ~TimerHandler()
        {
            KBE_ASSERT( numTimesRegistered_ == 0 );
        };

        // ����ʱ�¼�
        virtual void handleTimeout(TimerHandle handle, void * pUser) = 0;

    protected:
        // ����ʱ�����ͷ�ʱ����
        virtual void onRelease( TimerHandle handle, void * pUser ) {
        }

    private:
        // ��Ԫ�� TimeBase
        friend class TimeBase;

        // ���Ӷ�ʱ��ע�����
        void incTimerRegisterCount() { ++numTimesRegistered_; }

        // ���ٶ�ʱ��ע�����
        void decTimerRegisterCount() { --numTimesRegistered_; }

        // �ͷŶ�ʱ��
        void release( TimerHandle handle, void * pUser )
        {
            this->decTimerRegisterCount();
            this->onRelease( handle, pUser );
        }

        // ��ʱ��ע�����
        int numTimesRegistered_;
    };

    // TimeBase ���Ƕ�ʱ���Ļ�����
    class TimeBase
    {
    public:
        // ���캯��
        TimeBase(TimersBase &owner, TimerHandler* pHandler, void* pUserData);

        // ��������
        virtual ~TimeBase() {}

        // ȡ����ʱ��
        void cancel();

        // ��ȡ�û�����
        void * getUserData() const { return pUserData_; }

        // ��鶨ʱ���Ƿ���ȡ��
        bool isCancelled() const { return state_ == TIME_CANCELLED; }

        // ��鶨ʱ���Ƿ�����ִ��
        bool isExecuting() const { return state_ == TIME_EXECUTING; }

    protected:
        // ��ʱ��״̬ö��
        enum TimeState
        {
            TIME_PENDING,
            TIME_EXECUTING,
            TIME_CANCELLED
        };

        // ��ʱ�������� TimersBase ����
        TimersBase& owner_;

        // ��ʱ���������
        TimerHandler * pHandler_;

        // �û�����
        void *pUserData_;

        // ��ʱ��״̬
        TimeState state_;
    };

    // TimersBase ���Ƕ�ʱ�����ϵĻ���
    class TimersBase
    {
    public:
        // ����ʱ����ȡ��ʱ����
        virtual void onCancel() = 0;
    };

    // TimersT ���ǻ���ʱ����Ķ�ʱ������ģ����
    template<class TIME_STAMP>
    class TimersT : public TimersBase
    {
    public:
        // ʱ�������
        typedef TIME_STAMP TimeStamp;

        // ���캯��
        TimersT();

        // ��������
        virtual ~TimersT();

        // ��ȡ��ʱ�����ϵĴ�С
        inline uint32 size() const { return timeQueue_.size(); }

        // ��鶨ʱ�������Ƿ�Ϊ��
        inline bool empty() const { return timeQueue_.empty(); }

        // ����ʱ��
        int process(TimeStamp now);

        // ��鶨ʱ������Ƿ�Ϸ�
        bool legal( TimerHandle handle ) const;

        // ��ȡ��һ�����ڵ�ʱ��
        TIME_STAMP nextExp( TimeStamp now ) const;

        // ��ն�ʱ������
        void clear( bool shouldCallCancel = true );

        // ��ȡ��ʱ����Ϣ
        bool getTimerInfo( TimerHandle handle, 
                           TimeStamp& time, 
                           TimeStamp& interval,
                           void *& pUser ) const;

        // ��Ӷ�ʱ��
        TimerHandle add(TimeStamp startTime, TimeStamp interval,
                        TimerHandler* pHandler, void * pUser);

    private:
        // ��ʱ����������
        typedef std::vector<KBEngine::TimeBase *> Container;
        Container container_;

        // �����ȡ���Ķ�ʱ��
        void purgeCancelledTimes();

        // ����ʱ����ȡ��ʱ����
        void onCancel();

        // Time ���Ƕ�ʱ���ľ���ʵ��
        class Time : public TimeBase
        {
        public:
            // ���캯��
            Time( TimersBase & owner, TimeStamp startTime, TimeStamp interval,
                  TimerHandler * pHandler, void * pUser );

            // ��ȡ��ʱ���ĵ���ʱ��
            TIME_STAMP time() const { return time_; }

            // ��ȡ��ʱ���ļ��ʱ��
            TIME_STAMP interval() const { return interval_; }

            // ������ʱ��
            void triggerTimer();

        private:
            // ��ʱ���ĵ���ʱ��
            TimeStamp time_;

            // ��ʱ���ļ��ʱ��
            TimeStamp interval_;

            // ���ÿ������캯���͸�ֵ������
            Time( const Time & );
            Time & operator=( const Time & );
        };

        // �Ƚ����࣬�������ȶ�������
        class Comparator
        {
        public:
            // �Ƚ�������ʱ���ĵ���ʱ��
            bool operator()(const Time* a, const Time* b)
            {
                return a->time() > b->time();
            }
        };

        // ���ȶ����࣬���ڹ���ʱ��
        class PriorityQueue
        {
        public:
            // ��ʱ����������
            typedef std::vector<Time *> Container;

            // ��ʱ������������ֵ����
            typedef typename Container::value_type value_type;

            // ��ʱ�����������Ĵ�С����
            typedef typename Container::size_type size_type;

            // ������ȶ����Ƿ�Ϊ��
            bool empty() const { return container_.empty(); }

            // ��ȡ���ȶ��еĴ�С
            size_type size() const { return container_.size(); }

            // ��ȡ���ȶ��еĶ���Ԫ��
            const value_type & top() const { return container_.front(); }

            // �����ȶ��������Ԫ��
            void push( const value_type & x )
            {
                container_.push_back( x );
                std::push_heap( container_.begin(), container_.end(),
                                Comparator() );
            }

            // �����ȶ������Ƴ�����Ԫ��
            void pop()
            {
                std::pop_heap( container_.begin(), container_.end(), Comparator() );
                container_.pop_back();
            }

            // �����ȶ����в���ȫ���Ƴ����һ��Ԫ��
            Time * unsafePopBack()
            {
                Time * pTime = container_.back();
                container_.pop_back();
                return pTime;
            }

            // ��ȡ���ȶ��е�����
            Container & container() { return container_; }

            // ���¹������ȶ���
            void make_heap()
            {
                std::make_heap( container_.begin(), container_.end(),
                                Comparator() );
            }

        private:
            // ��ʱ����������
            Container container_;
        };

        // ���ȶ��ж���
        PriorityQueue timeQueue_;

        // ��ǰ����Ķ�ʱ���ڵ�
        Time * pProcessingNode_;

        // �ϴδ����ʱ��
        TimeStamp lastProcessTime_;

        // ��ȡ���Ķ�ʱ������
        int numCancelled_;

        // ���ÿ������캯���͸�ֵ������
        TimersT( const TimersT & );
        TimersT & operator=( const TimersT & );
    };

    // ������� uint32 �Ķ�ʱ������
    typedef TimersT<uint32> Timers;

    // ������� uint64 �Ķ�ʱ������
    typedef TimersT<uint64> Timers64;
}

// ����������������
#include "timer.inl"

#endif // KBE_TIMER_H