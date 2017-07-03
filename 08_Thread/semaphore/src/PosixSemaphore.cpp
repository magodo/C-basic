/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 01:55:39 PM CST
 Description: 
 ************************************************************************/

#include "PosixSemaphore.h"

PosixSemaphore::PosixSemaphore(unsigned int value)
{
    if (sem_init(&sem_, 0, value) != 0)
        throw SemaphoreCreationException();
}

PosixSemaphore::~PosixSemaphore()
{
    if (sem_destroy(&sem_) != 0)
        throw SemaphoreDestroyException();
}

int PosixSemaphore::Post()
{
    return sem_post(&sem_);
}

int PosixSemaphore::Wait()
{
    return sem_wait(&sem_);
}

int PosixSemaphore::TryWait()
{
    return sem_trywait(&sem_);
}

