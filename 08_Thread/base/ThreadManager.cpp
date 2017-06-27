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
@brief    Manager-Worker framework - manager part
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

using namespace vehicle::videoservice;

bool ThreadManager::NewWorker(std::shared_ptr<ThreadWorker> p_worker)
{
    /* Check if same named thread worker has already been created. */
    if (named_workers_.find(p_worker->GetName()) != named_workers_.end())
    {
        return false;
    }

    {
        std::unique_lock<std::mutex> lk(p_worker->GetRespMutex);
        /* Create worker thread */
        std::thread thread = std::thread(ThreadWorker::ThreadFunction, std::ref(*p_worker));
        /* Wait response "finish of creation" */
        p_worker->GetRespCv().wait(lk);
    }

    /* Insert worker to lookup map. */
    named_workers_[p_worker->GetName()] = {p_worker, thread};

    return true;
}

bool ThreadManager::DeleteWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_.find(worker_name).ptr);

    /* Request worker thread to deinit. Nothing happens if already deinited. */
    DeinitWorker(worker_name);

    /* Join the deinited thread. */
    worker.Join();

    /* Erase the worker from internal lookup map. This will delete the
     * WorkerThread object. */
    named_workers_.erase(worker_name);

    return true;
}

bool ThreadManager::InitWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_.find(worker_name).ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        {
            /* Request thread worker "init" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
            worker.SetToDeinit(false);
        }
        worker.GetReqCv().notify();

        /* Wait response "finish of init" */
        worker.GetRespCv().wait(lk_resp);
    }
}

bool ThreadManager::DeinitWorker(std::string name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_.find(worker_name).ptr);

    {
        /* Request thread worker "deinit" */
        std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        worker.SetToDeinit(true);
    }
    worker.GetReqCv().notify();

    /* Above requst need no wait, since we'll call join in "DeleteWorker()" */
}

bool ThreadManager::RunWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_.find(worker_name).ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        {
            /* Request thread worker "init" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
            worker.SetToRun(true);
        }
        worker.GetReqCv().notify();

        /* Wait response "finish of init" */
        worker.GetRespCv().wait(lk_resp);
    }
}

bool ThreadManager::StopWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_.find(worker_name).ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        {
            /* Request thread worker "init" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
            worker.SetToRun(false);
        }
        worker.GetReqCv().notify();

        /* Wait response "finish of init" */
        worker.GetRespCv().wait(lk_resp);
    }
}
