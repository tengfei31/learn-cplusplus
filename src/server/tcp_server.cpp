#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/event.h>
#include <fcntl.h>

#include "utils/thread_pool.h"
// #include "util/inc/sysapi/nio.h"


// class TcpServer {

// };

ThreadPool threadPool(4);

int handle_connection(int clientFd) {
    char buffer[1024];
    int n = read(clientFd, buffer, sizeof(buffer));
    if (n <= 0) {
        return 0;
    }
    std::cout << "Client message: " << buffer << std::endl;

    std::string msg = "welcome to cpp world\n\n\n";
    int len = write(clientFd, msg.c_str(), msg.length());

    //如果需要主动断开，或者检测客户端断开连接，可以通过queue队列通知主线程，来主动close和FD_CLR
    //close(clientFd);

    return len;
}


int tcp_server() {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }
    int port = 9999;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(sockFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }
    if (listen(sockFd, 1024) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }
    //这里增加epoll，来处理多路复用
    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(sockFd, &master_set);
    int maxFd = sockFd;
    int activity;

    while(1) {
        read_set = master_set;
        activity = select(maxFd + 1, &read_set, NULL, NULL, NULL);
        if (activity < 0) {
            std::cerr << "select failed" << std::endl;
            break;
        }
        //有新的连接
        if (FD_ISSET(sockFd, &read_set)) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientFd = accept(sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
            if (clientFd >= 0) {
                FD_SET(clientFd, &master_set);
                if (clientFd > maxFd) {
                    maxFd = clientFd;
                }
                //将客户端链接分发到线程池中
                std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << '\n';
            }
        }
        //检查所有连接的客户端
        int fd = 0;
        for (; fd <= maxFd; fd++) {
            if (fd != sockFd && FD_ISSET(fd, &read_set)) {
                //使用work线程池处理业务逻辑
                threadPool.enqueue(handle_connection, fd);
                // if (len <= 0) {
                //     close(fd);
                //     FD_CLR(fd, &master_set);
                // }
            }
        }
    }
    close(sockFd);

    return 0;
}

int main() {
    tcp_server();
    return 0;
}

