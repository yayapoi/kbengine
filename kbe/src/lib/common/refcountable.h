// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


/*
	引用计数实现类

	使用方法:
		class AA:public RefCountable
		{
		public:
			AA(){}
			~AA(){ printf("析构"); }
		};
		
		--------------------------------------------
		AA* a = new AA();
		RefCountedPtr<AA>* s = new RefCountedPtr<AA>(a);
		RefCountedPtr<AA>* s1 = new RefCountedPtr<AA>(a);
		
		int i = (*s)->getRefCount();
		
		delete s;
		delete s1;
		
		执行结果:
			析构
*/
#ifndef KBE_REFCOUNTABLE_H
#define KBE_REFCOUNTABLE_H
	
#include "common.h"
	
namespace KBEngine{

class RefCountable 
{
public:
	inline void incRef(void) const
	{
		++refCount_;	// 增加引用计数
	}

	inline void decRef(void) const
	{
		
		int currRef = --refCount_;	// 减少引用计数
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");	// 确保引用计数不为负
		if (0 >= currRef)
			onRefOver();											// 引用结束了
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<RefCountable*>(this);	// 删除对象
	}

	void setRefCount(int n)
	{
		refCount_ = n;	// 设置引用计数
	}

	int getRefCount(void) const 
	{ 
		return refCount_; // 获取当前引用计数
	}

protected:
	RefCountable(void) : refCount_(0) 
	{
	}

	virtual ~RefCountable(void) 
	{ 
		assert(0 == refCount_ && "RefCountable:currRef maybe a error!");	// 确保析构时引用计数为0
	}

protected:
	volatile mutable long refCount_;	// 引用计数，volatile确保多线程安全，mutable允许const方法修改
};

#if KBE_PLATFORM == PLATFORM_WIN32
class SafeRefCountable 
{
public:
	inline void incRef(void) const
	{
		::InterlockedIncrement(&refCount_);	// 使用Windows的原子操作增加引用计数
	}

	inline void decRef(void) const
	{
		
		long currRef =::InterlockedDecrement(&refCount_);	// 使用Windows的原子操作减少引用计数
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");	// 确保引用计数不为负
		if (0 >= currRef)
			onRefOver();											// 引用结束了
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<SafeRefCountable*>(this);	// 删除对象
	}

	void setRefCount(long n)
	{
		InterlockedExchange((long *)&refCount_, n);	// 使用Windows的原子操作设置引用计数
	}

	int getRefCount(void) const 
	{ 
		return InterlockedExchange((long *)&refCount_, refCount_);	// 使用Windows的原子操作获取引用计数
	}

protected:
	SafeRefCountable(void) : refCount_(0) 
	{
	}

	virtual ~SafeRefCountable(void) 
	{ 
		assert(0 == refCount_ && "SafeRefCountable:currRef maybe a error!"); 
	}

protected:
	volatile mutable long refCount_;	// 引用计数，volatile确保多线程安全，mutable允许const方法修改
};
#else
class SafeRefCountable 
{
public:
	inline void incRef(void) const
	{
		__asm__ volatile (
			"lock addl $1, %0"	// 使用汇编指令增加引用计数
			:						// 没有输出
			: "m"	(this->refCount_) 	// input: this->count_
			: "memory" 				// clobbers memory,影响内存
		);
	}

	inline void decRef(void) const
	{
		
		long currRef = intDecRef();	// 调用私有方法减少引用计数
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");
		if (0 >= currRef)
			onRefOver();	// 引用计数为0时调用onRefOver
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<SafeRefCountable*>(this);	// 删除对象
	}

	void setRefCount(long n)
	{
		//InterlockedExchange((long *)&refCount_, n);
	}

	int getRefCount(void) const 
	{ 
		//return InterlockedExchange((long *)&refCount_, refCount_);
		return refCount_;
	}

protected:
	SafeRefCountable(void) : refCount_(0) 
	{
	}

	virtual ~SafeRefCountable(void) 
	{ 
		assert(0 == refCount_ && "SafeRefCountable:currRef maybe a error!"); 
	}

protected:
	volatile mutable long refCount_;
private:
	/**
	 *	This private method decreases the reference count by 1.
	 */
	inline int intDecRef() const
	{
		int ret;
		__asm__ volatile (
			"mov $-1, %0  \n\t"	// 将-1移动到寄存器
			"lock xadd %0, %1"	// 使用锁前缀和xadd指令减少引用计数
			: "=&a"	(ret)				// output only and early clobber, 输出寄存器
			: "m"	(this->refCount_)		// input (memory)
			: "memory"
		);
		return ret;	// 返回减少后的引用计数
	}
};
#endif

template<class T>
class RefCountedPtr 
{
public:
	RefCountedPtr(T* ptr):ptr_(ptr) 
	{
		if (ptr_)
			ptr_->addRef();	// 如果指针不为空，增加引用计数
	}

	RefCountedPtr(RefCountedPtr<T>* refptr):ptr_(refptr->getObject()) 
	{
		if (ptr_)
			ptr_->addRef();
	}
	
	~RefCountedPtr(void) 
	{
		if (0 != ptr_)
			ptr_->decRef();	// 如果指针不为空，减少引用计数
	}

	T& operator*() const 
	{ 
		return *ptr_; 	// 解引用操作
	}

	T* operator->() const 
	{ 
		return (&**this);	// 成员访问操作
	}

	T* getObject(void) const 
	{ 
		return ptr_;	// 获取原始指针
	}

private:
	T* ptr_;	// 存储的指针
};

}
#endif // KBE_REFCOUNTABLE_H
