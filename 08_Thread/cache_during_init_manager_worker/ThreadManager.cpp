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

    if (!worker.IsQuit())
    {
        QuitWorker(worker_name);
    }

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
    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        if (worker.IsIniting())
        {
            worker.ToQuit();
        }
        else if (worker.IsQuit())
        {
#ifdef DEBUG
            std::cerr << "Manager: thread [" << worker_name << "] already in \"Quit\" state" << std::endl;
#endif
            return false;
        }
        else // IsRunning or IsInited
        {
            if (worker.IsRunning())
            {
#ifdef DEBUG
                std::cout << "Manager: request thread [" << worker_name << "] to stop" << std::endl;
#endif
                /* If in running state, transit to "Inited" state first. */
                worker.ToStop();
                worker.GetRespCv().wait(lk_resp, [&]{return !worker.IsInited();});
#ifdef DEBUG
                std::cout << "Manager: thread [" << worker_name << "] in \"Stop\" state" << std::endl;
#endif
            }

            /* Inited state */

            {
                /* This lock, guarantees worker thread is waiting before this point. */
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                worker.ToQuit();
            }
            /* Request worker thread to quit.*/
            worker.GetReqCv().notify_one();
            /* Wait "finish of quit" */
            worker.GetRespCv().wait(lk_resp, [&]{return worker.IsQuit();});
#ifdef DEBUG
                std::cout << "Manager: thread [" << worker_name << "] in \"Quit\" state" << std::endl;
#endif
        }
    }

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

    {
        /* Manager thread must acquire response lock first, this lock protect worker's state. */

        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        if (worker.IsRunning() || worker.IsQuit())
        {
#ifdef DEBUG
            std::cerr << "Manager: thread [" << worker_name << "] already in \"Running\" or \"Quit\" state" << std::endl;
#endif
            return false;
        }
        else if (worker.IsIniting())
        {
            worker.ToRun();
        }
        else // IsInited
        {
            {
                /* This lock, guarantees worker thread is waiting before this point. */
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                worker.ToRun();
            }
            /* Request thread worker "run" */
            worker.GetReqCv().notify_one();
            /* Wait response "finish of run" */
            worker.GetRespCv().wait(lk_resp);
#ifdef DEBUG
            std::cout << "Manager: thread [" << worker_name << "] in \"Running\" state" << std::endl;
#endif
        }
    }

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

    {
        /* Manager thread must acquire response lock first, this lock protect worker's state. */

        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        if (worker.IsInited() || worker.IsQuit())
        {
#ifdef DEBUG
            std::cerr << "Manager: thread [" << worker_name << "] already in \"Inited\" or \"Quit\" state" << std::endl;
#endif
            return false;
        }
        else if (worker.IsIniting())
        {
            worker.ToStop();
        }
        else // Running
        {
            worker.ToStop();
            /* Wait response "finish of stop" */
            worker.GetRespCv().wait(lk_resp, [&] {return worker.IsInited();});
#ifdef DEBUG
            std::cout << "Manager: thread [" << worker_name << "] in \"Inited\" state" << std::endl;
#endif
        }
    }

    return true;
}
