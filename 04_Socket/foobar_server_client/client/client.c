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
#include <arpa/inet.h>
#include "../server/PacketFormat.h"

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

void interactive(int sockfd)
{
    int n;
    struct PacketFormat packet;
    int opt = 1;

    while (opt)
    {
        printf("0. Quit\n1. OP1\nPlease enter: ");
        scanf("%d", &opt);

        switch(opt)
        {
            case 0:
                close(sockfd);
                break;
            case 1:
                {
                    packet.op_code = 1;
                    packet.op_value.i = 2;

                    /* TODO: send() is not guaranteed to send all bytes!*/
                    if (send(sockfd, &packet, sizeof(packet), 0) != sizeof(packet))
                    {
                        fprintf(stderr, "send error: %s\n", strerror(errno));
                        return;
                    }
                }
                break;
            default:
                fprintf(stderr, "???\n");
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    struct addrinfo *ai_list, *aip;
    struct addrinfo hint;
    int sockfd, err;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        return -1;
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;

    if ((err = getaddrinfo(NULL, argv[1], &hint, &ai_list)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(err));
        return -1;
    }

    for (aip = ai_list; aip != NULL; aip = aip->ai_next)
    {

#if 0
        struct sockaddr_in *addr_in = (struct sockaddr_in *)aip->ai_addr;
        char ip_address[INET6_ADDRSTRLEN];

        inet_ntop(aip->ai_family, &addr_in->sin_addr, ip_address, INET6_ADDRSTRLEN);
        printf("IP Address: %s\n", ip_address);
        printf("Port: %d\n", ntohs(addr_in->sin_port));
#endif

        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) >= 0)
        {
            freeaddrinfo(ai_list);
            interactive(sockfd);
            return 0;
        }
    }
}
        
