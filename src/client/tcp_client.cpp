#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int tcp_client() {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }
    int port = 9999;
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.5.120");
    int connectState = connect(sockFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    if (connectState < 0) {
        std::cerr << "Connect failed:" << strerror(errno) << std::endl;
        return -1;
    }
    std::string msg = "Hello, i am client\n";
    write(sockFd, msg.c_str(), msg.length());

    char buffer[1024];
    read(sockFd, buffer, sizeof(buffer));
    std::cout << "Server response: " << buffer << std::endl;
    close(sockFd);

    return 0;
}

int main() {
    tcp_client();
    return 0;
}

