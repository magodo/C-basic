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
    is_first_run_(true),
    state_(kWorkerStateIniting),
    name_(name),
    sem_op_(IUnNamedSemaphore::GetInstance(0, IUnNamedSemaphore::kSemPosix)),
    sem_stop_(IUnNamedSemaphore::GetInstance(0, IUnNamedSemaphore::kSemPosix)),
    callback_(nullptr)
{}

ThreadWorker::ThreadWorker():
    ThreadWorker("")
{}

void ThreadWorker::Init()
{ 
    std::cout << "Worker [" << name_ << "]: Initing..." << std::endl;
}

void ThreadWorker::Deinit()
{ std::cout << "Worker [" << name_ << "]: Deinit..." << std::endl; }

void ThreadWorker::Run()
{
    if (is_first_run_)
    {
        is_first_run_ = false;
        std::cout << "Worker [" << name_ << "]: Run..." << std::endl;
    }
}

void ThreadWorker::Stop()
{ 
    is_first_run_ = true;
    std::cout << "Worker [" << name_ << "]: Stop..." << std::endl;
}

void ThreadWorker::ThreadFunction(ThreadWorker& worker)
{
    /* Initializing... */

    worker.Init();

    /* Wait for operation, it will block here for a new request if there is none.
     * Otherwise, proceed.
     * (NOTE: here must guarantee that user will not call any API after calling `QuitWorker()`) */

    worker.sem_op_->Wait();

    while (!worker.to_quit_)
    {
        /* Initing -> Inited */
        worker.state_ = kWorkerStateInited;

        while (worker.to_run_)
        {
            /* Inited -> Running */
            worker.state_ = kWorkerStateRunning;
            worker.Run();
        }

        if (worker.state_ == kWorkerStateRunning)
        {
            /* Running -> Inited */
            worker.Stop();
            worker.state_ = kWorkerStateInited;
        }

        worker.sem_stop_->Post();
        worker.sem_op_->Wait();
    }

    /* Initing/Inited -> Quit */
    worker.Deinit();
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

