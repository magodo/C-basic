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
@brief    Base worker thread 
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
#include <iostream>
#include <chrono>
#include "ThreadWorker.h"

ThreadWorker::ThreadWorker(std::string name):
    to_run_(false),
    to_quit_(false),
    state_(kWorkerStateIniting),
    name_(name),
    cv_req_(),
    mutex_req_(),
    cv_resp_(),
    mutex_resp_(),
    callback_(nullptr)
{}

ThreadWorker::ThreadWorker():
    ThreadWorker("")
{}

void ThreadWorker::Init()
{ 
    std::cout << "Worker [" << name_ << "]: Initing..." << std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Worker [" << name_ << "]: Inited" << std::endl;
}

void ThreadWorker::Deinit()
{ std::cout << "Worker [" << name_ << "]: Deinit..." << std::endl; }

void ThreadWorker::Run()
{
    std::cout << "Worker [" << name_ << "]: Run..." << std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ThreadWorker::Stop()
{ std::cout << "Worker [" << name_ << "]: Stop..." << std::endl; }

void ThreadWorker::ThreadFunction(ThreadWorker& worker)
{
    std::unique_lock<std::mutex> lk_resp(worker.mutex_resp_, std::defer_lock);
    std::unique_lock<std::mutex> lk_req(worker.mutex_req_, std::defer_lock);

    /* Initializing... */

    worker.Init();

    /* Hold response lock here because we will change status then. Release lock only when we are at
     * a certain status. */
    lk_resp.lock();

    /* Hold request lock here because we will check any unhandled request issued during initing.
     * Release lock when finish checking. */
    lk_req.lock();

    if (!worker.to_run_ && !worker.to_quit_)
    {
        lk_req.unlock();

        /* If there is no "to_run" or "to_quit" request, we can change state to "inited", after aquiring
         * request lock, we can release response lock so that manager could proceed. */

        /* Initing -> Inited */
        worker.state_ = kWorkerStateInited;
        lk_req.lock(); // maybe this lock is not needed be unlock beforehead.
        lk_resp.unlock();
        worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
    }

    /* At this point, there are 2 situations need to notice:
     *
     * 1. During initing state, there is no "to_run"/"to_quit" request, then response lock is released here.
     * 2. During initing state, there is "to_quit"/"to_run" request, then we are still handling this
     *    request and holding the lock. And we will handle "to_quit" over "to_run" request if both exists. */

    while (!worker.to_quit_)
    {
        while (worker.to_run_)
        {
            /* Initing/Inited -> Running */
            if (worker.state_ != kWorkerStateRunning)
            {
                lk_req.unlock();

                /* If this "to_run" request is received during initing, then we are still holding
                 * response lock, so no need to lock again. Otherwise, need to lock.
                 * This response lock guarantees manager thread is waiting before this point. */
                if (worker.state_ != kWorkerStateIniting)
                    lk_resp.lock();

                worker.state_ = kWorkerStateRunning;
                lk_resp.unlock();

                /* TODO: If this "to_run" request is received during initing, then no need to notify. */
                worker.cv_resp_.notify_one();
            }

            worker.Run();
        }

        /* Running -> Inited, wait new request from manager */

        worker.Stop();

        /* This response lock guarantees manager thread is waiting before this point. */
        lk_resp.lock();
        worker.state_ = kWorkerStateInited;

        lk_req.lock();
        lk_resp.unlock();
        worker.cv_resp_.notify_one();
        worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
    }

    lk_req.unlock();

    /* Initing/Inited -> Quit */
    worker.Deinit();

    /* If this "to_quit" request is received during initing, then we are still holding response lock, so
     * no need to lock again. Otherwise, need to lock. */
    if (worker.state_ != kWorkerStateIniting)
        lk_resp.lock();

    worker.state_ = kWorkerStateQuit;
    lk_resp.unlock();

    /* TODO: If this "to_quit" request is received during initing, then no need to notify. */
    worker.cv_resp_.notify_one();
}

void ThreadWorker::Callback(unsigned int evt)
{
    if (callback_)
    {
        callback_(evt);
    }
    else
    {
        std::cout << "Worker [" << name_ << "]: Callback Event: " << evt << std::endl;
    }
}
