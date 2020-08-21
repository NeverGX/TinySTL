#ifndef VECTOR_H
#define VECTOR_H
#include "memory.h"
/*
vector迭代器的几种失效的情况：
1.当插入（push_back）一个元素后，end操作返回的迭代器肯定失效。
2.当插入(push_back)一个元素后，capacity返回值与没有插入元素之前相比有改变，则需要重新加载整个容器，此时first和end操 作返回的迭代器都会失效。
3.当进行删除操作（erase，pop_back）后，指向删除点的迭代器全部失效；指向删除点后面的元素的迭代器也将全部失效。
*/
namespace TinySTL{
template<typename T,typename Alloc=alloc>
class vector
{
public:

	typedef T value_type;
	typedef value_type* pointer;
	typedef value_type* iterator;
	typedef value_type& reference;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;

protected:
	typedef simple_alloc<value_type,Alloc> data_allocator;//这里已经标明value_type
	iterator start;//表示目前使用空间的头
	iterator finish;//表示目前使用空间的尾
	iterator end_of_storage;//表示目前可用空间的尾巴

	void fill_initialize(size_type n,const T& val)
	{
		start = allocate_and_fill(n,val);
		finish = start + n;
		end_of_storage = finish;//注意这里的end_of_storage等于finish
	}

	iterator allocate_and_fill(size_type n,const T& val)
	{
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result,n,val);
		return result;
	}

	void deallocate()
	{
		if(start)
		{
			data_allocator::deallocate(start,end_of_storage-start);
		}
	}

	void insert_aux(iterator position,const T& value);

public:
    //构造函数
	vector():start(0),finish(0),end_of_storage(0){}
	vector(size_type n,const T& val){fill_initialize(n,val);};
	vector(int n,const T& val){fill_initialize(n,val);};;
	vector(long n,const T& val){fill_initialize(n,val);};;
	explicit vector(size_type n){fill_initialize(n,value_type());};;
	~vector()
	{
		destroy(start,finish);
		deallocate();
	}
    
    //拷贝构造函数
    vector(vector& vec)
    {
        start = data_allocator::allocate(vec.capacity());
        finish = start+vec.size();
        end_of_storage= start+vec.capacity();
        uninitialized_copy(vec.begin(),vec.end(),start);
    }
    //赋值操作符重载
    //这里放回引用的原因是，使其能够连续赋值，如i=j=0这种类型
    vector<T,Alloc>& operator=(vector& vec)
    {
        //先释放掉原来的空间
        destroy(start,finish);
        deallocate();
        //再开辟空间拷贝赋值
        start = data_allocator::allocate(vec.capacity());
        finish = start+vec.size();
        end_of_storage= start+vec.capacity();
        uninitialized_copy(vec.begin(),vec.end(),start);
        return *this;
    }
    //
//iterator
	iterator begin() {return start;}
	iterator end() {return finish;}
//capacity
	size_type size() {return (end()-begin());}
	size_type max_size(){return (end_of_storage-begin());}
	size_type capacity(){return (end_of_storage-begin());}
	bool empty(){return begin()==end();}
//elememt access
	reference operator[](size_type n){return *(begin()+n);}
	reference front(){return *begin();}
	reference back() {return *(end()-1);}
	reference at(size_type n) {return *(begin()+n);}
//modify

    //vector中push_back和emplace_back的区别是什么？？？
	void push_back (const T& val)
    {
        /*  
            //const&右值引用
            const int& val = 12;
            //会被编译器翻译成:
            int __internal_unique_name = 12;
            const int& val = __internal_unique_name;
            //因此有着额外开销
        */
        if(finish!=end_of_storage)//前闭后开原则
        {
            construct(finish,val);
            finish++;
        }
        else
        {
            insert_aux(end(),val);
        }
    }
	void pop_back()//前闭后开原则
    {
        finish--;
        destroy(finish);
    }
//erase
/*
	iterator erase (iterator position)
	{
		if(position!=end())
		{
			copy(position+1,finish,position);//将[position+1,finish)间的元素拷贝到position上
		}
		--finish;
		destroy(finish);
		return position;
	}
	iterator erase (iterator first, iterator last)
	{
		iterator i= copy(last,finish,first);
		destroy(i,finish);
		finish = finish - (last - first);
		return first;

	}
	void clear()
	{
		destroy(begin(),end());
		start = 0;
		finish =0;
		end_of_storage=0;
	}
*/   
	void swap (vector& x)
	{
		iterator temp_start = start;
		iterator temp_finish = finish;
		iterator temp_end_of_storage = end_of_storage;

		start = x.begin();
		finish = x.end();
		end_of_storage = x.end_of_storage;

		x.start = temp_start;
		x.finish = temp_finish;
		x.end_of_storage = temp_end_of_storage;
	}
    
    /*
	void resize(size_type n,const T& val)////将现有的元素个数调至n个，多了就删，少则补，补充的值为val
	{
		if(n < size())
		{
			erase(begin()+n,end());
		}
		else
		{
			insert(end(),n-size(),val);
		}
	}
    */
};

template<typename T,typename Alloc>
void vector<T,Alloc>::insert_aux(iterator position,const T& value)
{
    //为什么这里重复判断两次finish!=end_of_storage，因为不只有push_back调用insert_aux,当其他函数调用使也得判断
    /*
    if(finish!=end_of_storage)
	{
		construct(finish,*(finish - 1));
		++finish;
		copy_backward(position,finish-2,finish-1);
		*position = value;
	}
    */

    //已无备用空间
    const size_type old_size = size();
    const size_type new_size = (old_size == 0?1:2*old_size);
    //如果原大小为0，则配置1个元素大小
    //如果原大小不为0，则配置原大小的两倍
    //前半段用来放置原数据，后半段用来放置新数据

    iterator new_start = data_allocator::allocate(new_size);
    iterator new_finish =uninitialized_copy(start,position,new_start);//若是push_back调用，此时position=end
    construct(new_finish,value);
    ++new_finish;

    //拷贝安插点后面的内存，因此此函数也可能被insert调用
    uninitialized_copy(position,finish,new_finish);

    destroy(begin(),end());
    deallocate();

    start = new_start;
    finish = new_finish;
    end_of_storage = start+new_size;
	

}

	
}

#endif

