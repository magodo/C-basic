/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 03 Jul 2017 01:38:29 PM CST
 Description: 
 ************************************************************************/

#include "CvSemaphore.h"
#include "PosixSemaphore.h"

std::shared_ptr<IUnNamedSemaphore> IUnNamedSemaphore::GetInstance(unsigned int value, enum SemaphoreType type)
{
    switch (type)
    {
        case kSemPosix:
            return std::make_shared<PosixSemaphore>(value);
        case kSemCv:
            return std::make_shared<CvSemaphore>(value);
        default:
            return nullptr;
    }
}


