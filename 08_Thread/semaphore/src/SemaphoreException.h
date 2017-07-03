/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 02:18:09 PM CST
 Description: 
 ************************************************************************/

#ifndef SEMAPHOREEXCEPTION_H
#define SEMAPHOREEXCEPTION_H

#include <exception>

struct SemaphoreCreationException: public std::exception
{
    inline const char *what() const throw()
    {
        return "Semaphore creation failed!";
    }
};

struct SemaphoreDestroyException: public std::exception
{
    inline const char *what() const throw()
    {
        return "Semaphore destroy failed!";
    }
};

#endif
