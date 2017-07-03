/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 12:43:42 PM CST
 Description: 
 ************************************************************************/
#ifndef ISEMAPHORE_H
#define ISEMAPHORE_H 

#include <memory>
#include "SemaphoreException.h"

/* Exceptions declaration */

struct SemaphoreCreationException;
struct SemaphoreDestroyException;

/* Unnamed, thread-shared semaphore. */

class IUnNamedSemaphore
{
    public:

        enum SemaphoreType
        {
            kSemPosix,  /* Implemented by POSIX semaphore */
            kSemCv      /* Implemented by condition variable */ 
        };

        virtual ~IUnNamedSemaphore() {}

        /**
         * @brief       Create a certain type of semaphore with initial value set as "value".
         *
         * @param[in]   value: initial value of semaphore. 
         * @param[in]   semaphore type.
         * @exception   Raise "SemaphoreCreationException" on error
         *
         * @note        For destroy the created semaphore, you could:
         *              1. delete it.
         *              2. set it to "nullptr"
         *              3. call its "reset()" member function.
         *              On error, it will raise "SemaphoreDestroyException". */

        static std::shared_ptr<IUnNamedSemaphore> GetInstance(unsigned int value, enum SemaphoreType type);

        /**
         * @brief       Increments the semaphore, if the semaphore's value consequently becomes greater than zero,
         *              then another thread blocked in a `Wait()` call will be woken up and proceed to lock.
         * @ret         Returns 0 on success.
         *              On error, the value of the semaphore is left unchanged, -1 is returned. */
        virtual int Post() = 0;

        /**
         * @brief       Decrements the semaphore. If the semaphore's value is greater than zero, the decrement
         *              proceeds, and the function returns, immediately. If the semaphore currently has the value
         *              zero, then the call blocks until either it becomes possible to perform the decrement or
         *              (possibly) a signal handler interrupts the call.
         * @ret         Returns 0 on success.
         *              On error, the value of semaphore is left unchanged, -1 is returned. */
        virtual int Wait() = 0;

        /**
         * @brief       The same as `Wait()`, except that if the decrement cannot be immediately performed, then 
         *              call returns an error(-1) instead of blocking. */
        virtual int TryWait() = 0;
};

#endif
