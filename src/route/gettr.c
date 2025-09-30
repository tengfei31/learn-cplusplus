#include <net/route.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "unproute.h"

#define BUFLEN (sizeof(struct rt_msghdr) + 512)
#define SEQ 9999


char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];  // 静态缓冲区，返回字符串
    void *addr;

    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in *sin = (struct sockaddr_in *)sa;
        addr = &(sin->sin_addr);
        if (inet_ntop(AF_INET, addr, str, sizeof(str)) == NULL)
            return NULL;
        break;
    }
    case AF_INET6: {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
        addr = &(sin6->sin6_addr);
        if (inet_ntop(AF_INET6, addr, str, sizeof(str)) == NULL)
            return NULL;
        break;
    }
    default:
        snprintf(str, sizeof(str), "Unknown AF %d", sa->sa_family);
        break;
    }

    return str;
}

int main(int argc, char **argv)
{
    int sockfd;
    char *buf;
    pid_t pid;
    ssize_t n;
    struct rt_msghdr *rtm;
    struct sockaddr *sa, *rti_info[RTAX_MAX];
    struct sockaddr_in *sin;

    if (argc != 2) {
        puts("param error");
        return 0;
    }
    sockfd = socket(AF_ROUTE, SOCK_RAW, 0);

    buf = (char *)calloc(1, BUFLEN);
    rtm = (struct rt_msghdr*)buf;
    rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
    rtm->rtm_type = RTM_GET;
    rtm->rtm_version = RTM_VERSION;
    rtm->rtm_addrs = RTA_DST;
    rtm->rtm_pid = pid = getpid();
    rtm->rtm_seq = SEQ;

    sin = (struct sockaddr_in*) (rtm+1);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(sin->sin_addr));

    write(sockfd, rtm , rtm->rtm_msglen);

    do {
        n = read(sockfd, rtm, BUFLEN);
    } while(rtm->rtm_type != RTM_GET || rtm->rtm_seq != SEQ || rtm->rtm_pid != pid);

    rtm = (struct rt_msghdr*) buf;
    sa = (struct sockaddr*)(rtm + 1);
    get_rtaddr(rtm->rtm_addrs, sa, rti_info);

    // struct sockaddr_in* s;
    // void* addr;
    // char ipstr[INET6_ADDRSTRLEN];
    if ((sa = rti_info[RTAX_DST]) != NULL) {
        // s = (struct sockaddr_in*) sa;
        // addr = &s->sin_addr;
        // inet_ntop(sa->sa_family, addr, ipstr, sizeof(ipstr));
        printf("dest: %s\n", sock_ntop_host(sa, sa->sa_len));
    }
    if ((sa = rti_info[RTAX_GATEWAY]) != NULL) {
        // s = (struct sockaddr_in*) sa;
        // addr = &s->sin_addr;
        // inet_ntop(sa->sa_family, addr, ipstr, sizeof(ipstr));
        printf("gateway: %s\n", sock_ntop_host(sa, sa->sa_len));
    }
    return 0;
}





