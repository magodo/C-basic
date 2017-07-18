/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Sun 16 Jul 2017 02:50:03 PM CST
 Description: 
 ************************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <cstring>

#include <iostream>

void ClientThread()
{
}

void ServerThread()
{
    struct sockaddr_in addr;
    char hostname[HOST_NAME_MAX];

}

int ConnectRetry(int domain, int type, int proto,
                  const struct sockaddr *addr, socklen_t addrlen)
{
    const unsigned int kSleepDuration = 128; // unit: second
    int fd;

    for (unsigned int n_sec= 1; n_sec< kSleepDuration; n_sec<<= 1)
    {
        if ((fd = socket(domain, type, proto)) == -1)
        {
            std::cerr << "socket error: " << strerror(errno) << std::endl;
            return -1;
        }

        /* client has no need to `bind()` since the address is not interested.
         * `connect()` will pick one arbitrary address if none is bound.*/
        if (connect(fd, addr, addrlen) == 0)
        {
            /* Connect succeeds, return socket handle. */
            return fd;
        }
        std::cerr << "connect error: " << strerror(errno) << std::endl;
        close(fd);
        sleep(n_sec);
    }
    return -1;
}

int InitServer(int type, int proto, const struct sockaddr *addr,
               int addrlen, int qlen)
{
    int fd;
    if ((fd = socket(addr->sa_family, type, proto)) == -1)
    {
        std::cerr << "socket error: " << strerror(errno) << std::endl;
        return -1;
    }
    if (bind(fd, addr, addrlen) == -1)
    {
        std::cerr << "bind error: " << strerror(errno) << std::endl;
        close(fd);
        return -1;
    }

    if ((type == SOCK_STREAM || type == SOCK_SEQPACKET))
    {
        if (listen(fd, qlen) == -1)
        {
            std::cerr << "listen error: " << strerror(errno) << std::endl;
            close(fd);
            return -1;
        }
    }
    return fd;
}

int main()
{
     
}
