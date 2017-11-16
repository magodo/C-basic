/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 25 Jul 2017 10:11:27 AM CST
 Description: 
 ************************************************************************/

#ifndef FOOBARSERVER_H
#define FOOBARSERVER_H

#include <sys/types.h>
#include <string>

class FoobarServer
{
    public:

        bool Listen(std::string name);

        bool Accept();

    private:

        static int DoListen(std::string name);

        static int DoAccept(int fd, pid_t &peer_pid);

    private:

        int fd_;

        int clfd_;
};

#endif
