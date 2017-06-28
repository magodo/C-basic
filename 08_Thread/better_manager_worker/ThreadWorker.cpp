#include <iostream>
#include <chrono>
#include "ThreadWorker.h"

ThreadWorker::ThreadWorker(std::string name):
    to_deinit_(false),
    to_run_(false),
    is_running_(false),
    name_(name),
    cv_req_(),
    mutex_req_(),
    cv_resp_(),
    mutex_resp_()
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
    /******** Creation Sync ******/

    {
        std::unique_lock<std::mutex> lk_req(worker.mutex_req_);

        /* Notify manager respopnse: "finish of creation" */
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        }
        worker.cv_resp_.notify_one();

        /* Wait manager request: "init" or "deinit" */
        worker.cv_req_.wait(lk_req);
    }

    /* request: "deinit" */
    if (worker.to_deinit_)
    {
        /* Notify manager respopnse: "finish of deinit" */
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        }
        worker.cv_resp_.notify_one();
        return;
    }

    /******** Init Sync ******/

    {
        std::unique_lock<std::mutex> lk_req(worker.mutex_req_);

        /* Init */
        worker.Init();

        /* Notify manager respopnse: "finish of init" */
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        }
        worker.cv_resp_.notify_one();

        /* Wait manager request: "run" or "deinit" */
        worker.cv_req_.wait(lk_req);
    }

    /******** Loop  ******/

    while (!worker.to_deinit_)
    {
        /* "stop" -> "run" */
        if (worker.to_run_ && !worker.is_running_)
        {
            worker.is_running_ = true;

            /* Notify manager respopnse: "finish of start" */
            {
                std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
            }
            worker.cv_resp_.notify_one();
        }

        /* "run" -> "stop" */
        if (!worker.to_run_ && worker.is_running_)
        {
            std::unique_lock<std::mutex> lk_req(worker.mutex_req_);

            /* Stop */
            worker.Stop();
            worker.is_running_ = false;

            /* Notify manager respopnse: "finish of stop" */
            {
                std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
            }
            worker.cv_resp_.notify_one();

            /* Wait manager request: "run" or "deinit" */
            worker.cv_req_.wait(lk_req);
        }
    
        /* Run */
        worker.Run();
    }

    /* Notify manager respopnse: "finish of deinit" */
    {
        std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        worker.Deinit();
    }
    worker.cv_resp_.notify_one();

    return;
}



