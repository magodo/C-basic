/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 06 Jun 2017 11:17:57 AM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <functional>

class Foobar
{
    public:
        int m_;
};

int main()
{
    Foobar foobar;
    foobar.m_ = 123;

    auto f = std::bind(&Foobar::m_, &foobar);
    std::cout << f() << std::endl;;
}
