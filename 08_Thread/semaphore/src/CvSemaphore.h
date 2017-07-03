/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 01:03:13 PM CST
 Description: 
 ************************************************************************/

#ifndef CVSEMAPHORE_H
#define CVSEMAPHORE_H

#include "IUnNamedSemaphore.h"
#include <mutex>
#include <condition_variable>

class CvSemaphore: public IUnNamedSemaphore
{
    public:
        
        CvSemaphore(unsigned int value);

        virtual ~CvSemaphore();

        virtual int Post();

        virtual int Wait();

        virtual int TryWait();

    private:

        std::mutex mutex_;
        std::condition_variable cv_;
        unsigned int counter_;
        const unsigned int max_sem_value_;
};

#endif
