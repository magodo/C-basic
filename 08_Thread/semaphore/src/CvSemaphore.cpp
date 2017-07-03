/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 01:10:03 PM CST
 Description: 
 ************************************************************************/

#include "CvSemaphore.h"

CvSemaphore::CvSemaphore(unsigned int value):
    mutex_(),
    cv_(),
    max_sem_value_(UINT32_MAX)
{
    if (value > max_sem_value_)
    {
        throw SemaphoreCreationException();
    }
    counter_ = value;
}

CvSemaphore::~CvSemaphore()
{}

int CvSemaphore::Post()
{
    {
        std::unique_lock<decltype(mutex_)> lk(mutex_);
        if (counter_ == max_sem_value_)
            return -1;
        ++counter_;
    }
    // Here we unblock the mutex before notifying to avoid the woken up thread to be blocked again.
    cv_.notify_one();
    return 0;
}

int CvSemaphore::Wait()
{
    std::unique_lock<decltype(mutex_)> lk(mutex_);
    cv_.wait(lk, [&]{ return counter_ != 0;});
    --counter_;
    return 0;
}

int CvSemaphore::TryWait()
{
    std::unique_lock<decltype(mutex_)> lk(mutex_);
    if (counter_ != 0)
    {
        --counter_;
        return 0;
    }
    return -1;
}

