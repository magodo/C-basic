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
	%derived_by: uidp7757 %
	%full_filespec: IFbCtrl.h-1 % 
	%version: 1 % 
	%date_created: Tue May 16 17:49:00 2017 % 
@endverbatim

@todo     list of things to do
*/

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <cerrno>
#include <cstring>
#endif

#ifdef _GNU_SOURCE
#include <pthread.h>
#endif

#include "ThreadManager.h"

bool ThreadManager::NewWorker(std::shared_ptr<ThreadWorker> p_worker)
{
#ifdef DEBUG
    std::cout << "Manager: create thread [" << p_worker->GetName() << "]..." << std::endl;
#endif
    /* Check if same named thread worker has already been created. */
    if (named_workers_.find(p_worker->GetName()) != named_workers_.end())
    {
#ifdef DEBUG
        std::cerr << "Manager: thread [" << p_worker->GetName() << "] already exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorkerInfo& worker_info = named_workers_[p_worker->GetName()];

    /* Insert worker to lookup map. */
    worker_info.ptr = p_worker;

    {
        /* Create worker thread */
        worker_info.thread = std::thread(ThreadWorker::ThreadFunction, std::ref(*p_worker));

        /* Set the thread's name */
#ifdef _GNU_SOURCE
        int ret;
        ret = pthread_setname_np(worker_info.thread.native_handle(), p_worker->GetName().c_str());
#ifdef DEBUG
        if (ret != 0)
            std::cerr << "Manager: pthread_setname_np failed: " << strerror(errno) << std::endl;
#endif
#endif
    }

    return true;
}

bool ThreadManager::DeleteWorker(std::string worker_name)
{
#ifdef DEBUG
    std::cout << "Manager: request delete thread [" << worker_name << "]" << std::endl;
#endif

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

#ifdef DEBUG
    std::cout << "Manager: start to join thread [" << worker_name << "]" << std::endl;
#endif
    /* Join the deinited thread. */
    named_workers_[worker_name].thread.join();

#ifdef DEBUG
    std::cout << "Manager: finish to join thread [" << worker_name << "]" << std::endl;
#endif

    /* Erase the worker from internal lookup map. This will delete the
     * WorkerThread object. */
    named_workers_.erase(worker_name);

    return true;
}

bool ThreadManager::QuitWorker(std::string worker_name)
{
#ifdef DEBUG
    std::cout << "Manager: request thread [" << worker_name << "] to quit" << std::endl;
#endif

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    worker.ToQuit();
    /* In case the requsts quit during worker initing or inited. */
    worker.GetOpSemaphore()->Post();

    return true;
}

bool ThreadManager::RunWorker(std::string worker_name)
{
#ifdef DEBUG
    std::cout << "Manager: request thread [" << worker_name << "] to run" << std::endl;
#endif

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    worker.ToRun();
    worker.GetOpSemaphore()->Post();

    return true;
}

bool ThreadManager::StopWorker(std::string worker_name)
{
#ifdef DEBUG
    std::cout << "Manager: request thread [" << worker_name << "] to stop" << std::endl;
#endif

    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cerr << "Manager: thread [" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    worker.ToStop();
    worker.GetStopSemaphore()->Wait();

#ifdef DEBUG
    std::cout << "Manager: thread [" << worker_name << "] in \"Inited\" state" << std::endl;
#endif

    return true;
}
