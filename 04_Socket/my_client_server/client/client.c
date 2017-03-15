/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Sat 11 Mar 2017 09:00:00 PM CST
 Description: 
 ************************************************************************/

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define BUFLEN 128
#define MAXSLEEP 128

int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t len)
{
    int fd;
    int i;

    for (i = 1; i <= MAXSLEEP; i <<= 1)
    {
        if ((fd = socket(domain, type, protocol)) < 0)
        {
            perror("socket error");
            return -1;
        }
        if (connect(fd, addr, len) == 0)
            return fd;

        close(fd);
        sleep(i);
    }
    return -1;
}

void print_uptime(int sockfd)
{
    int n;
    char buf[BUFLEN];

    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        write(STDOUT_FILENO, buf, n);
    if (n < 0)
        perror("recv error");
}

int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err;
    char hostname[HOST_NAME_MAX];

    if (argc != 2)
    {
        fprintf(stderr, "usage: ./a.out service_name\n");
        return -1;
    }

    gethostname(hostname, HOST_NAME_MAX);

    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(hostname, argv[1], &hint, &ailist)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(err));
        return -1;
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) < 0)
            err = errno;
        else
        {
            print_uptime(sockfd);
            return 0;
        }
    }
}
        
