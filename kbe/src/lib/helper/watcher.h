// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com
#ifndef KBE_HELPER_WATCHER_H
#define KBE_HELPER_WATCHER_H

#include "common/common.h"
#include "helper/debug_helper.h"
#include "common/smartpointer.h"
#include "common/singleton.h"
#include "common/memorystream.h"

namespace KBEngine
{

namespace Network{
}

typedef uint16 WATCHER_ID; //  监控器ID类型
typedef uint8 WATCHER_VALUE_TYPE; //  监控器值类型

#define WATCHER_VALUE_TYPE_UNKNOWN				0
#define WATCHER_VALUE_TYPE_UINT8				1
#define WATCHER_VALUE_TYPE_UINT16				2
#define WATCHER_VALUE_TYPE_UINT32				3
#define WATCHER_VALUE_TYPE_UINT64				4
#define WATCHER_VALUE_TYPE_INT8					5
#define WATCHER_VALUE_TYPE_INT16				6
#define WATCHER_VALUE_TYPE_INT32				7
#define WATCHER_VALUE_TYPE_INT64				8
#define WATCHER_VALUE_TYPE_FLOAT				9
#define WATCHER_VALUE_TYPE_DOUBLE				10
#define WATCHER_VALUE_TYPE_CHAR					11
#define WATCHER_VALUE_TYPE_STRING				12
#define WATCHER_VALUE_TYPE_BOOL					13
#define WATCHER_VALUE_TYPE_COMPONENT_TYPE		14

class Watchers;

/*
	watcher基础对象
	用于监控和记录对象状态的基类。它提供了一些方法来设置和获取对象的路径、名称、ID、值
*/
class WatcherObject
{
public:
	WatcherObject(std::string path);
	
	virtual ~WatcherObject();

	// 将初始化信息添加到内存流中
    virtual void addToInitStream(MemoryStream* s) {};

    // 将当前信息添加到内存流中
    virtual void addToStream(MemoryStream* s) {};

    // 获取和设置ID
    WATCHER_ID id() { return id_; }
    void id(WATCHER_ID i) { id_ = i; }

    // 获取路径和名称
    const char* path() { return path_.c_str(); }
    const char* name() { return name_.c_str(); }

    // 模板方法，获取值类型
    template <class T>
    WATCHER_VALUE_TYPE type() const { return WATCHER_VALUE_TYPE_UNKNOWN; }

	// 更新内存流中的值
    template <class T>
    void updateStream(MemoryStream* s) {
        T v;
        (*s) >> v;
        strval_ = StringConv::val2str(v);
    }

    // 增加和减少观察者数量
    void addWitness() { numWitness_++; }
    void delWitness() { numWitness_--; }

    // 获取观察者数量
    int32 numWitness() const { return numWitness_; }

    // 获取字符串值
    const char* str() const { return strval_.c_str(); }
    
    const char* getValue() { return strval_.c_str(); }

    // 获取值类型
    virtual WATCHER_VALUE_TYPE getType() { return WATCHER_VALUE_TYPE_UNKNOWN; }

protected:
	std::string path_, name_, strval_; // 路径、名称和字符串值
    WATCHER_ID id_; // ID
    int32 numWitness_; // 观察者数量
};

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<uint8>() const
{
	return WATCHER_VALUE_TYPE_UINT8;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<uint16>() const
{
	return WATCHER_VALUE_TYPE_UINT16;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<uint32>() const
{
	return WATCHER_VALUE_TYPE_UINT32;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<uint64>() const
{
	return WATCHER_VALUE_TYPE_UINT64;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<int8>() const
{
	return WATCHER_VALUE_TYPE_INT8;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<int16>() const
{
	return WATCHER_VALUE_TYPE_INT16;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<int32>() const
{
	return WATCHER_VALUE_TYPE_INT32;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<int64>() const
{
	return WATCHER_VALUE_TYPE_INT64;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<float>() const
{
	return WATCHER_VALUE_TYPE_FLOAT;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<double>() const
{
	return WATCHER_VALUE_TYPE_DOUBLE;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<bool>() const
{
	return WATCHER_VALUE_TYPE_BOOL;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<char*>() const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<const char*>() const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<std::string>() const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<std::string&>() const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<const std::string&>() const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE WatcherObject::type<COMPONENT_TYPE>() const
{
	return WATCHER_VALUE_TYPE_COMPONENT_TYPE;
}

template <>
inline void WatcherObject::updateStream<COMPONENT_TYPE>(MemoryStream* s)
{
	COMPONENT_TYPE v;
	(*s) >> v;
	strval_ = COMPONENT_NAME_EX(v);
}

template <>
inline void WatcherObject::updateStream<std::string>(MemoryStream* s)
{
	(*s) >> strval_;
}

template <>
inline void WatcherObject::updateStream<std::string&>(MemoryStream* s)
{
	updateStream<std::string>(s);
}

template <>
inline void WatcherObject::updateStream<const std::string&>(MemoryStream* s)
{
	updateStream<std::string>(s);
}

template <>
inline void WatcherObject::updateStream<char*>(MemoryStream* s)
{
	updateStream<std::string>(s);
}

template <>
inline void WatcherObject::updateStream<const char*>(MemoryStream* s)
{
	updateStream<std::string>(s);
}


/*
	watcher: 直接监视一个值
*/
template <class T>
class WatcherValue : public WatcherObject
{
public:
	WatcherValue(std::string path, const T& pVal):
	WatcherObject(path),
	watchVal_(pVal)
	{
	}
	
	virtual ~WatcherValue(){
	}
	
	// 将初始化信息添加到内存流中
    void addToInitStream(MemoryStream* s) {
        (*s) << path() << name() << id_ << type<T>() << watchVal_;
    }

    // 将当前信息添加到内存流中
    void addToStream(MemoryStream* s) {
        (*s) << id_ << watchVal_;
    }

    // 获取值
    T getValue() { return watchVal_; }

    // 获取值类型
    WATCHER_VALUE_TYPE getType() { return type<T>(); }

protected:
    const T& watchVal_; // 监视的值
    T val_; // 值副本
};

/*
	watcher: 监视一个方法返回的值
*/
template <class RETURN_TYPE>
class WatcherFunction : public WatcherObject
{
public:
	typedef RETURN_TYPE(*FUNC)();

	WatcherFunction(std::string path, RETURN_TYPE (*func)()):
	WatcherObject(path),
	func_(func)
	{
	}
	
	virtual ~WatcherFunction(){
	}
	
	// 将初始化信息添加到内存流中
    void addToInitStream(MemoryStream* s) {
        (*s) << path() << name() << id_ << type<RETURN_TYPE>() << (*func_)();
    }

    // 将当前信息添加到内存流中
    void addToStream(MemoryStream* s) {
        (*s) << id_ << (*func_)();
    }

    // 获取值
    RETURN_TYPE getValue() { return (*func_)(); }

    // 获取值类型
    WATCHER_VALUE_TYPE getType() { return type<RETURN_TYPE>(); }

protected:
    FUNC func_; // 函数指针
};

/*
	watcher: 监视一个成员函数返回的值
*/
template <class RETURN_TYPE, class OBJ_TYPE>
class WatcherMethod : public WatcherObject
{
public:
	typedef RETURN_TYPE(OBJ_TYPE::*FUNC)();

	WatcherMethod(std::string path, OBJ_TYPE* obj, RETURN_TYPE (OBJ_TYPE::*func)()):
	WatcherObject(path),
	func_(func),
	obj_(obj)
	{
	}
	
	virtual ~WatcherMethod(){
	}
	
	// 将初始化信息添加到内存流中
    void addToInitStream(MemoryStream* s) {
        (*s) << path() << name() << id_ << type<RETURN_TYPE>() << (obj_->*func_)();
    }

    // 将当前信息添加到内存流中
    void addToStream(MemoryStream* s) {
        (*s) << id_ << (obj_->*func_)();
    }

    // 获取值
    RETURN_TYPE getValue() { return (obj_->*func_)(); }

    // 获取值类型
    WATCHER_VALUE_TYPE getType() { return type<RETURN_TYPE>(); }

protected:
    FUNC func_; // 成员函数指针
    OBJ_TYPE* obj_; // 对象指针
};

/**
 * 监视一个常量成员函数返回值的watcher
 */
template <class RETURN_TYPE, class OBJ_TYPE>
class WatcherMethodConst : public WatcherObject
{
public:
	typedef RETURN_TYPE(OBJ_TYPE::*FUNC)() const;

	WatcherMethodConst(std::string path, OBJ_TYPE* obj, RETURN_TYPE (OBJ_TYPE::*func)() const):
	WatcherObject(path),
	func_(func),
	obj_(obj)
	{
	}
	
	virtual ~WatcherMethodConst(){
	}
	
	// 将初始化信息添加到内存流中
    void addToInitStream(MemoryStream* s) {
        RETURN_TYPE v = (obj_->*func_)();
        (*s) << path() << name() << id_ << type<RETURN_TYPE>() << v;
    }

    // 将当前信息添加到内存流中
    void addToStream(MemoryStream* s) {
        (*s) << id_ << (obj_->*func_)();
    }

    // 获取值
    RETURN_TYPE getValue() { return (obj_->*func_)(); }

    // 获取值类型
    WATCHER_VALUE_TYPE getType() { return type<RETURN_TYPE>(); }

protected:
    FUNC func_; // 常量成员函数指针
    OBJ_TYPE* obj_; // 对象指针
};

/*
	watcher管理器
*/
class Watchers
{
public:
	Watchers();
	~Watchers();
	
	void clear();

	// 获取根watcher管理器
    static Watchers& rootWatchers();

    // 将所有watcher信息添加到内存流中
    void addToStream(MemoryStream* s);

    // 从内存流中读取watcher信息
    void readWatchers(MemoryStream* s);
    // 定义watcher映射类型
    typedef KBEUnordered_map<std::string, KBEShared_ptr< WatcherObject > > WATCHER_MAP;

    // 添加watcher
    bool addWatcher(const std::string& path, WatcherObject* pwo);
    // 删除watcher
    bool delWatcher(const std::string& name);
    // 检查是否存在指定名称的watcher
    bool hasWatcher(const std::string& name);

    // 获取指定名称的watcher
    KBEShared_ptr< WatcherObject > getWatcher(const std::string& name);

    // 更新内存流中的watcher信息
    void updateStream(MemoryStream* s);

    // 获取watcher映射
    WATCHER_MAP& watcherObjs() { return watcherObjs_; }

protected:
    WATCHER_MAP watcherObjs_; // watcher映射
};

/**
 * watcher路径管理器
 */
class WatcherPaths
{
public:
	WatcherPaths();
	~WatcherPaths();

	void clear();

	// 获取根watcher路径管理器
    static WatcherPaths& root();
    // 最终化watcher路径管理器
    static bool finalise();

    // 将所有watcher路径信息添加到内存流中
    void addToStream(MemoryStream* s);

    // 从内存流中读取watcher路径信息
    void readWatchers(std::string path, MemoryStream* s);
    // 读取子路径信息
    void readChildPaths(std::string srcPath, std::string path, MemoryStream* s);
    // 获取目录路径
    void dirPath(std::string path, std::vector<std::string>& vec);

    // 定义watcher路径映射类型
    typedef KBEUnordered_map<std::string, KBEShared_ptr<WatcherPaths> > WATCHER_PATHS;

    // 添加watcher
    bool addWatcher(std::string path, WatcherObject* pwo);
    // 内部添加watcher
    bool _addWatcher(std::string path, WatcherObject* pwo);

	// 从内存流中添加watcher
    WatcherObject* addWatcherFromStream(std::string path, std::string name, 
        WATCHER_ID wid, WATCHER_VALUE_TYPE wtype, MemoryStream* s);

    // 删除watcher
    bool delWatcher(const std::string& fullpath);
    // 检查是否存在指定路径的watcher
    bool hasWatcher(const std::string& fullpath);
    // 获取指定路径的watcher
    KBEShared_ptr< WatcherObject > getWatcher(const std::string& fullpath);

    // 更新内存流中的watcher路径信息
    void updateStream(MemoryStream* s);

    // 获取watcher管理器
    Watchers& watchers() { return watchers_; }
    // 获取watcher路径映射
    WATCHER_PATHS& watcherPaths() { return watcherPaths_; }

protected:
    WATCHER_PATHS watcherPaths_; // watcher路径映射
    Watchers watchers_; // watcher管理器
};

/**
	用于监视一个值
	int32 a = 1;
	addWatcher("a", a);

	AAA a;
	a.x = 2;
	addWatcher("a", axxxx.x);
*/
template <class TYPE> 
inline WatcherObject* addWatcher(std::string path, const TYPE& type)
{
	path = std::string("root/") + path;
	WatcherValue<TYPE>* pwo = new WatcherValue<TYPE>(path, type);
	WatcherPaths::root().addWatcher(path, pwo);
	return pwo;
};

/**
	用于监视一个函数的返回值

	int32 func(){}

	addWatcher("func", &func);
*/
template <class RETURN_TYPE> 
inline WatcherObject* addWatcher(std::string path, RETURN_TYPE (*func)())
{
	path = std::string("root/") + path;
	WatcherFunction<RETURN_TYPE>* pwo = new WatcherFunction<RETURN_TYPE>(path, func);
	WatcherPaths::root().addWatcher(path, pwo);
	return pwo;
};

/**
	用于监视一个成员函数的返回值

	int32 AAA::func(){}
	AAA a;

	addWatcher("func", &a, &AAA::func);
*/
template <class RETURN_TYPE, class OBJ_TYPE> 
inline WatcherObject* addWatcher(std::string path, OBJ_TYPE* obj, RETURN_TYPE (OBJ_TYPE::*func)())
{
	path = std::string("root/") + path;
	WatcherMethod<RETURN_TYPE, OBJ_TYPE>* pwo = new WatcherMethod<RETURN_TYPE, OBJ_TYPE>(path, obj, func);
	WatcherPaths::root().addWatcher(path, pwo);
	return pwo;
};

/**
 * 用于监视一个常量成员函数的返回值
 * 示例：
 * int32 AAA::func() const {}
 * AAA a;
 * addWatcher("func", &a, &AAA::func);
 */
template <class RETURN_TYPE, class OBJ_TYPE> 
inline WatcherObject* addWatcher(std::string path, OBJ_TYPE* obj, RETURN_TYPE (OBJ_TYPE::*func)() const)
{
	path = std::string("root/") + path;
	WatcherMethodConst<RETURN_TYPE, OBJ_TYPE>* pwo = new WatcherMethodConst<RETURN_TYPE, OBJ_TYPE>(path, obj, func);
	WatcherPaths::root().addWatcher(path, pwo);
	return pwo;
};

#ifdef ENABLE_WATCHERS
	#define WATCH_OBJECT addWatcher
	#define WATCH_FINALIZE WatcherPaths::root().clear();
#else
	inline WatcherObject* __addWatcher(...){ return NULL; }
	#define WATCH_OBJECT __addWatcher
	#define WATCH_FINALIZE
#endif


}
#endif
