#ifndef MEMORY_H
#define MEMORY_H
#include "alloc.h"
#include "construct.h"
#include "uninitialized.h"

namespace TinySTL{

template<class T, class Alloc>
class simple_alloc{
public:
    static T* allocate(size_t n)
    {
        return 0==n?0:(T*) Alloc::allocate(sizeof(T)*n);
    }
    static T* allocate()
    {
        return (T*) Alloc::allocate(sizeof(T));
    }
    static void deallocate(T *p,size_t n)
    {
        if(0!=n) Alloc::deallocate(p,sizeof(T)*n);
    }
     static void deallocate(T *p)
    {
        Alloc::deallocate(p,sizeof(T));
    }
};

}
#endif