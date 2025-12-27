#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#if __linux__
#include <sys/epoll.h>
#elif defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/event.h>
#include <sys/time.h>
#endif

#include "utils/thread_pool.h"

static const int MAX_EVENTS = 128;
static ThreadPool workerThreadPool(4);

static void handle_connection(int clientFd, int kq);
static void close_event(int clientFd, int kq);

int tcp_server_event() {

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }
    // 设置 sockFd 非阻塞
    int flags = fcntl(sockFd, F_GETFL, 0);
    fcntl(sockFd, F_SETFL, flags | O_NONBLOCK);

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
#if __linux__
#elif defined(__APPLE__) || defined(__FreeBSD__)
    int kq = kqueue();
    if (kq == -1) {
        std::cout << "kqueue created failed" << std::endl;
        close(sockFd);
        return -1;
    }
    struct kevent kev;
    EV_SET(&kev, sockFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &kev, 1, NULL, 0, NULL) < 0) {
        std::cerr << "kqueue add sockFd failed" << std::endl;
        close(sockFd);
        close(kq);
        return -1;
    }
    struct kevent events[MAX_EVENTS];
    while(1) {
        int n = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (n < 0) {
            std::cerr << "kqueue add events failed" << std::endl;
            break;
        }
        for (int i = 0; i < n; i++) {
            int fd = events[i].ident;
            if (fd == sockFd) {
                //服务器有动静了
                while (1) {
                    struct sockaddr_in clientAddr;
                    socklen_t clientAddrLen = sizeof(clientAddr);
                    int clientFd = accept(sockFd, (struct sockaddr*) &clientAddr, &clientAddrLen);
                    if (clientFd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        else break;
                    }
                    //增加非阻塞操作
                    int clientFcntl;
                    clientFcntl = fcntl(clientFd, F_GETFL, 0);
                    fcntl(clientFd, F_SETFL, clientFcntl | O_NONBLOCK);

                    struct kevent change;
                    EV_SET(&change, clientFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    kevent(kq, &change, 1, NULL, 0, NULL);
                    //将客户端链接分发到线程池中
                    std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << '\n';
                }
            } else {
                //客户端有动静了
                std::cout << "client trigger event:" << fd << "\n";
                //使用work线程池处理业务逻辑
                workerThreadPool.enqueue(handle_connection, fd, kq);
            }
        }
    }

    close(sockFd);
    close(kq);
    return 0;
#else
    #error "unknown platform"
#endif

    return -1;
}

static void handle_connection(int clientFd, int kq) {
    std::string recv_data;
    char buffer[1024];
    while(true) {
        int n = read(clientFd, buffer, sizeof(buffer));
        if (n > 0) {
            recv_data.append(buffer);
            continue;
        } else if (n == 0) {
            close_event(clientFd, kq);
            return;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            //错误就关闭客户端
            close_event(clientFd, kq);
            return;
        }
    }
    if (recv_data.length() <= 0) {
        return;
    }
    std::cout << "Client message: " << recv_data << std::endl;

    std::string msg = "welcome to cpp world\n\n\n";
    int len;
    int w_len = 0;
    while(true) {
        len = write(clientFd, msg.c_str()+w_len, msg.length()-w_len);
        if (len > 0) {
            w_len += len;
        } else if (len == 0) {
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            break;
        }
    }

    // close_event(clientFd, kq);

    return;
}

static void close_event(int clientFd, int kq) {
    // 先移除事件监听
    struct kevent change;
    EV_SET(&change, clientFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kq, &change, 1, NULL, 0, NULL);

    close(clientFd);
}
