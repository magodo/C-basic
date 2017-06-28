#include <iostream>
#include <chrono>
#include "ThreadWorker.h"

ThreadWorker::ThreadWorker(std::string name):
    to_quit_(false),
    to_init_(false),
    to_run_(false),
    is_inited_(false),
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
    /* -> deinit */
    {
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        }
        std::unique_lock<std::mutex> lk_req(worker.mutex_req_);
        /* Notify manager respopnse: "finish of creation" */
        worker.cv_resp_.notify_one();
        /* Wait manager request: "init" or "quit" */
        worker.cv_req_.wait(lk_req);
    }

    while (!worker.to_quit_)
    {
        while (worker.to_init_ && !worker.to_quit_)
        {
            /* deinit -> init */
            if (!worker.is_inited_)
            {
                /* Init */
                worker.Init();
                worker.is_inited_ = true;

                {
                    std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
                }
                std::unique_lock<std::mutex> lk_req(worker.mutex_req_);
                /* Notify manager respopnse: "finish of init" */
                worker.cv_resp_.notify_one();
                /* Wait manager request: "run" or "deinit" or "quit" */
                worker.cv_req_.wait(lk_req);
            }

            while (worker.to_run_ && worker.to_init_ && !worker.to_quit_)
            {
                /* init -> run */
                if (!worker.is_running_)
                {
                    worker.is_running_ = true;

                    {
                        std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
                    }
                    /* Notify manager respopnse: "finish of run" */
                    worker.cv_resp_.notify_one();
                }

                /* Run */
                worker.Run();
            }

            /* run -> init */
            if (!worker.to_run_ && worker.is_running_)
            {
                /* Stop */
                worker.Stop();
                worker.is_running_ = false;

                {
                    std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
                }
                std::unique_lock<std::mutex> lk_req(worker.mutex_req_);
                /* Notify manager respopnse: "finish of stop" */
                worker.cv_resp_.notify_one();
                /* Wait manager request: "run" or "deinit" or "quit" */
                worker.cv_req_.wait(lk_req);
            }
        }

        /* run/init -> deinit */
        if (!worker.to_init_ && worker.is_inited_)
        {
            if (worker.is_running_)
            {
                worker.Stop();
                worker.is_running_ = false;
            }

            /* Deinit */
            worker.Deinit();
            worker.is_inited_ = false;
            {
                std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
            }
            std::unique_lock<std::mutex> lk_req(worker.mutex_req_);
            /* Notify manager respopnse: "finish of deinit" */
            worker.cv_resp_.notify_one();
            /* Wait manager request: "init" or "quit" */
            worker.cv_req_.wait(lk_req);
        }
    }

    /* run/init/deinit -> quit */
    if (worker.is_running_)
    {
        worker.Stop();
        worker.is_running_ = false;
    }
    if (worker.is_inited_)
    {
        worker.Deinit();
        worker.is_inited_ = false;
    }
    {
        std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
    }
    /* Notify manager respopnse: "finish of quit" */
    worker.cv_resp_.notify_one();
    return;
}



