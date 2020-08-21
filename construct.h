#ifndef CONSTRUCT_H
#define CONSTRUCT_H
#include <new>
#include "typetraits.h"
namespace TinySTL{
    template<class T1, class T2>
    inline void construct(T1*p,const T2& value)
    {
        new(p)T1(value); //placement new调用拷贝构造函数T1:T1(value)
    }
    template<class T>
    inline void destroy(T *p)
    {
        p->~T();//直接调用析构函数
    }

    //_true_type和_false_type是类名，相当于数据类型，但是没有形参，意思是传入哪个类型的参数就用哪个函数

    //如果是_true_type说明析构函数不重要，因此什么都不做
    template<class ForwardIterator>
	inline void _destroy(ForwardIterator first, ForwardIterator last, _true_type){}

    //如果是_true_type说明析构函数是重要的，将[first,last)前闭后开区间内的所有对象都析构掉
	template<class ForwardIterator>
	inline void _destroy(ForwardIterator first, ForwardIterator last, _false_type){
		for (; first != last; ++first){
			destroy(&*first);
		}
	}
    //判断元素的数值类别的析构函数是不是重要的（通过萃取器），再调用上面两个中的一个
	template<class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last){
        /*
        其中typename是告诉编译器A::a_type是个类型，而不是A这个类里面的变量或函数。
        不申明，编译器就可以把a_type当一个变量。
        */
		typedef typename _type_traits<ForwardIterator>::is_POD_type is_POD_type;
		_destroy(first, last, is_POD_type());
	}
}
#endif