/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 02:42:41 PM CST
 Description: 
 ************************************************************************/
#include <iostream>
#include <thread>
#include <memory>
#include <chrono>
#include "IUnNamedSemaphore.h"

std::shared_ptr<IUnNamedSemaphore> posix_sem_ptr = IUnNamedSemaphore::GetInstance(1, IUnNamedSemaphore::kSemPosix);
std::shared_ptr<IUnNamedSemaphore> cv_sem_ptr = IUnNamedSemaphore::GetInstance(1, IUnNamedSemaphore::kSemCv);

void PosixSemThread()
{
    for (int i = 0; i < 10; ++i)
    {
        std::cout << __func__ << ": starting to wait..." << std::endl;
        posix_sem_ptr->Wait();
        std::cout << __func__ << ": wait succeeds" << std::endl;
    }
}

void CvSemThread()
{
    for (int i = 0; i < 10; ++i)
    {
        std::cout << __func__ << ": starting to wait..." << std::endl;
        cv_sem_ptr->Wait();
        std::cout << __func__ << ": wait succeeds" << std::endl;
    }
}

int main()
{
    std::thread t1(PosixSemThread);
    std::thread t2(CvSemThread);

    for (int i = 0; i < 10; ++i)
    {
        std::cout << __func__ << ": posting semaphores..." << std::endl;
        posix_sem_ptr->Post();
        cv_sem_ptr->Post();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    t1.join();
    t2.join();

    return 0;
}
