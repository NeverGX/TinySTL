#ifndef _UNINITIALIZED_H
#define _UNINITIALIZED_H
#include "typetraits.h"
#include "construct.h"
#include "iterator.h"

namespace TinySTL{

    /*****************************************************************************************/
    // uninitialized_copy
    // 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
    /*****************************************************************************************/

	template<class InputIterator, class ForwardIterator>
	ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
		typedef typename _type_traits<iterator_traits<InputIterator>::value_type>::is_POD_type isPODType;
		return _uninitialized_copy_aux(first, last, result, isPODType());
	}

	template<class InputIterator, class ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,ForwardIterator result, _true_type){
		memcpy(result, first, (last - first) * sizeof(*first));
		return result + (last - first);
	}
	template<class InputIterator, class ForwardIterator>
	ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last,ForwardIterator result, _false_type){
		int i = 0;
		for (; first != last; ++first, ++i){
			construct((result + i), *first);
		}
		return (result + i);
	}

	/*****************************************************************************************/
    // uninitialized_fill
    // 在 [first, last) 区间内填充元素值
    /*****************************************************************************************/

	template<class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value){
		typedef typename _type_traits<T>::is_POD_type isPODType;
		_uninitialized_fill_aux(first, last, value, isPODType());
	}
	template<class ForwardIterator, class T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& value, _true_type){
		fill(first, last, value);
	}
	template<class ForwardIterator, class T>
	void _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,const T& value, _false_type){
		for (; first != last; ++first){
			construct(first, value);
		}
	}

	/*****************************************************************************************/
    // uninitialized_fill_n
    // 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
    /*****************************************************************************************/

	template<class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
		typedef typename _type_traits<T>::is_POD_type isPODType;
		return _uninitialized_fill_n_aux(first, n, x, isPODType());
	}
	template<class ForwardIterator, class Size, class T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type){
		return fill_n(first, n, x);
	}
	template<class ForwardIterator, class Size, class T>
	ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type){
		int i = 0;
		for (; i != n; ++i){
			construct((T*)(first + i), x);
		}
		return (first + i);
	}

}
#endif