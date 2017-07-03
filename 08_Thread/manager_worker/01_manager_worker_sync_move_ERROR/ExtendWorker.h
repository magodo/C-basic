/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 31 May 2017 01:22:46 PM CST
 Description: 
 ************************************************************************/

#ifndef __EXTENDWORKER_H__
#define __EXTENDWORKER_H__ 

#include "ManagerWorker.h"

class ExtendWorker: public Worker
{
    public:
        ExtendWorker();
        ExtendWorker(std::string name);
        ExtendWorker(ExtendWorker&& other);
        ExtendWorker& operator=(ExtendWorker&& other);

        virtual ~ExtendWorker();
        virtual void init();
        virtual void deinit();
        virtual void run();
        virtual void stop();
};

#endif
