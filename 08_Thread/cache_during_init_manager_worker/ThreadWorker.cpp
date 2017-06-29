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
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Worker [" << name_ << "]: Inited" << std::endl;
}

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
    worker.Init();
    /* Change status to "Inited" */
    {
        std::unique_lock<std::mutex> lk_resp(worker.mutex_resp_);
        worker.state_ = kWorkerStateInited;
        /* Check if there is unhandled request during init, if yes, then handle them. 
         * Otherwise, wait request. */
        {
            std::unique_lock<std::mutex> lk_req(worker.mutex_req_);
            lk_resp.unlock();
            worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
        }
    }

    while (!worker.to_quit_)
    {
        while (worker.to_run_)
        {
            /* Inited -> Running */
            if (worker.state_ != kWorkerStateRunning)
            {
                {
                    std::unique_lock<std::mutex> lk_resp(worker.mutex_resp_);
                    worker.state_ = kWorkerStateRunning;
                }
                worker.cv_resp_.notify_one();
            }

            worker.Run();
        }

        /* Running -> Inited, wait new request from manager */
        {
            std::unique_lock<std::mutex> lk_resp(worker.mutex_resp_);
            worker.Stop();
            worker.state_ = kWorkerStateInited;
            {
                std::unique_lock<std::mutex> lk_req(worker.mutex_req_);
                lk_resp.unlock();
                worker.cv_resp_.notify_one();
                worker.cv_req_.wait(lk_req, [&]{return (worker.to_run_ || worker.to_quit_);});
            }
        }
    }

    /* Inited -> Quiting */
    worker.Deinit();
    {
        std::unique_lock<std::mutex> lk_resp(worker.mutex_resp_);
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
