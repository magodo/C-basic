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
#ifndef __THREADWORKER_H__
#define __THREADWORKER_H__

#include <string>
#include <thread>
#include <functional>
#include "IUnNamedSemaphore.h"
#include "ThreadManager.h"


class ThreadWorker
{

    enum WorkerState
    {
        kWorkerStateIniting,
        kWorkerStateInited,
        kWorkerStateRunning
    };

    public:

        ThreadWorker();
        ThreadWorker(std::string name);
        virtual ~ThreadWorker(){}

        // Following are called in a dedicated thread and meant to be overloaded
        
        virtual void Init();
        virtual void Run();
        virtual void Stop();
        virtual void Deinit();

        void Callback(unsigned int evt);

        // Used by manager thread
        
        inline std::string GetName() {return name_;}

        inline std::shared_ptr<IUnNamedSemaphore> GetOpSemaphore()  { return sem_op_;}
        inline std::shared_ptr<IUnNamedSemaphore> GetStopSemaphore() { return sem_stop_;}
        inline void ToRun()         {to_run_ = true;}
        inline void ToStop()        {to_run_ = false;}
        inline void ToQuit()        {to_quit_ = true;}

        inline void SetCallback(std::function<void(unsigned int)> f)    {callback_ = f;}

        static void ThreadFunction(ThreadWorker& worker);

    private:

        bool to_run_;
        bool to_quit_;

        bool is_first_run_; // this is only used for test purpose

        WorkerState state_;

        std::string name_;

        std::shared_ptr<IUnNamedSemaphore> sem_op_;
        std::shared_ptr<IUnNamedSemaphore> sem_stop_;

        std::function<void(unsigned int)> callback_;
};

#endif
