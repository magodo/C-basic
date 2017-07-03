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
    for (int i = 0; i < 10;)
    {
        std::cout << __func__ << ": starting to wait..." << std::endl;
        if (posix_sem_ptr->TryWait() == 0)
        {
            std::cout << __func__ << ": try wait succeeds" << std::endl;
            ++i;
        }
        else
        {
            std::cout << __func__ << ": try wait failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

    }
}

void CvSemThread()
{
    for (int i = 0; i < 10;)
    {
        std::cout << __func__ << ": starting to wait..." << std::endl;
        if (cv_sem_ptr->TryWait() == 0)
        {
            std::cout << __func__ << ": try wait succeeds" << std::endl;
            ++i;
        }
        else
        {
            std::cout << __func__ << ": try wait failed" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

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
