/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 06 Jun 2017 11:17:57 AM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <functional>

int add(int a, int b, int& c)
{
    return a+b+c;
}

int main()
{
    int c = 10;
    auto f = std::bind(add, 1, 2, c);
}
