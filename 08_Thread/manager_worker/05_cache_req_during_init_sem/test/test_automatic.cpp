/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 30 Jun 2017 02:03:28 PM CST
 Description: 
 ************************************************************************/

#include "ThreadWorker.h"
#include "ThreadManager.h"

#include <string>
#include <iostream>
#include <set>
#include <vector>

#include <cmath>
#include <ctime>
#include <cstdlib>

enum Request
{
    kNewThread,
    kDeleteThread,
    kRunThread,
    kStopThread,
    kQuitThread
};

std::vector<Request> GetAllRequest()
{
    return std::vector<Request>{kNewThread, kDeleteThread, kRunThread, kStopThread, kQuitThread};
}

std::vector<Request> GetRationalRequest(Request last_req)
{
    switch (last_req)
    {
        case kNewThread:
            return std::vector<Request>{kRunThread, kQuitThread};
        case kDeleteThread:
            return std::vector<Request>{kNewThread};
        case kRunThread:
            return std::vector<Request>{kStopThread};
        case kStopThread:
            return std::vector<Request>{kRunThread, kQuitThread};
        case kQuitThread:
            return std::vector<Request>{kDeleteThread};
        default:
            std::cerr << "??? (" << last_req << ")" << std::endl;
            exit(-1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./a.out" << " num_of_thread  num_of_request" << std::endl;
        return 1;
    }

    int num_of_thread = std::stoi(argv[1]);
    int num_of_request = std::stoi(argv[2]);

    ThreadManager mgr;

    /* Construct a mapping: thread_name -> last_request(by default is kDeleteThread) */

    std::map<std::string, Request> workers_last_requests;

    const std::string alphas = "abcdefghijklmnopqrstuvwxyz";
    std::srand(std::time(nullptr));

    /* Maximum amount of threads: MAX_N_THREAD = (2 ** 64 - 1) < (26 ** 14).
     * So set the length of thread name to be 14.
     * (But too many thread will consume too many memory and will fail at begining... ) */
    const int thread_name_length = 14;

    while (workers_last_requests.size() != num_of_thread)
    {
        std::string name;
        for (int i = 0; i < thread_name_length; ++i)
        {
            name.push_back(alphas[rand() % alphas.size()]);
        }
        workers_last_requests[name] = kDeleteThread;
    }

    /* Pick one thread to perform one rational request. Repeat this process for "num_of_request" times. */

    for (int i = 0; i < num_of_request; ++i)
    {
        auto it = workers_last_requests.cbegin();
        
        // randomly pick one thread name from the generated thread names

        int name_index = rand() % workers_last_requests.size();
        for (int j = 0; j < name_index; ++j)
            // use std::advance in C++ for convenience
            it++;

        std::string thread_name = it->first;

        
        // Get a request set (based on last request or all request sets available)
#if 0
        std::vector<Request> requests = GetRationalRequest(it->second);
#else
        const std::vector<Request> requests = GetAllRequest();
#endif

        // randomly send one request to the chosen thread
        Request req = requests[rand() % requests.size()]; 

        /* Update last requst as current request. */
        workers_last_requests[thread_name] = req;

        switch (req)
        {
            case kNewThread:
                mgr.NewWorker(std::make_shared<ThreadWorker>(thread_name));
                break;
            case kDeleteThread:
                mgr.DeleteWorker(thread_name);
                break;
            case kRunThread:
                mgr.RunWorker(thread_name);
                break;
            case kStopThread:
                mgr.StopWorker(thread_name);
                break;
            case kQuitThread:
                mgr.QuitWorker(thread_name);
                break;
        }
    }

    /* Clean up threads */
    std::cout << "End of Test, cleaning up..." << std::endl;
    for (auto& element: workers_last_requests)
    {
        Request req = element.second;
        std::string name = element.first;

#if 0
        switch (req)
        {
            case kNewThread:
            case kStopThread:
                mgr.QuitWorker(name);
                mgr.DeleteWorker(name);
                break;
            case kDeleteThread:
                break;
            case kRunThread:
                mgr.StopWorker(name);
                mgr.QuitWorker(name);
                mgr.DeleteWorker(name);
                break;
            case kQuitThread:
                mgr.DeleteWorker(name);
                break;
        }
#else
        mgr.QuitWorker(name);
        mgr.DeleteWorker(name);
#endif
    }
}
