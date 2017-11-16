/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 25 Jul 2017 01:13:38 PM CST
 Description: 
 ************************************************************************/

#include "FoobarServer.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>

int FoobarServer::DoListen(std::string name)
{
    struct sockaddr_un un;

    if (name.size() > sizeof un.sun_path)
        errno = 
    /* Fill in socket address. */
}
