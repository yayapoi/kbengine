// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


/*
	���ü���ʵ����

	ʹ�÷���:
		class AA:public RefCountable
		{
		public:
			AA(){}
			~AA(){ printf("����"); }
		};
		
		--------------------------------------------
		AA* a = new AA();
		RefCountedPtr<AA>* s = new RefCountedPtr<AA>(a);
		RefCountedPtr<AA>* s1 = new RefCountedPtr<AA>(a);
		
		int i = (*s)->getRefCount();
		
		delete s;
		delete s1;
		
		ִ�н��:
			����
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
		++refCount_;	// �������ü���
	}

	inline void decRef(void) const
	{
		
		int currRef = --refCount_;	// �������ü���
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");	// ȷ�����ü�����Ϊ��
		if (0 >= currRef)
			onRefOver();											// ���ý�����
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<RefCountable*>(this);	// ɾ������
	}

	void setRefCount(int n)
	{
		refCount_ = n;	// �������ü���
	}

	int getRefCount(void) const 
	{ 
		return refCount_; // ��ȡ��ǰ���ü���
	}

protected:
	RefCountable(void) : refCount_(0) 
	{
	}

	virtual ~RefCountable(void) 
	{ 
		assert(0 == refCount_ && "RefCountable:currRef maybe a error!");	// ȷ������ʱ���ü���Ϊ0
	}

protected:
	volatile mutable long refCount_;	// ���ü�����volatileȷ�����̰߳�ȫ��mutable����const�����޸�
};

#if KBE_PLATFORM == PLATFORM_WIN32
class SafeRefCountable 
{
public:
	inline void incRef(void) const
	{
		::InterlockedIncrement(&refCount_);	// ʹ��Windows��ԭ�Ӳ����������ü���
	}

	inline void decRef(void) const
	{
		
		long currRef =::InterlockedDecrement(&refCount_);	// ʹ��Windows��ԭ�Ӳ����������ü���
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");	// ȷ�����ü�����Ϊ��
		if (0 >= currRef)
			onRefOver();											// ���ý�����
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<SafeRefCountable*>(this);	// ɾ������
	}

	void setRefCount(long n)
	{
		InterlockedExchange((long *)&refCount_, n);	// ʹ��Windows��ԭ�Ӳ����������ü���
	}

	int getRefCount(void) const 
	{ 
		return InterlockedExchange((long *)&refCount_, refCount_);	// ʹ��Windows��ԭ�Ӳ�����ȡ���ü���
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
	volatile mutable long refCount_;	// ���ü�����volatileȷ�����̰߳�ȫ��mutable����const�����޸�
};
#else
class SafeRefCountable 
{
public:
	inline void incRef(void) const
	{
		__asm__ volatile (
			"lock addl $1, %0"	// ʹ�û��ָ���������ü���
			:						// û�����
			: "m"	(this->refCount_) 	// input: this->count_
			: "memory" 				// clobbers memory,Ӱ���ڴ�
		);
	}

	inline void decRef(void) const
	{
		
		long currRef = intDecRef();	// ����˽�з����������ü���
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");
		if (0 >= currRef)
			onRefOver();	// ���ü���Ϊ0ʱ����onRefOver
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<SafeRefCountable*>(this);	// ɾ������
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
			"mov $-1, %0  \n\t"	// ��-1�ƶ����Ĵ���
			"lock xadd %0, %1"	// ʹ����ǰ׺��xaddָ��������ü���
			: "=&a"	(ret)				// output only and early clobber, ����Ĵ���
			: "m"	(this->refCount_)		// input (memory)
			: "memory"
		);
		return ret;	// ���ؼ��ٺ�����ü���
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
			ptr_->addRef();	// ���ָ�벻Ϊ�գ��������ü���
	}

	RefCountedPtr(RefCountedPtr<T>* refptr):ptr_(refptr->getObject()) 
	{
		if (ptr_)
			ptr_->addRef();
	}
	
	~RefCountedPtr(void) 
	{
		if (0 != ptr_)
			ptr_->decRef();	// ���ָ�벻Ϊ�գ��������ü���
	}

	T& operator*() const 
	{ 
		return *ptr_; 	// �����ò���
	}

	T* operator->() const 
	{ 
		return (&**this);	// ��Ա���ʲ���
	}

	T* getObject(void) const 
	{ 
		return ptr_;	// ��ȡԭʼָ��
	}

private:
	T* ptr_;	// �洢��ָ��
};

}
#endif // KBE_REFCOUNTABLE_H
