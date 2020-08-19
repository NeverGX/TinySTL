#include "construct.h"
#include "alloc.h"
#include <iostream>
#include <vector>
int main()
{
    TinySTL::alloc alloctor;
    for(int i=0;i<30;i++)
    {
        int *a = (int *)alloctor.allocate(8);
        std::cout<<a<<std::endl;
        alloctor.deallocate(a,8);
    }
    

    return 0;
}