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
        std::unique_lock<std::mutex> lk(p_worker->GetRespMutex());
        /* Create worker thread */
        worker_info.thread = std::thread(ThreadWorker::ThreadFunction, std::ref(*p_worker));
        /* Wait response "finish of creation" */
        p_worker->GetRespCv().wait(lk);
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

    /* Request worker thread to deinit. Nothing happens if already deinited. */
    DeinitWorker(worker_name);

    /* Join the deinited thread. */
    named_workers_[worker_name].thread.join();

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

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        {
            /* Request thread worker "init" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
            worker.SetToDeinit(false);
        }
        worker.GetReqCv().notify_one();

        /* Wait response "finish of init" */
        worker.GetRespCv().wait(lk_resp);
    }

    return true;
}

bool ThreadManager::InitWorkers(std::vector<std::string> worker_name_list)
{
    for (auto& worker_name: worker_name_list)
    {
        /* Check existence of each worker thread. */
        if (named_workers_.find(worker_name) == named_workers_.end()) 
        {
            return false;
        }
    }

    /* Now all the worker threads are legal. We need to lock each one's response locker first,
     * then lock/unlock each one's request locker(guarantee the workers are waiting) and send request
     * (notify_one) one-by-one. At last, wait with each one's response locker */

    std::map< std::string, std::shared_ptr< std::unique_lock<std::mutex> > > name_to_ptr_lock_resp;

    for (auto& worker_name: worker_name_list)
    {
        ThreadWorker& worker = *(named_workers_[worker_name].ptr);

        /* lock respose locker */
        std::shared_ptr< std::unique_lock<std::mutex> > ptr_lock_resp(new std::unique_lock<std::mutex>(worker.GetRespMutex()));
        name_to_ptr_lock_resp[worker_name] = ptr_lock_resp;

        {
            /* Request thread worker "init" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
            worker.SetToDeinit(false);
        }
        worker.GetReqCv().notify_one();
    }

    for (auto& worker_name: worker_name_list)
    {
        ThreadWorker& worker = *(named_workers_[worker_name].ptr);

        worker.GetRespCv().wait(*name_to_ptr_lock_resp[worker_name]);
    }

    return true;
}


bool ThreadManager::DeinitWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    {
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());

        worker.SetToDeinit(true);

        {
            /* Request thread worker "deinit" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        worker.GetReqCv().notify_one();

        /* Wait response "finish of deinit" */
        worker.GetRespCv().wait(lk_resp);
    }

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

        worker.SetToRun(true);

        {
            /* Request thread worker "run" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        worker.GetReqCv().notify_one();

        /* Wait response "finish of run" */
        worker.GetRespCv().wait(lk_resp);
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
        worker.SetToRun(false);

        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        {
            /* Request thread worker "stop" */
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        worker.GetReqCv().notify_one();

        /* Wait response "finish of stop" */
        worker.GetRespCv().wait(lk_resp);
    }

    return true;
}
