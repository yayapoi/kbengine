// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

/*
*/
#ifndef KBE_MEMORY_MALLOC_H
#define KBE_MEMORY_MALLOC_H

#ifdef USE_JEMALLOC
#include "jemalloc/jemalloc.h"  //jemalloc 是一个高性能的内存分配器
#endif

//#define USE_KBE_MALLOC				    //启用自定义的内存分配器
//#define KBE_MALLOC_ALIGN			16      //用于指定内存对齐的方式（例如 16 字节对齐）

#ifdef USE_KBE_MALLOC
#include "nedmalloc/nedmalloc.h"            //nedmalloc 是另一个高性能的内存分配器

#if KBE_PLATFORM == PLATFORM_WIN32          //是否为 Windows 32 位。如果是，并且在调试模式下编译，则链接 nedmalloc_d.lib 库；否则，链接 nedmalloc.lib 库
#ifdef _DEBUG
#pragma comment (lib, "nedmalloc_d.lib")
#else
#pragma comment (lib, "nedmalloc.lib")
#endif
#endif

inline void* operator new(size_t size)
{
#if KBE_MALLOC_ALIGN == 0
    return nedalloc::nedpmalloc(0, size);
#else
    return nedalloc::nedpmemalign(0, KBE_MALLOC_ALIGN, size);   //分配对齐的内存
#endif
}

inline void* operator new[](size_t size)    //这个函数重载了全局的 new[] 操作符。作用与上面的 operator new 类似，但用于数组的内存分配
{
#if KBE_MALLOC_ALIGN == 0
    return nedalloc::nedpmalloc(0, size);
#else
    return nedalloc::nedpmemalign(0, KBE_MALLOC_ALIGN, size);
#endif
}

inline void  operator delete(void* p)
{
    nedalloc::nedpfree(0, p);
}

inline void  operator delete[](void* p)
{
    nedalloc::nedpfree(0, p);
}

#endif
#endif
