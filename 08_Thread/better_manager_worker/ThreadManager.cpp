#ifdef DEBUG
#include <iostream>
#endif

#include "ThreadManager.h"

bool ThreadManager::NewWorker(std::shared_ptr<ThreadWorker> p_worker)
{
    /* Check if same named thread worker has already been created. */
    if (named_workers_.find(p_worker->GetName()) != named_workers_.end())
    {
#ifdef DEBUG
        std::cout << "Manager: worker thread[" << p_worker->GetName() << "] already exists!" << std::endl;
#endif
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
#ifdef DEBUG
        std::cout << "Manager: thread[" << p_worker->GetName() << "] creation done." << std::endl;
#endif
    }

    return true;
}

bool ThreadManager::DeleteWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    {
        /* Request worker thread to quit.*/
        worker.ToQuit();

        {
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        /* Request worker thread to quit.*/
        worker.GetReqCv().notify_one();
        /* Wait "finish of quit" */
        worker.GetRespCv().wait(lk_resp);
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] quit done." << std::endl;
#endif
    }

    /* Join the deinited thread. */
    named_workers_[worker_name].thread.join();
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] join done." << std::endl;
#endif

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
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    /* Check if inited */
    if (worker.IsInited())
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] already inited!" << std::endl;
#endif
        return false;
    }

    {
        /* Request thread worker "init" */
        worker.ToInit();
        {
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        /* Request thread worker "init" */
        worker.GetReqCv().notify_one();
        /* Wait response "finish of init" */
        worker.GetRespCv().wait(lk_resp);
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] init done." << std::endl;
#endif
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
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not exists!" << std::endl;
#endif
            return false;
        }
        /* Check if any worker thread already inited. */
        if (named_workers_[worker_name].ptr->IsInited())
        {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] is already inited!" << std::endl;
#endif
            return false;
        }
    }

    /* Now all the worker threads are legal. We need to follow following sequence:
     * 1. Set init flag of each worker thread
     * 2. Lock/unlock request locker of each worker thread(guarantee workers are waiting before manager notify)
     * 3. Lock response locker of each worker thread (guarantee manager is waiting before worker notify)
     * 4. Notify request to each worker thread
     * 5. Wait with each one's response locker */

    std::map< std::string, std::shared_ptr< std::unique_lock<std::mutex> > > name_to_ptr_lock_resp;

    for (auto& worker_name: worker_name_list)
    {
        ThreadWorker& worker = *(named_workers_[worker_name].ptr);

        /* Request thread worker "init" */
        worker.ToInit();
        {
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        /* lock respose locker */
        std::shared_ptr< std::unique_lock<std::mutex> > ptr_lock_resp(new std::unique_lock<std::mutex>(worker.GetRespMutex()));
        name_to_ptr_lock_resp[worker_name] = ptr_lock_resp;
        /* Request thread worker "init" */
        worker.GetReqCv().notify_one();
    }

    for (auto& worker_name: worker_name_list)
    {
        ThreadWorker& worker = *(named_workers_[worker_name].ptr);
        worker.GetRespCv().wait(*name_to_ptr_lock_resp[worker_name]);
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] init done." << std::endl;
#endif
    }

    return true;
}


bool ThreadManager::DeinitWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    if (!worker.IsInited())
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not inited!" << std::endl;
#endif
        return false;
    }

    {
        /* Request thread worker "deinit" */
        worker.ToDeinit();

        {
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        /* Request thread worker "deinit" */
        worker.GetReqCv().notify_one();
        /* Wait response "finish of deinit" */
        worker.GetRespCv().wait(lk_resp);
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] deinit done!" << std::endl;
#endif
    }

    return true;
}

bool ThreadManager::RunWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    if (!worker.IsInited() || worker.IsRunning())
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not inited or already running!" << std::endl;
#endif
        return false;
    }

    {
        /* Request thread worker "run" */
        worker.ToRun();

        {
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        /* Request thread worker "run" */
        worker.GetReqCv().notify_one();
        /* Wait response "finish of run" */
        worker.GetRespCv().wait(lk_resp);
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] run done." << std::endl;
#endif
    }

    return true;
}

bool ThreadManager::StopWorker(std::string worker_name)
{
    /* Check existence of the worker thread. */
    if (named_workers_.find(worker_name) == named_workers_.end()) 
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] not exists!" << std::endl;
#endif
        return false;
    }

    ThreadWorker& worker = *(named_workers_[worker_name].ptr);

    if (!worker.IsRunning())
    {
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] is not running!" << std::endl;
#endif
        return false;
    }

    {
        /* Request thread worker "stop" */
        worker.ToStop();
        {
            std::lock_guard<std::mutex> lk_req(worker.GetReqMutex());
        }
        std::unique_lock<std::mutex> lk_resp(worker.GetRespMutex());
        /* Request thread worker "stop" */
        worker.GetReqCv().notify_one();
        /* Wait response "finish of stop" */
        worker.GetRespCv().wait(lk_resp);
#ifdef DEBUG
        std::cout << "Manager: thread[" << worker_name << "] stop done." << std::endl;
#endif
    }

    return true;
}
