/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 12 Jul 2017 10:13:29 AM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <limits.h>
#include <arpa/inet.h>
#include <cstdlib>

#include <iostream>

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "usage: " << argv[0] << " ip port" << std::endl;
        return -1;
    }

    int err;
    struct sockaddr_in saddr;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = atoi(argv[2]);
    if ((err = inet_pton(AF_INET, argv[1], &saddr.sin_addr.s_addr)) != 1)
    {
        if (err == 0)
        {
            std::cerr << argv[1] << " doesn't represent a valid network address in the specified address family" << std::endl;
            return -1;
        }
        std::cerr << "inet_pton error: " << strerror(errno) << std::endl;
        return -1;
    }

    if ((err = getnameinfo((struct sockaddr*)(&saddr), sizeof(struct sockaddr), hbuf, NI_MAXHOST, sbuf,NI_MAXSERV,NI_NAMEREQD)) != 0)
    {
        std::cerr << "gethostname error: " << gai_strerror(err) << std::endl;
        return -1;
    }

    std::cout << "Host   : " << hbuf << std::endl;
    std::cout << "Servant: " << sbuf << std::endl;

}
