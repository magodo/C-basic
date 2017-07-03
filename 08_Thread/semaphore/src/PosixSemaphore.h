/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 01:31:27 PM CST
 Description: 
 ************************************************************************/

#ifndef POSIXSEMAPHORE_H
#define POSIXSEMAPHORE_H

#include <semaphore.h>
#include "IUnNamedSemaphore.h"

class PosixSemaphore: public IUnNamedSemaphore
{
    public:

        PosixSemaphore(unsigned int value);

        virtual ~PosixSemaphore();

        virtual int Post();

        virtual int Wait();

        virtual int TryWait();

    private:

        sem_t sem_;
};

#endif
