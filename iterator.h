#ifndef _ITERATOR_H_
#define _ITERATOR_H_
#include<stddef.h>
namespace TinySTL{
    //五种迭代器类型
	struct input_iterator_tag{};//只读迭代器(read only)这种迭代器所指的对象，不允许外界改变，
	struct output_iterator_tag{};//只写迭代器(write only)
	struct forward_iterator_tag :public input_iterator_tag {};//前向迭代器，允许写入型算法这此种迭代器所形成的区间上进行读写操作
	struct bidirectional_iterator_tag :public forward_iterator_tag {};//双向迭代器
	struct random_access_iterator_tag :public bidirectional_iterator_tag {};//随机访问迭代器

    /*
    为了符合规范，任何迭代器都应该提供五个内嵌相应类别，以利于萃取.
    因此为了避免写代码时漏写，自行开发的迭代器最好继承下面这个类
    */
	template<class Category, class T, class Distance = ptrdiff_t,class Pointer = T*, class Reference = T&>
	struct iterator //从上面这个迭代器的模板中可以看出下面五种属性的代表着什么！！！！！
	{
		typedef Category	iterator_category;//迭代器类型
		typedef T			value_type;//迭代器所指对象的类型
		typedef Distance	difference_type;//difference_type用来表示两个迭代器之间的距离
		typedef Pointer		pointer;
		typedef Reference	reference;
	};

	/*
	template <class T, class Distance> 
	struct input_iterator
	{
		typedef input_iterator_tag	iterator_category;
		typedef T					value_type;
		typedef Distance			difference_type;
		typedef T*					pointer;
		typedef T&					reference;
	};

	struct output_iterator
	{
		typedef output_iterator_tag iterator_category;
		typedef void                value_type;
		typedef void                difference_type;
		typedef void                pointer;
		typedef void                reference;
	};

	template <class T, class Distance> 
	struct forward_iterator
	{
		typedef forward_iterator_tag	iterator_category;
		typedef T						value_type;
		typedef Distance				difference_type;
		typedef T*						pointer;
		typedef T&						reference;
	};

	template <class T, class Distance> 
	struct bidirectional_iterator
	{
		typedef bidirectional_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef Distance					difference_type;
		typedef T*							pointer;
		typedef T&							reference;
	};

	template <class T, class Distance> 
	struct random_access_iterator
	{
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef Distance					difference_type;
		typedef T*							pointer;
		typedef T&							reference;
	};
	*/

     //迭代器萃取器
	template<class Iterator>
	struct iterator_traits
	{
		typedef typename Iterator::iterator_category	iterator_category;//迭代器类型
		typedef typename Iterator::value_type			value_type;//迭代器所指对象的类型
		typedef typename Iterator::difference_type		difference_type;//difference_type用来表示两个迭代器之间的距离
		typedef typename Iterator::pointer				pointer;
		typedef typename Iterator::reference 			reference;
	};

    //针对原生指针(native pointer)而设计的traits偏特化版本
	template<class T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag 	iterator_category;
		typedef T 							value_type;
		typedef ptrdiff_t 					difference_type;
		typedef T*							pointer;
		typedef T& 							reference;
	};
    //针对const原生指针(native pointer)而设计的traits偏特化版本
	template<class T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag 	iterator_category;
		typedef T 							value_type;
		typedef ptrdiff_t 					difference_type;
		typedef const T*					pointer;
		typedef const T& 					reference;
	};

	//typename告诉编译器iterator_traits<Iterator>::iterator_category是一个类型而不是一个成员
    //这个函数可以很方便的决定某个迭代器category
	template<typename Iterator>
	typename iterator_traits<Iterator>::iterator_category
	iterator_category(const Iterator&)
	{
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return category();

	}
	//这个函数可以很方便的决定某个迭代器distance type
	template<typename Iterator>
	typename iterator_traits<Iterator>::difference_type*
	difference_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}
	//这个函数可以很方便的决定某个迭代器value type
	template<typename Iterator>
	typename iterator_traits<Iterator>::value_type*
	value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	
}

#endif