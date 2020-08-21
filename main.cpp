#include <cstdlib>
#include "memory.h"
#include <iostream>
#include "vector.h"
#include <vector>

int main()
{
    /*
    TinySTL::simple_alloc<long,TinySTL::alloc> alloctor;
    for(int i=0;i<30;i++)
    {
        long *a = (long *)alloctor.allocate(1);
        std::cout<<a<<std::endl;
        //alloctor.deallocate(a,1);
    }
    */
    TinySTL::vector<int> a(2);
    std::cout<<a.capacity()<<std::endl;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    std::cout<<a.capacity()<<std::endl;
    /*
    std::cout<<a[2]<<std::endl;
    std::cout<<a.size()<<std::endl;
    std::cout<<a.capacity()<<std::endl;
    std::cout<<a.front()<<std::endl;
    std::cout<<a.back()<<std::endl;
    std::cout<<a[10]<<std::endl;
    std::cout<<a[18]<<std::endl;
    */

    TinySTL::vector<int> b(a);
    TinySTL::vector<int> c;
    c=b;
    std::cout<<b.size()<<std::endl;
    std::cout<<b.capacity()<<std::endl;
    std::cout<<c.size()<<std::endl;
    std::cout<<c.capacity()<<std::endl;
    std::cout<<c[2]<<std::endl;
    std::cout<<c[3]<<std::endl;
    return 0;
}