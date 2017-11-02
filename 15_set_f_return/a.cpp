/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Thu 02 Nov 2017 11:38:17 PM CST
 Description: 
 ************************************************************************/

#define _FCNTL_H
#include <fcntl.h>

#include <typeinfo>
#include <iostream>
#include <functional>

using namespace std;

//int open (const char *__file, int __oflag, ...) {cout << "open()" << endl;}
//std::function<int(const char*, int)> f;
std::function<int(const char*, int)> open;

#if 0
// function traits
template<typename T> 
struct function_traits;  

template<typename R, typename ...Args> 
struct function_traits<std::function<R(Args...)>>
{
    static const size_t nargs = sizeof...(Args);

    typedef R result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};
#endif

#if 1
template<class F>
void set_f_ret(F& f, typename F::result_type r)
{
    f = [=]<typename ...Ts>(Ts&& ...ts){return r;};
}
#endif

int main()
{
    set_f_ret(open,3);
    cout << open(0,0) << endl;
}
