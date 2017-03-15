/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 15 Mar 2017 10:22:25 PM CST
 Description: 
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#define BUFLEN 100
#define QLEN 10

const char *service = "uptimed";

void setcloexec(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFD);
    flags |= FD_CLOEXEC;
    fcntl(fd, F_SETFD, flags);
}

int serve(int fd)
{
    int clfd;
    FILE *fp;
    char buf[BUFLEN];

    for (;;)
    {
        if ((clfd = accept(fd, NULL, NULL)) < 0)
            exit(1);
        setcloexec(clfd);
        if ((fp = popen("/usr/bin/uptime", "r")) == NULL)
        {
            snprintf(buf, BUFLEN, "error: %s\n", strerror(errno));
            send(clfd, buf, strlen(buf), 0);
        }
        else
        {
            while (fgets(buf, BUFLEN, fp) != NULL)
                send(clfd, buf, strlen(buf), 0);
            pclose(fp);
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
    
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    if (bind(fd, addr, alen) == -1)
        return -1;

    // only listen for conntion socket type
    if ((type == SOCK_STREAM) || (type == SOCK_SEQPACKET))
        listen(fd, qlen);
    return fd;
}

int main()
{
    struct addrinfo *ai_list = NULL, *aip;
    char hostname[HOST_NAME_MAX];
    int fd;

    gethostname(hostname, HOST_NAME_MAX);
    getaddrinfo(hostname, service, NULL, &ai_list);
    
    for (aip = ai_list; aip != NULL; aip = aip->ai_next)
    {
        if ((fd = initserver(aip->ai_socktype, aip->ai_addr, aip->ai_addrlen, QLEN)) >=0 )
        {
            serve(fd);
            exit(0);
        }
    }
    exit(1);
}
