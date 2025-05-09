#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


// class TcpServer {

// };


int start() {
    int sockFd;
    if (sockFd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
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
        


        close(clientFd);

    }
    close(sockFd);



    return 0;
}



