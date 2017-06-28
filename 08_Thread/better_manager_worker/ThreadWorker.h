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
        inline bool IsInited() {return is_inited_;}
        inline bool IsRunning() {return is_running_;}
        inline std::condition_variable& GetReqCv() { return cv_req_;}
        inline std::condition_variable& GetRespCv() { return cv_resp_;}
        inline std::mutex& GetReqMutex() {return mutex_req_;}
        inline std::mutex& GetRespMutex() {return mutex_resp_;}

        inline void ToQuit()        {to_quit_ = true;}
        inline void ToInit()        {to_init_ = true;}
        inline void ToDeinit()      {to_init_ = false;}
        inline void ToRun()         {to_run_  = true;}
        inline void ToStop()        {to_run_  = false;}
        static void ThreadFunction(ThreadWorker& worker);

    private:

        bool to_quit_;
        bool to_init_;
        bool to_run_;
        bool is_inited_;
        bool is_running_;
        std::string name_;

        std::condition_variable cv_req_;   // manager -> worker
        std::mutex mutex_req_;
        std::condition_variable cv_resp_;  // worker -> manager
        std::mutex mutex_resp_;

};

#endif
