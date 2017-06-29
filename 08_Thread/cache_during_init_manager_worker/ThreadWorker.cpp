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
    to_quit_(false),
    to_run_(false),
    is_inited_(false),
    is_running_(false),
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
{ std::cout << "Worker [" << name_ << "]: Init..." << std::endl; }

void ThreadWorker::Deinit()
{ std::cout << "Worker [" << name_ << "]: Deinit..." << std::endl; }

void ThreadWorker::Run()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Worker [" << name_ << "]: Run..." << std::endl;
}

void ThreadWorker::Stop()
{ std::cout << "Worker [" << name_ << "]: Stop..." << std::endl; }

void ThreadWorker::ThreadFunction(ThreadWorker& worker)
{
    std::unique_lock<std::mutex> lk_req(worker.mutex_req_, std::defered_lock);
    std::unique_lock<std::mutex> lk_resp(worker.mutex_resp_, std::defered_lock);

    worker.Init();

    lk_resp.lock();
    worker.state_ = kWorkerStateInited;
    lk_resp.unlock();
      
    lk_req.lock();
    /* Initing -> Inited, wait new request from manager */
    worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || woker.to_quit_);});
    lk_req.unlock();

    while (!worker.to_quit_)
    {
        while (worker.to_run_)
        {
            /* Inited -> Running */
            if (worker.state_ != kWorkerStateRunning)
            {
                lk_resp.lock();
                worker.state_ = kWorkerStateRunning;
                lk_resp.unlock();
                worker.cv_resp_.notify_one();
            }

            Run();
        }

        /* Running -> Inited, wait new request from manager */
        lk_resp.lock()
        Stop();
        worker.state_ = kWorkerStateInited;
        lk_req.lock();
        lk_resp.unlock();
        worker.cv_resp_.notify_one();
        worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
        lk_req.unlock();
    }

    /* Inited -> Quiting */
    Deinit();
    lk_resp.lock();
    worker.state_ = kWorkerStateQuiting;
    lk_resp.unlock();
    worker.cv_resp_.notify_one();
}


    /////////
    lk_req.lock();
    if (!worker.to_quit_ && !worker.to_run_)
    {
        /* Initing -> Inited, wait new request from manager */
        lk_resp.lock();
        worker.state_ = kWorkerStateInited;
        lk_resp.unlock();

        worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
    }
    else if (worker.to_run_)
    {
        lk_resp.lock();
        worker.state_ = kWorkerStateRunning;
        lk_resp.unlock();
    }
    else
    {

    }

    lk_req.unlock();

    while (!worker.to_quit_)
    {
        while (worker.to_run_)
        {
            {
                std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);

                /* Inited -> Running */
                if (worker.state_ != kWorkerStateRunning)
                {
                    {
                        worker.state_ = kWorkerStateRunning;
                    }
                    worker.cv_resp_.notify_one();
                }
            }

            Run();
        }

        /* Running -> Inited, wait new request from manager */
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
            Stop();
            worker.state_ = kWorkerStateInited;
        }
        lk_req.lock();
        worker.cv_resp_.notify_one();
        worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
        lk_req.unlock();
    }

    /* Inited -> Quiting */
    Deinit();
    {
        std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        worker.state_ = kWorkerStateQuiting;
    }
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
