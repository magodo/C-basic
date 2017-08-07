/* ************************************************************************
* Copyright (c) Continental AG and subsidiaries
* All rights reserved
*
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
* ************************************************************************/


/**
@file     FbDevBL.h
@brief    Base manager thread 
@par      Project: GWM MY18
@par      SW Component: VideoService
@par      SW Package: VideoService 
@author   
@par      Configuration Management:
@verbatim
	%derived_by: uidq8219 %
	%full_filespec: ThreadManager.cpp-1:ascii:GWM_Sha#1 % 
	%version: 1 % 
	%date_created: Tue Jul 18 19:51:24 2017 % 
@endverbatim

@todo     list of things to do
*/

#include <iostream>
#include <cerrno>
#include <cstring>

#ifdef _GNU_SOURCE
#include <pthread.h>
#endif

#include "ThreadManager.h"

bool ThreadManager::NewWorker(std::shared_ptr<ThreadWorker> p_worker)
{
    std::cout << "Manager: create thread [" << p_worker->GetName() << "]..." << std::endl;
    /* Check if same named thread worker has already been created. */
    if (named_workers_.find(p_worker->GetName()) != named_workers_.end())
    {
        std::cerr << "Manager: thread [" << p_worker->GetName() << "] already exists!" << std::endl;
        return false;
    }

    /* Insert worker to lookup map. */

    ThreadWorkerInfo& worker_info = named_workers_[p_worker->GetName()];
    worker_info.ptr = p_worker;
    worker_info.req = ThreadWorkerReq::New;
    /* Create worker thread */
    worker_info.thread = std::thread(ThreadWorker::ThreadFunction, std::ref(*p_worker));
    /* Set the thread's name */
#ifdef _GNU_SOURCE
    int ret;
    ret = pthread_setname_np(worker_info.thread.native_handle(), p_worker->GetName().c_str());
    if (ret != 0)
        std::cerr << "Manager: pthread_setname_np failed: " << strerror(errno) << std::endl;
#endif

    return true;
}

bool ThreadManager::DeleteWorker(std::string worker_name)
{
    std::cout << "Manager: request thread [" << worker_name << "] to delete" << std::endl;

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
        return false;
    }
    ThreadWorkerInfo &worker_info = named_workers_[worker_name];

    /* Check if current request on worker thread is "quit". */

    if (worker_info.req != ThreadWorkerReq::Quit)
    {
        std::cerr << "Manager: thread [" << worker_name << "] has not quited! \
Please call `QuitWorker()` first!" << std::endl;
        return false;
    }

    std::cout << "Manager: start to join thread [" << worker_name << "]" << std::endl;
    /* Join the deinited thread. */
    worker_info.thread.join();

    std::cout << "Manager: finish to join thread [" << worker_name << "]" << std::endl;

    /* Erase the worker from internal lookup map. This will delete the
     * WorkerThread object. */
    named_workers_.erase(worker_name);

    return true;
}

bool ThreadManager::QuitWorker(std::string worker_name)
{
    std::cout << "Manager: request thread [" << worker_name << "] to quit" << std::endl;

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
        return false;
    }
    ThreadWorkerInfo &worker_info = named_workers_[worker_name];

    switch (worker_info.req)
    {
        case ThreadWorkerReq::New:
        case ThreadWorkerReq::Stop:
            /* do nothing */
            break;
        case ThreadWorkerReq::Run:
            /* stop worker */
            std::cerr << "Manager: thread [" << worker_name << "] is running, now stop it first." << std::endl;
            StopWorker(worker_name);
            break;
        case ThreadWorkerReq::Quit:
            std::cerr << "Manager: thread [" << worker_name << "] already quited!" << std::endl;
            return false;
            //break;
    }
    worker_info.req = ThreadWorkerReq::Quit;

    ThreadWorker& worker = *(worker_info.ptr);
    worker.ToQuit();
    worker.GetOpSemaphore()->Post(); /* In case the requsts quit during worker initing or inited. */

    return true;
}

bool ThreadManager::RunWorker(std::string worker_name)
{
    std::cout << "Manager: request thread [" << worker_name << "] to run" << std::endl;

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
        return false;
    }
    ThreadWorkerInfo &worker_info = named_workers_[worker_name];

    switch (worker_info.req)
    {
        case ThreadWorkerReq::New:
        case ThreadWorkerReq::Stop:
            {
                ThreadWorker& worker = *(worker_info.ptr);
                worker.ToRun();
                worker.GetOpSemaphore()->Post();
            }
            break;
        case ThreadWorkerReq::Run:
            std::cerr << "Manager: thread [" << worker_name << "] already run!" << std::endl;
            return false;
            //break;
        case ThreadWorkerReq::Quit:
            std::cerr << "Manager: thread [" << worker_name << "] already quit!" << std::endl;
            return false;
            //break;
    }

    worker_info.req = ThreadWorkerReq::Run;
    return true;
}

bool ThreadManager::StopWorker(std::string worker_name)
{
    std::cout << "Manager: request thread [" << worker_name << "] to stop" << std::endl;

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
        return false;
    }
    ThreadWorkerInfo &worker_info = named_workers_[worker_name];

    switch (worker_info.req)
    {
        case ThreadWorkerReq::New:
        case ThreadWorkerReq::Stop:
            std::cerr << "Manager: thread [" << worker_name << "] not running!" << std::endl;
            return false;
            //break;
        case ThreadWorkerReq::Run:
            {
                ThreadWorker& worker = *(worker_info.ptr);
                worker.ToStop();
                worker.GetStopSemaphore()->Wait();
            }
            break;
        case ThreadWorkerReq::Quit:
            std::cerr << "Manager: thread [" << worker_name << "] already quit!" << std::endl;
            return false;
            //break;
    }
    worker_info.req = ThreadWorkerReq::Stop;
    return true;
}
