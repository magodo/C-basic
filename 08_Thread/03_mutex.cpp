/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Thu 25 May 2017 09:23:53 PM CST
 Description: 
 ************************************************************************/

#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>

std::mutex g_lock;

void thread_function()
{
    g_lock.lock();
    std::cout << "Thread " << std::this_thread::get_id() << " enter" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Thread " << std::this_thread::get_id() << " leave" << std::endl;
    g_lock.unlock();
}

int main()
{
    std::thread t1(&thread_function);
    std::thread t2(&thread_function);
    std::thread t3(&thread_function);
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
