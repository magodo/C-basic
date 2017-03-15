/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 15 Mar 2017 03:33:46 PM CST
 Description: Show address info of specified service on the host
              running this script.
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#define HOST_NAME_MAX 100

void print_flag(const struct addrinfo *aip)
{
    int flag = aip->ai_flags;
    int has_flag = 0;

    printf("Flag: ");
    if (flag & AI_PASSIVE)
    {
        has_flag = 1;
        printf("passive ");
    }
    if (flag & AI_CANONNAME)
    {
        has_flag = 1;
        printf("canoname ");
    }
    if (flag &AI_NUMERICHOST)
    {
        has_flag = 1;
        printf("numeric_host ");
    }
    if (flag & AI_V4MAPPED)
    {
        has_flag = 1;
        printf("v4mapped ");
    }

    if (!has_flag)
        printf("N/A");
    printf("\n");

}

void print_family(const struct addrinfo *aip)
{
    switch (aip->ai_family)
    {
        case AF_INET:
            printf("Family: AF_INET\n");
            break;
        case AF_INET6:
            printf("Family: AF_INET6\n");
            break;
        default:
            fprintf(stderr, "Unknown family\n");
            break;
    }
}

void print_type(const struct addrinfo *aip)
{
    switch(aip->ai_socktype)
    {
        case SOCK_STREAM:
            printf("Socket type: SOCK_STREAM\n");
            break;
        case SOCK_DGRAM:
            printf("Socket type: SOCK_DGRAM\n");
            break;
        case SOCK_RAW:
            printf("Socket type: SOCK_RAW\n");
            break;
        case SOCK_RDM:
            printf("Socket type: SOCK_RDM\n");
            break;
        case SOCK_PACKET:
            printf("Socket type: SOCK_PACKET\n");
            break;
        case SOCK_SEQPACKET:
            printf("Socket type: SOCK_SEQPACKET\n");
            break;
    }
}

void print_protocol(const struct addrinfo *aip)
{
    switch(aip->ai_protocol)
    {
        case 0:
            printf("Protocal: default\n");
            break;
        case IPPROTO_RAW:
            printf("Protocal: RAW\n");
            break;
        case IPPROTO_UDP:
            printf("Protocal: UDP\n");
            break;
        case IPPROTO_TCP:
            printf("Protocal: TCP\n");
            break;
        default:
            printf("Protocal: %d\n", aip->ai_protocol);
            break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: $ ./a.out service\n");
        exit(1);
    }

    long buf_len;
    char *host, ip_address[INET6_ADDRSTRLEN];
    struct addrinfo *ai_list = NULL, hints, *aip;

    // get node
    buf_len = sysconf(_SC_HOST_NAME_MAX);
    buf_len = (buf_len == -1)? HOST_NAME_MAX:buf_len;
    host = (char*)malloc(buf_len); //FIXME
    gethostname(host, buf_len);

    // hint
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    getaddrinfo(host, argv[1], &hints, &ai_list); // FIXME

    int index = 0;
    for (aip = ai_list; aip != NULL; aip = aip->ai_next)
    {
        printf("-----------------------------------\n");
        printf("Index: %d\n", index++);
        print_flag(aip);
        print_family(aip);
        print_type(aip);
        print_protocol(aip);

        // print canonname
        printf("Host: %s\n", aip->ai_canonname? aip->ai_canonname:"N/A");
        // print IP address and port if is IP family
        if ((aip->ai_family == AF_INET) || (aip->ai_family == AF_INET6))
        {
            struct sockaddr_in *addr_in = (struct sockaddr_in *)aip->ai_addr;

            inet_ntop(aip->ai_family, addr_in, ip_address, INET6_ADDRSTRLEN);
            printf("IP Address: %s\n", ip_address);
            printf("Port: %d\n", ntohs(addr_in->sin_port));
        }
    }
    printf("-----------------------------------\n");
}
