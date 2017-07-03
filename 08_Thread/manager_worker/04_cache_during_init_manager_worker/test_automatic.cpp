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

    /* Generate thread names */

    std::set<std::string> thread_names;
    const std::string alphas = "abcdefghijklmnopqrstuvwxyz";
    std::srand(std::time(nullptr));

    /* Maximum amount of threads: MAX_N_THREAD = (2 ** 64 - 1) < (26 ** 14).
     * So set the length of thread name to be 14.
     * (But too many thread will consume too many memory and will fail at begining... ) */
    const int thread_name_length = 14;

    while (thread_names.size() != num_of_thread)
    {
        std::string name;
        for (int i = 0; i < thread_name_length; ++i)
        {
            name.push_back(alphas[rand() % alphas.size()]);
        }
        thread_names.insert(name);
    }

    /* Send any request to these threads...*/

    enum Request
    {
        kNewThread,
        kDeleteThread,
        kRunThread,
        kStopThread,
        kQuitThread
    };
    const std::vector<Request> requests = {kNewThread, kDeleteThread, kRunThread, kStopThread, kQuitThread};


    for (int i = 0; i < num_of_request; ++i)
    {
        auto it = thread_names.cbegin();
        // randomly pick one thread name from the generated thread names

        int name_index = rand() % thread_names.size();
        for (int j = 0; j < name_index; ++j)
            // use std::advance in C++ for convenience
            it++;

        std::string thread_name = *it;

        // randomly send one request to the chosen thread
        switch (requests[rand() % requests.size()])
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
    for (auto& thread_name: thread_names)
    {
        mgr.QuitWorker(thread_name);
        mgr.DeleteWorker(thread_name);
    }
}








