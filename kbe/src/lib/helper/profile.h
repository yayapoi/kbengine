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
 * @brief 用于管理一组性能分析值的类
 */
class ProfileGroup
{
public:
	ProfileGroup(std::string name = "default"); //  构造函数，默认参数为"default"
	~ProfileGroup();

	typedef std::vector<ProfileVal*> PROFILEVALS; // 性能分析值列表类型
    typedef PROFILEVALS::iterator iterator;       // 迭代器类型

	static ProfileGroup & defaultGroup();

	// 获取当前组的性能分析值栈
	PROFILEVALS & stack() { return stack_; }
	// 添加一个性能分析值到组中
	void add(ProfileVal * pVal);

	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }

	// 获取当前组的运行时间性能分析值
    ProfileVal * pRunningTime() { return profiles_[0]; }
    // 获取当前组的运行时间性能分析值（常量版本）
    const ProfileVal * pRunningTime() const { return profiles_[0]; }
    // 获取当前组的运行时间
    TimeStamp runningTime() const;

	INLINE const char* name() const;

	bool initializeWatcher();

	static void finalise(void);

	// 获取当前组的所有性能分析值
	INLINE const ProfileGroup::PROFILEVALS& profiles() const;

private:
	PROFILEVALS profiles_; // 当前组的性能分析值列表
    PROFILEVALS stack_;    // 当前组的性能分析值栈
    std::string name_;     // 组的名称
};

/**
 * @brief 用于记录性能分析数据的类
 */
class ProfileVal
{
public:
	ProfileVal(std::string name, ProfileGroup * pGroup = NULL);
	~ProfileVal();

	bool initializeWatcher();

	void start() //  开始计时
	{
		if(!initWatcher_ && count_ > 10) //  如果没有初始化监视器且处理次数大于10次，则初始化监视器
			initializeWatcher();

		TimeStamp now = timestamp(); //  获取当前时间

		// 记录第几次处理
		if (inProgress_++ == 0)
			lastTime_ = now;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack(); //  获取ProfileGroup的栈

		// 如果栈中有对象则自己是从上一个ProfileVal函数进入调用的
		// 我们可以在此得到上一个函数进入到本函数之前的一段时间片
		// 然后将其加入到sumIntTime_
		if (!stack.empty()){
			ProfileVal & profile = *stack.back();
			profile.lastIntTime_ = now - profile.lastIntTime_; //  计算上一个函数进入到本函数之前的一段时间片
			profile.sumIntTime_ += profile.lastIntTime_; //  将这段时间片加入到sumIntTime_
		}

		// 将自己压栈
		stack.push_back(this);

		// 记录开始时间
		lastIntTime_ = now;
	}

	void stop(uint32 qty = 0)
	{
		TimeStamp now = timestamp();

		// 如果为0则表明自己是调用栈的产生着
		// 在此我们可以得到这个函数总共耗费的时间
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

		// 得到本函数所耗费的时间
		lastIntTime_ = now - lastIntTime_;
		sumIntTime_ += lastIntTime_;

		// 我们需要在此重设上一个函数中的profile对象的最后一次内部时间
		// 使其能够在start时正确得到自调用完本函数之后进入下一个profile函数中时所消耗
		// 的时间片段
		if (!stack.empty())
			stack.back()->lastIntTime_ = now;
	}

	// 结束记录性能分析数据（带文件名和行号）
	INLINE bool stop( const char * filename, int lineNum, uint32 qty = 0);

	INLINE bool running() const; //  判断计时是否正在进行

	INLINE const char * c_str() const; //  获取性能分析值的字符串表示

	INLINE TimeStamp sumTime() const; //  返回计时器的总时间
	INLINE TimeStamp lastIntTime() const ; //  返回计时器的最后一次内部时间
	INLINE TimeStamp sumIntTime() const ; //  返回计时器的总内部时间
	INLINE TimeStamp lastTime() const; //  返回计时器的最后一次时间

	INLINE double lastTimeInSeconds() const; //  返回计时器的最后一次时间，以秒为单位
	INLINE double sumTimeInSeconds() const ; //  返回计时器的总时间，以秒为单位
	INLINE double lastIntTimeInSeconds() const ; //  返回计时器的最后一次内部时间，以秒为单位
	INLINE double sumIntTimeInSeconds() const; //  返回计时器的总内部时间，以秒为单位

	INLINE const char* name() const;

	INLINE uint32 count() const;

	INLINE bool isTooLong() const;	//检查是否超时

	static void setWarningPeriod(TimeStamp warningPeriod) { warningPeriod_ = warningPeriod; } //  设置警告周期

	// 名称
	std::string		name_;

	// ProfileGroup指针
	ProfileGroup * pProfileGroup_;

	// startd后的时间.
	TimeStamp		lastTime_;

	// count_次的总时间
	TimeStamp		sumTime_;

	// 记录最后一次内部时间片
	TimeStamp		lastIntTime_;

	// count_次内部总时间
	TimeStamp		sumIntTime_;

	uint32			lastQuantity_;	///< The last value passed into stop.上次传递给 stop 的值
	uint32			sumQuantity_;	///< The total of all values passed into stop.传递给 stop 的所有值的总和
	uint32			count_;			///< The number of times stop has been called.调用 stop 的次数

	// 记录第几次处理, 如递归等
	int				inProgress_;

	bool			initWatcher_;

private:
	static TimeStamp warningPeriod_;	// 警告周期

};

/**
 * @brief 用于自动管理性能分析值的类
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
	ProfileVal& profile_; // 要管理的性能分析值
    const char* filename_; // 文件名
    int lineNum_; // 行号
};

#define START_PROFILE( PROFILE ) PROFILE.start();

#define STOP_PROFILE( PROFILE )	PROFILE.stop( __FILE__, __LINE__ );

// 宏定义，用于自动管理性能分析值
#define AUTO_SCOPED_PROFILE( NAME )												\
	static ProfileVal _localProfile( NAME );									\
	ScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );

// 宏定义，用于手动创建并管理性能分析值
#define SCOPED_PROFILE(PROFILE)													\
	ScopedProfile PROFILE##_scopedProfile(PROFILE, __FILE__, __LINE__);

// 宏定义，用于结束记录性能分析数据并检查是否超时
#define STOP_PROFILE_WITH_CHECK( PROFILE )										\
	if (PROFILE.stop( __FILE__, __LINE__ ))

// 宏定义，用于结束记录性能分析数据并传递数据
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )									\
	PROFILE.stop( __FILE__, __LINE__ , DATA );

// 由此可得到系统profile时间
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


