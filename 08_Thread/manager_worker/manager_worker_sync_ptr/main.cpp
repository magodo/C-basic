/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 31 May 2017 01:21:46 PM CST
 Description: 
 ************************************************************************/

//#define BASE

#include "ManagerWorker.h"
#include "ExtendWorker.h"


int main()
{
    Manager manager;
    int choice;
    std::string name;
    bool to_quit = false;

    while (!to_quit)
    {
        std::cout << "0. New worker\n1. Delete worker\n2. Init worker\n3. Start worker\n4. Stop worker\n5. Deinit worker\n6. Quit\nEnter the number of what you want to do: ";
        std::cin >> choice;
        if (choice != 6)
        {
            std::cout << "Thread name: ";
            std::cin >> name;
        }
        
        switch (choice)
        {
            case 0:
                //manager.attachWorker(std::unique_ptr<Worker>(new Worker(name)));
                manager.attachWorker(std::unique_ptr<ExtendWorker>(new ExtendWorker(name)));
                break;
            case 1:
                manager.detachWorker(name);
                break;
            case 2:
                manager.initWorker(name);
                break;
            case 3:
                manager.startWorker(name);
                break;
            case 4:
                manager.stopWorker(name);
                break;
            case 5:
                manager.deinitWorker(name);
                break;
            case 6:
                to_quit = true;
                break;
            default:
                std::cerr << "???" << std::endl;
                break;
        }
    }
    return 0;
}

