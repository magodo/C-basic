/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 31 May 2017 01:25:31 PM CST
 Description: 
 ************************************************************************/

#include "ExtendWorker.h"

// C'tor

ExtendWorker::ExtendWorker(std::string name):
    Worker(name)
{
    std::cout << "ExtendWorker: C'tor with string" << std::endl;
}

ExtendWorker::ExtendWorker():
    ExtendWorker("")
{
    std::cout << "ExtendWorker: C'tor" << std::endl;
}

// Move C'tor
ExtendWorker::ExtendWorker(ExtendWorker&& o):
    Worker(std::move(o))
{
    std::cout << "ExtendWorker: move C'tor" << std::endl;
}

// Move assign
ExtendWorker& ExtendWorker::operator=(ExtendWorker&& o)
{
    std::cout << "ExtendWorker: move assign" << std::endl;
    return static_cast<ExtendWorker&>(Worker::operator=(std::move(o)));
}

// D'tor
ExtendWorker::~ExtendWorker()
{
    std::cout << "ExtendWorker: D'tor" << std::endl;
}

void ExtendWorker::init()
{
    std::cout << "ExtendWorker[" << getName() << "] init..." << std::endl;
}

void ExtendWorker::deinit()
{
    std::cout << "ExtendWorker[" << getName() << "] deinit..." << std::endl;
}

void ExtendWorker::run()
{
    std::cout << "ExtendWorker[" << getName() << "] run..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ExtendWorker::stop()
{
    std::cout << "ExtendWorker[" << getName() << "] stop..." << std::endl;
}
