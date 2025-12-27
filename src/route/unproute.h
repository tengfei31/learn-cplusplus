#include <net/route.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>

#define ROUNDUP(a, size) (((a) & ((size)-1)) ? (1 + ((a) | ((size) - 1))) : (a))
#define NEXT_SA(ap) ap = (struct sockaddr *) \
    ((caddr_t) ap + (ap->sa_len ? ROUNDUP(ap->sa_len, sizeof(u_long)) : \
                                  sizeof(u_long)))

void get_rtaddr(int addrs, struct sockaddr *sa, struct sockaddr **rti_info)
{
    int i;
    char *cp = (char *)sa;

    for (i = 0; i < RTAX_MAX; i++) {
        if (addrs & (1 << i)) {         // 检查该地址是否存在
            rti_info[i] = (struct sockaddr *)cp; // 保存指针
            NEXT_SA(sa);
            // cp += ((struct sockaddr *)cp)->sa_len; // 跳到下一个 sockaddr
        } else {
            rti_info[i] = NULL;
        }
    }
}