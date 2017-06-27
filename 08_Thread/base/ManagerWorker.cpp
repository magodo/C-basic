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
@brief    Manager-Worker framework
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

#include "ManagerWorker.h"
#include "VsCoreLogger.h"

using namespace vehicle::videoservice;

/* WORKER */

// C'tor
Worker::Worker():
    Worker("")
{}

Worker::Worker(std::string name):
    is_running(false),
    is_init(false),
    thread(),
    name_(name),
    manager_(nullptr)
{}

// D'tor
Worker::~Worker()
{}

void Worker::init()
{
    core_logger.Info("[Worker: %s] %s() Init", getName().c_str(), __func__);
}

void Worker::deinit()
{
    core_logger.Info("[Worker: %s] %s() Deinit", getName().c_str(), __func__);
}

void Worker::run()
{
    core_logger.Info("[Worker: %s] %s() Run (sleep for 1 second actually...)", getName().c_str(), __func__);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Worker::stop()
{
    core_logger.Info("[Worker: %s] %s() Stop", getName().c_str(), __func__);
}

void Worker::setManager(Manager *manager)
{ manager_ = manager;}

void Worker::notify(unsigned int evt)
{
    manager_->callback(evt);
}

std::mutex& Worker::getLockReq() {return manager_->lock_req_;}
std::mutex& Worker::getLockResp() {return manager_->lock_resp_;}
std::condition_variable& Worker::getCvReq() {return manager_->cv_req_;}
std::condition_variable& Worker::getCvResp() {return manager_->cv_resp_;}
std::string Worker::getName() {return name_;}


/* MANAGER */

void Manager::attachWorker(Worker *p_worker)
{
    core_logger.Info("[Manager] %s() Attach worker: %s", __func__, p_worker->getName().c_str());
    p_worker->setManager(this);
    named_workers_.insert(std::make_pair(p_worker->getName(),p_worker));
}

void Manager::detachWorker(std::string name)
{
    auto it = named_workers_.find(name);
    if (it != named_workers_.end())
    {
        /* deinit worker if it is inited */
        if (it->second->is_init)
            deinitWorker(name);
        core_logger.Info("[Manager] %s() Detach worker: %s", __func__, name.c_str());
        named_workers_.erase(it);
    }
}

void Manager::startWorker(std::string name)
{
    core_logger.Info("[Manager] %s() Start worker: %s", __func__, name.c_str());

    /* Check existence of the thread */
    if (named_workers_.find(name) == named_workers_.end())
    {
        core_logger.Warn("[Manager] %s() No such worker: %s", __func__, name.c_str(), name.c_str());
        return;
    }

    Worker &worker = *named_workers_[name];

    if (!worker.is_init)
    {
        core_logger.Warn("[Manager] %s() Worker: %s not initiated yet.", __func__, name.c_str(), name.c_str());
        return;
    }
    if (worker.is_running)
    {
        core_logger.Warn("[Manager] %s() Worker: %s has already been running", __func__, name.c_str(), name.c_str());
        return;
    }

    {
        // this guarantees controllor(this) get resp lock only after worker thread release
        // it in wait...
        std::unique_lock<std::mutex> locker(lock_resp_);

        /* Change state and notify worker */
        {
            std::lock_guard<std::mutex> locker(lock_req_);
            worker.is_running = true;
        }
        cv_req_.notify_all();
        cv_resp_.wait(locker);
    }
}

void Manager::stopWorker(std::string name)
{
    core_logger.Info("[Manager] %s() Stop worker: %s", __func__, name.c_str());

    /* Check existence of the thread */
    if (named_workers_.find(name) == named_workers_.end())
    {
        core_logger.Warn("[Manager] %s() No such worker: %s", __func__, name.c_str(), name.c_str());
        return;
    }

    Worker &worker = *named_workers_[name];

    if (!worker.is_running)
    {
        core_logger.Warn("[Manager] %s() Worker: %s is not running yet.", __func__, name.c_str(), name.c_str());
        return;
    }

    // change worker state and wait notification
    {
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker.is_running = false;
        cv_resp_.wait(locker);
    }
}

void Manager::deinitWorker(std::string name)
{
    core_logger.Info("[Manager] %s() Deinit worker: %s", __func__, name.c_str());

    /* Check existence of the thread */
    if (named_workers_.find(name) == named_workers_.end())
    {
        core_logger.Warn("[Manager] %s() No such worker: %s", __func__, name.c_str(), name.c_str());
        return;
    }

    Worker &worker = *named_workers_[name];

    if (!worker.is_init)
    {
        core_logger.Warn("[Manager] %s() Worker: %s not initiated yet.", __func__, name.c_str(), name.c_str());
        return;
    }

    {
        // change worker state and wait notification
        {
            std::unique_lock<std::mutex> locker(lock_resp_);
            worker.is_init = false;
        }
        cv_req_.notify_all();
    }

    // wait for finish
    core_logger.Info("[Manager] %s() Join worker: %s", __func__, name.c_str());
    worker.thread.join();
}

void Manager::initWorker(std::string name)
{
    core_logger.Info("[Manager] %s() Init worker: %s", __func__, name.c_str());

    /* Check existence of the thread */
    if (named_workers_.find(name) == named_workers_.end())
    {
        core_logger.Warn("[Manager] %s() No such worker: %s", __func__, name.c_str(), name.c_str());
        return;
    }

    Worker &worker = *named_workers_[name];

    if (worker.is_init)
    {
        core_logger.Warn("[Manager] %s() Worker: %s already initiated.", __func__, name.c_str(), name.c_str());
        return;
    }
    
    {
        std::unique_lock<std::mutex> locker(lock_resp_);

        /* new thread */
        core_logger.Info("[Manager] %s() New thread for worker: %s", __func__, name.c_str());
        worker.thread = std::thread(newThread, std::ref(worker));

        worker.is_init = true;

        /* wait for notification */
        cv_resp_.wait(locker);
    }

}

void Manager::newThread(Worker &worker)
{
    /* initiate worker*/
    worker.init();

    {
        std::lock_guard<std::mutex> lk(worker.getLockResp());
    }
    {
        std::unique_lock<std::mutex> lk(worker.getLockReq());
        /* Notify manager finish of initiating */
        worker.getCvResp().notify_one();
        /* Wait for start or deinit*/
        worker.getCvReq().wait(lk, [&worker]{return (worker.is_running || !worker.is_init);});
    }

    if (worker.is_running)
    {
        /* Notify finish of starting */
        {
            std::lock_guard<std::mutex> lk(worker.getLockResp());
        }
        worker.getCvResp().notify_one();
    }

    while (worker.is_init)
    {
        if (!worker.is_running)
        {
            {
                std::lock_guard<std::mutex> locker(worker.getLockResp());
            }
            {
                std::unique_lock<std::mutex> locker(worker.getLockReq());
                /* Stop */
                worker.stop();
                /* Notify stop */
                worker.getCvResp().notify_one();
                /* Wait for start or deinit */
                worker.getCvReq().wait(locker, [&worker]{return (worker.is_running || !worker.is_init);});
            }
            if (!worker.is_init)
                break;

            /* Notify finish of starting */
            {
                std::lock_guard<std::mutex> lk(worker.getLockResp());
            }
            worker.getCvResp().notify_one();
        }

        /* Running */

        while (worker.is_running && worker.is_init)
        {
            worker.run();
        }
        if (!worker.is_init)
            break;
    }

    /* Deinit, no need to notify finish because controllor will join. */
    worker.deinit();

    return;
}

void Manager::callback(unsigned int evt)
{}
