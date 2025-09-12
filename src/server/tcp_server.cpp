#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/event.h>

#include "utils/thread_pool.h"
// #include "util/inc/sysapi/nio.h"


// class TcpServer {

// };

ThreadPool threadPool(4);

void handle_connection(int clientFd) {
    char buffer[1024];
    read(clientFd, buffer, sizeof(buffer));
    std::cout << "Client message: " << buffer << std::endl;

    std::string msg = "welcome to cpp world\n";
    write(clientFd, msg.c_str(), msg.length());

    close(clientFd);
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


    while(1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientFd = accept(sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
        if (clientFd < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        //将客户端链接分发到线程池中
        std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << '\n';

        // threadPool.addTask([](int clientFd){
        //     char buffer[1024];
        //     read(clientFd, buffer, sizeof(buffer));
        //     std::cout << "Client message: " << buffer << std::endl;

        //     std::string msg = "welcome to cpp world\n";
        //     write(clientFd, msg.c_str(), msg.length());

        //     close(clientFd);
        // }, clientFd);
        //or直接使用lambda表达式
        threadPool.enqueue(handle_connection, clientFd).get();
        //threadPool.addTask(handle_connection, clientFd);

    }
    close(sockFd);



    return 0;
}

int main() {
    tcp_server();
    return 0;
}

