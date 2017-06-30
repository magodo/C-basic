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

#include "ThreadManager.h"

bool ThreadManager::NewWorker(std::shared_ptr<ThreadWorker> p_worker)
{
    /* Check if same named thread worker has already been created. */
    if (named_workers_.find(p_worker->GetName()) != named_workers_.end())
    {
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
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    // Request worker to quit


    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        if (worker.IsIniting())
        {
            {
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                lk_resp.unlock();
                worker.ToQuit();
            }
        }
        else
        {
            if (worker.IsRunning())
            {
                {
                    std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                    worker.ToStop();
                }
                worker.GetRespCv().wait(lk_resp, [&]{return !worker.IsInited();});
            }

            {
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                worker.ToQuit();
            }
            /* Request worker thread to quit.*/
            worker.GetReqCv().notify_one();
            /* Wait "finish of quit" */
            worker.GetRespCv().wait(lk_resp, [&]{return worker.IsQuiting();});
        }
    }

    /* Join the deinited thread. */
    named_workers_[worker_name].thread.join();

    /* Erase the worker from internal lookup map. This will delete the
     * WorkerThread object. */
    named_workers_.erase(worker_name);

    return true;
}

bool ThreadManager::RunWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        if (worker.IsRunning())
        {
            lk_resp.unlock();
            return false;
        }
        else if (worker.IsIniting())
        {

            {
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                lk_resp.unlock();
                worker.ToRun();
            }
        }
        else // IsInited
        {
            /* Request thread worker "run" */
            {
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                worker.ToRun();
            }
            /* Request thread worker "run" */
            worker.GetReqCv().notify_one();
            /* Wait response "finish of run" */
            worker.GetRespCv().wait(lk_resp);
        }
    }

    return true;
}

bool ThreadManager::StopWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        if (worker.IsInited())
        {
            lk_resp.unlock();
            return false;
        }
        else if (worker.IsIniting())
        {
            {
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                lk_resp.unlock();
                worker.ToStop();
            }
        }
        else // Running
        {
            {
                std::unique_lock<std::mutex> lk_req(worker.GetReqMutex());
                worker.ToStop();
            }
            /* Wait response "finish of stop" */
            worker.GetRespCv().wait(lk_resp, [&] {return worker.IsInited();});
        }
    }

    return true;
}
