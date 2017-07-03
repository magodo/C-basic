/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 28 Jun 2017 09:52:40 AM CST
 Description: 
 ************************************************************************/

#include "ThreadManager.h"
#include "ThreadWorker.h"
#include <vector>
#include <string>

int main()
{
    ThreadManager mgr;
    std::vector<std::string> thread_names;

    for (int i = 0; i < 100; ++i)
    {
        std::string thread_name(std::to_string(i));
        /* Create workers and remember their names. */
        mgr.NewWorker(std::make_shared<ThreadWorker>(thread_name));
        thread_names.push_back(thread_name);
    }
    mgr.InitWorkers(thread_names);

    for (auto& name: thread_names)
    {
        mgr.DeleteWorker(name);
    }
}
