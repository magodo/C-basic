/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 15 Mar 2017 10:22:25 PM CST
 Description: 
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "PacketFormat.h"

#define BUFLEN sizeof(struct PacketFormat)
#define QLEN 10

void serve(int fd)
{
    int clfd;
    FILE *fp;
    char buf[BUFLEN];

    for (;;)
    {
        if ((clfd = accept(fd, NULL, NULL)) < 0)
            return;

        /* Only handle one connection one time. */

        ssize_t n_recv;

        while ((n_recv = recv(clfd, buf, BUFLEN, MSG_WAITALL)) == BUFLEN)
        {
            printf("op_code = %d\n", buf[0]);
        }
        if (n_recv < 0)
        {
            fprintf(stderr, "recv error: %s\n", strerror(errno));
        }
        else if (n_recv == 0)
        {
            printf("EOF received\n");
        }
        else
        {
            fprintf(stderr, "recv byte: %ld\n", n_recv);
        }

        close(clfd);
    }
}

int initserver(int type, const struct sockaddr *addr, int alen, int qlen)
{
    int fd;
    int reuse = 1;

    if ((fd = socket(addr->sa_family, type, 0)) == -1)
        return -1;
    
    /* Avoid TCP socket regards the address of a closed socket is in used during linger time(aka. TIME_WAIT state).
     * Enabling this flag, another socket could bind to the exact same address if the previous socket bound to it
     * is in TIME_WAIT state.*/

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
 
    if (bind(fd, addr, alen) == -1)
        return -1;

    // only listen for conntion socket type
    if ((type == SOCK_STREAM) || (type == SOCK_SEQPACKET))
        listen(fd, qlen);
    return fd;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return -1;
    }

    struct addrinfo *ai_list = NULL, *aip, hint;
    int fd;

    /* If AI_PASSIVE flag is specified in hints.ai_flags and node is NULL, then
     * the returned socket address will contain the "wildcard address"(INADDR_ANY
     * for IPv4 addresses, IN6ADDR_ANY_INIT for IPv6 addresses). */

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_PASSIVE;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;

    int ret;
    if ((ret = getaddrinfo(NULL, argv[1], &hint, &ai_list)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(ret));
        return -1;
    }

    for (aip = ai_list; aip != NULL; aip = aip->ai_next)
    {
        if ((fd = initserver(aip->ai_socktype, aip->ai_addr, aip->ai_addrlen, QLEN)) >=0 )
        {
#if 0
            struct sockaddr_in *addr_in = (struct sockaddr_in *)aip->ai_addr;
            char ip_address[INET6_ADDRSTRLEN];

            inet_ntop(aip->ai_family, &addr_in->sin_addr, ip_address, INET6_ADDRSTRLEN);
            printf("IP Address: %s\n", ip_address);
            printf("Port: %d\n", ntohs(addr_in->sin_port));
#endif
            freeaddrinfo(ai_list);
            serve(fd);
            exit(0);
        }
    }
    fprintf(stderr, "Can't init server on port: %s\n", argv[1]);
    exit(1);
}
