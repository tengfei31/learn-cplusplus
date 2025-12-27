#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/event.h>
#include <fcntl.h>

#include "utils/thread_pool.h"

extern int tcp_server_select();
extern int tcp_server_event();

int main() {
    // tcp_server_select();
    tcp_server_event();
    return 0;
}


