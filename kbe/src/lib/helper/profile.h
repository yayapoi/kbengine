// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBENGINE_PROFILE_H
#define KBENGINE_PROFILE_H

#include "debug_helper.h"
#include "common/common.h"
#include "common/timer.h"
#include "common/timestamp.h"

namespace KBEngine
{

#if ENABLE_WATCHERS

class ProfileVal;

/**
 * @brief ���ڹ���һ�����ܷ���ֵ����
 */
class ProfileGroup
{
public:
	ProfileGroup(std::string name = "default"); //  ���캯����Ĭ�ϲ���Ϊ"default"
	~ProfileGroup();

	typedef std::vector<ProfileVal*> PROFILEVALS; // ���ܷ���ֵ�б�����
    typedef PROFILEVALS::iterator iterator;       // ����������

	static ProfileGroup & defaultGroup();

	// ��ȡ��ǰ������ܷ���ֵջ
	PROFILEVALS & stack() { return stack_; }
	// ���һ�����ܷ���ֵ������
	void add(ProfileVal * pVal);

	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }

	// ��ȡ��ǰ�������ʱ�����ܷ���ֵ
    ProfileVal * pRunningTime() { return profiles_[0]; }
    // ��ȡ��ǰ�������ʱ�����ܷ���ֵ�������汾��
    const ProfileVal * pRunningTime() const { return profiles_[0]; }
    // ��ȡ��ǰ�������ʱ��
    TimeStamp runningTime() const;

	INLINE const char* name() const;

	bool initializeWatcher();

	static void finalise(void);

	// ��ȡ��ǰ����������ܷ���ֵ
	INLINE const ProfileGroup::PROFILEVALS& profiles() const;

private:
	PROFILEVALS profiles_; // ��ǰ������ܷ���ֵ�б�
    PROFILEVALS stack_;    // ��ǰ������ܷ���ֵջ
    std::string name_;     // �������
};

/**
 * @brief ���ڼ�¼���ܷ������ݵ���
 */
class ProfileVal
{
public:
	ProfileVal(std::string name, ProfileGroup * pGroup = NULL);
	~ProfileVal();

	bool initializeWatcher();

	void start() //  ��ʼ��ʱ
	{
		if(!initWatcher_ && count_ > 10) //  ���û�г�ʼ���������Ҵ����������10�Σ����ʼ��������
			initializeWatcher();

		TimeStamp now = timestamp(); //  ��ȡ��ǰʱ��

		// ��¼�ڼ��δ���
		if (inProgress_++ == 0)
			lastTime_ = now;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack(); //  ��ȡProfileGroup��ջ

		// ���ջ���ж������Լ��Ǵ���һ��ProfileVal����������õ�
		// ���ǿ����ڴ˵õ���һ���������뵽������֮ǰ��һ��ʱ��Ƭ
		// Ȼ������뵽sumIntTime_
		if (!stack.empty()){
			ProfileVal & profile = *stack.back();
			profile.lastIntTime_ = now - profile.lastIntTime_; //  ������һ���������뵽������֮ǰ��һ��ʱ��Ƭ
			profile.sumIntTime_ += profile.lastIntTime_; //  �����ʱ��Ƭ���뵽sumIntTime_
		}

		// ���Լ�ѹջ
		stack.push_back(this);

		// ��¼��ʼʱ��
		lastIntTime_ = now;
	}

	void stop(uint32 qty = 0)
	{
		TimeStamp now = timestamp();

		// ���Ϊ0������Լ��ǵ���ջ�Ĳ�����
		// �ڴ����ǿ��Եõ���������ܹ��ķѵ�ʱ��
		if (--inProgress_ == 0){
			lastTime_ = now - lastTime_;
			sumTime_ += lastTime_;
		}

		lastQuantity_ = qty;
		sumQuantity_ += qty;
		++count_;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack();
		KBE_ASSERT( stack.back() == this );

		stack.pop_back();

		// �õ����������ķѵ�ʱ��
		lastIntTime_ = now - lastIntTime_;
		sumIntTime_ += lastIntTime_;

		// ������Ҫ�ڴ�������һ�������е�profile��������һ���ڲ�ʱ��
		// ʹ���ܹ���startʱ��ȷ�õ��Ե����걾����֮�������һ��profile������ʱ������
		// ��ʱ��Ƭ��
		if (!stack.empty())
			stack.back()->lastIntTime_ = now;
	}

	// ������¼���ܷ������ݣ����ļ������кţ�
	INLINE bool stop( const char * filename, int lineNum, uint32 qty = 0);

	INLINE bool running() const; //  �жϼ�ʱ�Ƿ����ڽ���

	INLINE const char * c_str() const; //  ��ȡ���ܷ���ֵ���ַ�����ʾ

	INLINE TimeStamp sumTime() const; //  ���ؼ�ʱ������ʱ��
	INLINE TimeStamp lastIntTime() const ; //  ���ؼ�ʱ�������һ���ڲ�ʱ��
	INLINE TimeStamp sumIntTime() const ; //  ���ؼ�ʱ�������ڲ�ʱ��
	INLINE TimeStamp lastTime() const; //  ���ؼ�ʱ�������һ��ʱ��

	INLINE double lastTimeInSeconds() const; //  ���ؼ�ʱ�������һ��ʱ�䣬����Ϊ��λ
	INLINE double sumTimeInSeconds() const ; //  ���ؼ�ʱ������ʱ�䣬����Ϊ��λ
	INLINE double lastIntTimeInSeconds() const ; //  ���ؼ�ʱ�������һ���ڲ�ʱ�䣬����Ϊ��λ
	INLINE double sumIntTimeInSeconds() const; //  ���ؼ�ʱ�������ڲ�ʱ�䣬����Ϊ��λ

	INLINE const char* name() const;

	INLINE uint32 count() const;

	INLINE bool isTooLong() const;	//����Ƿ�ʱ

	static void setWarningPeriod(TimeStamp warningPeriod) { warningPeriod_ = warningPeriod; } //  ���þ�������

	// ����
	std::string		name_;

	// ProfileGroupָ��
	ProfileGroup * pProfileGroup_;

	// startd���ʱ��.
	TimeStamp		lastTime_;

	// count_�ε���ʱ��
	TimeStamp		sumTime_;

	// ��¼���һ���ڲ�ʱ��Ƭ
	TimeStamp		lastIntTime_;

	// count_���ڲ���ʱ��
	TimeStamp		sumIntTime_;

	uint32			lastQuantity_;	///< The last value passed into stop.�ϴδ��ݸ� stop ��ֵ
	uint32			sumQuantity_;	///< The total of all values passed into stop.���ݸ� stop ������ֵ���ܺ�
	uint32			count_;			///< The number of times stop has been called.���� stop �Ĵ���

	// ��¼�ڼ��δ���, ��ݹ��
	int				inProgress_;

	bool			initWatcher_;

private:
	static TimeStamp warningPeriod_;	// ��������

};

/**
 * @brief �����Զ��������ܷ���ֵ����
 */
class ScopedProfile
{
public:
	ScopedProfile(ProfileVal & profile, const char * filename, int lineNum) :
		profile_(profile),
		filename_(filename),
		lineNum_(lineNum)
	{
		profile_.start();
	}

	~ScopedProfile()
	{
		profile_.stop(filename_, lineNum_);
	}

private:
	ProfileVal& profile_; // Ҫ��������ܷ���ֵ
    const char* filename_; // �ļ���
    int lineNum_; // �к�
};

#define START_PROFILE( PROFILE ) PROFILE.start();

#define STOP_PROFILE( PROFILE )	PROFILE.stop( __FILE__, __LINE__ );

// �궨�壬�����Զ��������ܷ���ֵ
#define AUTO_SCOPED_PROFILE( NAME )												\
	static ProfileVal _localProfile( NAME );									\
	ScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );

// �궨�壬�����ֶ��������������ܷ���ֵ
#define SCOPED_PROFILE(PROFILE)													\
	ScopedProfile PROFILE##_scopedProfile(PROFILE, __FILE__, __LINE__);

// �궨�壬���ڽ�����¼���ܷ������ݲ�����Ƿ�ʱ
#define STOP_PROFILE_WITH_CHECK( PROFILE )										\
	if (PROFILE.stop( __FILE__, __LINE__ ))

// �궨�壬���ڽ�����¼���ܷ������ݲ���������
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )									\
	PROFILE.stop( __FILE__, __LINE__ , DATA );

// �ɴ˿ɵõ�ϵͳprofileʱ��
uint64 runningTime();

#else

#define AUTO_SCOPED_PROFILE( NAME )
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )
#define STOP_PROFILE_WITH_CHECK( PROFILE )
#define SCOPED_PROFILE(PROFILE)
#define STOP_PROFILE( PROFILE )
#define START_PROFILE( PROFILE )

uint64 runningTime(){
	return 0;
}

#endif //ENABLE_WATCHERS


}

#ifdef CODE_INLINE
#include "profile.inl"
#endif

#endif // KBENGINE_PROFILE_H


