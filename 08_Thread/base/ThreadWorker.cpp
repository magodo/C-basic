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
@brief    Manager-Worker framework - worker part
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

#include "ThreadWorker.h"

void ThreadWorker::ThreadFunction(ThreadWorker& worker)
{
    /* Creation Sync */
    {
        std::unique_lock<std::mutex> lk_req(worker.mutex_req_);

        /* Notify manager respopnse: "finish of creation" */
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        }
        worker.cv_resp_.notify();

        /* Wait manager request: "init" or "deinit" */
        worker.cv_req_.wait(lk_req);

        /* request: "deinit" */
        if (worker.to_deinit_)
            return;
    }

    /* Init Sync */
    {
        std::unique_lock<std::mutex> lk_req(worker.mutex_req_);

        /* Init */
        worker.Init()

        /* Notify manager respopnse: "finish of init" */
        {
            std::lock_guard<std::mutex> lk_resp(worker.mutex_resp_);
        }
        worker.cv_resp_.notify(lk_resp);

        /* Wait manager request: "run" or "deinit" */
        worker.cv_req_.wait(lk_req);
    }

    /* Loop */
    while (!worker.to_deinit_)
    {
        if (!worker.to_run_)
        {

        }
    }

    worker.Deinit();
    return;
}


