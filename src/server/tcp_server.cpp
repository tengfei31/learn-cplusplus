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

ThreadPool workerThreadPool(4);
//全局队列，用作通知关闭socket
std::queue<int> closeQueue;
std::mutex closeQueueMutex;
// std::condition_variable closeQueueCv;

fd_set master_set;

void handle_connection(int clientFd);

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
    // //增加非阻塞操作
    // int sockFcntl;
    // sockFcntl = fcntl(sockFd, F_GETFL, 0);
    // fcntl(sockFd, F_SETFL, sockFcntl | O_NONBLOCK);

    //这里增加epoll，来处理多路复用
    fd_set read_set;
    FD_SET(sockFd, &master_set);
    int maxFd = sockFd;
    int activity;

    while(1) {
        //处理已经关闭的fd
        {
            std::unique_lock<std::mutex> closeQueueLock(closeQueueMutex);
            while(!closeQueue.empty()) {
                int clientFd = closeQueue.front();
                closeQueue.pop();
                close(clientFd);
                FD_CLR(clientFd, &master_set);
                if (maxFd == clientFd) {
                    while(maxFd >= 0 && !FD_ISSET(maxFd, &master_set)) {
                        --maxFd;
                    }
                }
            }
        }

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
                // //增加非阻塞操作
                // int clientFcntl;
                // clientFcntl = fcntl(clientFd, F_GETFL, 0);
                // fcntl(clientFd, F_SETFL, clientFcntl | O_NONBLOCK);

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
                //TODO: 后续可将read/write放到当前主线程中，比如增加read_queue和write_queue，handl只处理具体的业务逻辑
                //使用work线程池处理业务逻辑
                workerThreadPool.enqueue(handle_connection, fd);
            }
        }
    }
    close(sockFd);

    return 0;
}

int main() {
    //初始化全局变量
    FD_ZERO(&master_set);

    tcp_server();
    return 0;
}

void handle_connection(int clientFd) {
    char buffer[1024];
    int n = read(clientFd, buffer, sizeof(buffer));
    if (n < 0) {
        if (errno != EWOULDBLOCK) {
            //这里错误就断开
            std::lock_guard<std::mutex> closeQueueLock(closeQueueMutex);
            closeQueue.push(clientFd);
            // closeQueueCv.notify_one();
            return;
        }
    }
    if (n == 0) {
        std::cout << "Client No message: EOF on socket " << std::endl;
        // {
        //     std::lock_guard<std::mutex> closeQueueLock(closeQueueMutex);
        //     closeQueue.push(clientFd);
        // }
        // closeQueueCv.notify_one();
        return ;
    }
    std::cout << "Client message: " << buffer << std::endl;

    std::string msg = "welcome to cpp world\n\n\n";
    int len;
    int w_len = 0;
    while(true) {
        len = write(clientFd, msg.c_str(), msg.length());
        w_len += len;
        if (w_len >= msg.length()) {
            break;
        }
    }

    //为了测试，需要主动断开，后续可能需要在具体业务中关闭
    // {
    //     std::lock_guard<std::mutex> closeQueueLock(closeQueueMutex);
    //     closeQueue.push(clientFd);
    // }
    // closeQueueCv.notify_one();

    return;
}


