#ifndef __THREADMANAGER_H__
#define __THREADMANAGER_H__

#include <thread>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "ThreadWorker.h"

// forward delcare
class ThreadWorker;

struct ThreadWorkerInfo
{
    std::shared_ptr<ThreadWorker> ptr;
    std::thread                   thread;
};

class ThreadManager
{
    public:

        virtual ~ThreadManager(){}
        bool NewWorker(std::shared_ptr<ThreadWorker> p_worker);
        bool DeleteWorker(std::string worker_name);
        bool InitWorker(std::string worker_name);
        bool InitWorkers(std::vector<std::string> worker_name_list);
        bool DeinitWorker(std::string name);
        bool RunWorker(std::string name);
        bool StopWorker(std::string name);
    
    protected:

        std::map<std::string, ThreadWorkerInfo> named_workers_;
};

#endif
