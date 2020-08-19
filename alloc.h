#ifndef ALLOC_H
#define ALLOC_H

#include <cstdlib>

namespace TinySTL{

	/*
	**空间配置器，以字节数为单位分配
	**内部使用
	*/
	class alloc{
	private:
		enum { ALIGN = 8};//小型区块的上调边界
		enum { MAXBYTES = 128};//小型区块的上限，超过的区块由malloc分配
		enum { NFREELISTS = (MAXBYTES / ALIGN)};//free-lists的个数
	private:
		//free-lists的节点构造
		struct obj{
			struct obj *next;
		};
        //16个free-lists,指针数组
		static obj *free_list[NFREELISTS];
	private:
		//将bytes上调至8的倍数
		static size_t ROUND_UP(size_t bytes){
			return ((bytes + ALIGN - 1) & ~(ALIGN - 1));//~取反
		}
		//根据区块大小，决定使用第n号free-list，n从0开始计算
		static size_t FREELIST_INDEX(size_t bytes){
			return (((bytes)+ALIGN - 1)/ALIGN - 1);
		}
		//返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
		static void *refill(size_t n);

		//配置一大块空间，可容纳nobjs个大小为size的区块
		//如果配置nobjs个区块有所不便，nobjs可能会降低
		static char *chunk_alloc(size_t size, int& nobjs);

        //chunk allocation state
        static char *start_free;//内存池起始位置
		static char *end_free;//内存池结束位置
		static size_t heap_size;//已经分配的堆大小

	public:
		static void *allocate(size_t bytes);
		static void deallocate(void *ptr, size_t bytes);
		//static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
	};

    char *alloc::start_free = 0;
	char *alloc::end_free = 0;
	size_t alloc::heap_size = 0;
	alloc::obj *alloc::free_list[alloc::NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
    
    void* alloc::allocate(size_t bytes)
    {
        //如果请求的字节数大于128，调用第一级配置器，即malloc
        if( bytes > MAXBYTES )
        {
            return malloc(bytes);
        }
        size_t index = FREELIST_INDEX(bytes);
        obj** my_free_list=free_list+index;
        obj* result=*my_free_list;
        if(result) //my_free_list还有空间给我们
        {
            *my_free_list=result->next;
            return result;
        }
        else//my_free_list没有足够的空间，需要从内存池里面取空间
        {
            return refill(ROUND_UP(bytes));
        }
    }
    void alloc::deallocate(void *ptr,size_t bytes)
    {
        if(bytes>MAXBYTES)
        {
            free(ptr);
            return;
        }
        size_t index = FREELIST_INDEX(bytes);
        obj* my_free_list=free_list[index];
        obj* tmp=static_cast<obj*>(ptr);
        tmp->next=my_free_list;
        free_list[index]=tmp;
    }
    void* alloc::refill(size_t bytes)//充值
    {
        int nobjs = 20;//nobjs表示取几个块
		//尝试获取nobjs个区块作为freelist的新节点
		char *chunk = chunk_alloc(bytes, nobjs);//注意这里的nobjs是引用传递

        obj *cur_obj = 0;
		obj *result = 0;
		if (nobjs == 1)//取出的空间只够一个对象使用
        {
			return chunk;
		}
        result = cur_obj = (obj *)(chunk);
        //将m_free_list各个节点串联起来
        for (int i=0;i<nobjs-1; ++i)//注意这里是nobjs-1
        {
            cur_obj->next=(obj*)((char*)cur_obj+bytes);
            cur_obj=cur_obj->next;
        }
        cur_obj->next=NULL;
        size_t index = FREELIST_INDEX(bytes);
        free_list[index]=result->next;
        return result;
    }

    //假设bytes已经上调为8的倍数
	char* alloc::chunk_alloc(size_t bytes, int& nobjs){
		char *result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes){//内存池剩余空间完全满足需要
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes){//内存池剩余空间不能完全满足需要，但足够供应一个或以上的区块
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else{//内存池剩余空间连一个区块的大小都无法提供
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

			//内存池还有一些零头，尝试分配给适当的freelist
			//如果都是分配的8的倍数的内存，若有剩余，肯定大于8比特
			if (bytes_left > 0){
				//首先寻找适当的freelist
				obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
				//调整freelist将内存池的残余空间编入
				((obj *)start_free)->next = *my_free_list;
				*my_free_list = (obj *)start_free;
			}

			//分配堆空间来补充内存池
			start_free = (char *)malloc(bytes_to_get);
			if (!start_free){//如果malloc失败
				obj **my_free_list = 0, *p = 0;
				for (int i = bytes; i <= MAXBYTES; i += ALIGN){
					//注意这里i是从bytes开始
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0){//freelist内还有未用区块
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;
						//递归调用自己，为了修正nobjs
						return chunk_alloc(bytes, nobjs);
					}
				}

				end_free = 0;//如果出现意外，山穷水尽，到处都没有内存可用了

				/*
				代码省略，调用第一级配置器，抛出异常
				*/
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			//递归调用自己，为了修正nobjs
			return chunk_alloc(bytes, nobjs);
		}
	}

}
#endif