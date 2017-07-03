/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 27 Jun 2017 10:42:05 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <memory>
#include <sstream>
#include "ThreadManager.h"
#include "ThreadWorker.h"

int main()
{
    ThreadManager manager;

    while (1)
    {
        int choice;
        std::string worker_name;

        std::cout << "1. New worker thread\n2. Delete worker thread\n3. Run worker thread\n4. Stop worker thread\n5. Quit worker\n6. Exit\nEnter the number: ";
        std::cin >> choice;

        if (choice == 6)
            return 1;

        std::cout << "Please enter worker's name: ";
        std::cin >> worker_name;
        switch (choice)
        {
            case 1:
                manager.NewWorker(std::make_shared<ThreadWorker>(worker_name));
                break;
            case 2:
                manager.DeleteWorker(worker_name);
                break;
            case 3:
                manager.RunWorker(worker_name);
                break;
            case 4:
                manager.StopWorker(worker_name);
                break;
            case 5:
                manager.QuitWorker(worker_name);
                break;
            default:
                std::cerr << "???" << std::endl;
                break;
        }
    }
}
