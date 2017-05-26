/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Thu 25 May 2017 09:46:21 PM CST
 Description: 
 ************************************************************************/

#include <thread>
#include <mutex>
#include <iostream>

std::mutex g_lock;

void thread_func()
{
    g_lock.lock();
    std::cout << "TID: " << std::this_thread::get_id() << std::endl;
    g_lock.unlock();
}

int main()
{
    std::thread t1(thread_func);       // 立即启动线程1
    std::thread t2;                     // 延迟启动线程2
    std::cout << "T2: " << t2.get_id() << std::endl;
    t2 = std::thread{&thread_func};     // 线程2此时启动
    std::cout << "T2: " << t2.get_id() << std::endl;

    t1.join();
    t2.join();
}
