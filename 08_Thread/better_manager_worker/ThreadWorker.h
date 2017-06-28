#ifndef __THREADWORKER_H__
#define __THREADWORKER_H__

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ThreadManager.h"

class ThreadWorker
{
    public:

        ThreadWorker();
        ThreadWorker(std::string name);
        virtual ~ThreadWorker(){}

        // Following are called in a dedicated thread and meant to be overloaded
        
        virtual void Init();
        virtual void Deinit();
        virtual void Run();
        virtual void Stop();

        // Used by manager thread
        
        inline std::string GetName() {return name_;}
        inline std::condition_variable& GetReqCv() { return cv_req_;}
        inline std::condition_variable& GetRespCv() { return cv_resp_;}
        inline std::mutex& GetReqMutex() {return mutex_req_;}
        inline std::mutex& GetRespMutex() {return mutex_resp_;}

        inline void SetToDeinit(bool to_deinit) {to_deinit_ = to_deinit;}
        inline void SetToRun(bool to_run)       {to_run_ = to_run;}
        static void ThreadFunction(ThreadWorker& worker);

    private:

        bool to_deinit_;
        bool to_run_;
        bool is_running_;
        std::string name_;

        std::condition_variable cv_req_;   // manager -> worker
        std::mutex mutex_req_;
        std::condition_variable cv_resp_;  // worker -> manager
        std::mutex mutex_resp_;

};

#endif
