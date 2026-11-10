#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include "utils/logger.h"

#define MAX_LINE 1024

// 练习：用 kqueue 监听一个已连接的 socket，把读到的数据回显到标准输出。
// 注意：kqueue 中一个 kevent 条目只能注册一个 filter，
// READ 和 WRITE 必须分别注册两条（位或 EVFILT_READ|EVFILT_WRITE 等于 EVFILT_READ，属隐蔽 bug）。
void create_kqueue(int socketFd)
{
    int kq = kqueue();
    if (kq == -1) {
        LOG(ERROR) << "kqueue created failed";
        return;
    }

    struct kevent changes[2];
    EV_SET(&changes[0], socketFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(&changes[1], socketFd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kq, changes, 2, NULL, 0, NULL) == -1) {
        LOG(ERROR) << "kqueue register failed: " << strerror(errno);
        close(kq);
        return;
    }

    struct kevent eventlist[2];
    char buffer[MAX_LINE];
    for (;;) {
        int nready = kevent(kq, NULL, 0, eventlist, 2, NULL);
        if (nready == -1) {
            if (errno == EINTR) continue;
            LOG(ERROR) << "kevent wait failed: " << strerror(errno);
            break;
        }
        for (int i = 0; i < nready; i++) {
            if (eventlist[i].flags & EV_ERROR) {
                LOG(ERROR) << "kevent error on fd " << eventlist[i].ident;
                close(socketFd);
                close(kq);
                return;
            }
            if (eventlist[i].filter == EVFILT_READ) {
                ssize_t n = read(socketFd, buffer, MAX_LINE);
                if (n == 0) {
                    // 对端关闭连接
                    close(socketFd);
                    close(kq);
                    return;
                }
                if (n < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                        continue;
                    }
                    LOG(ERROR) << "read failed: " << strerror(errno);
                    close(socketFd);
                    close(kq);
                    return;
                }
                //将读取到的字节写入到标准输出
                ssize_t written = 0;
                while (written < n) {
                    ssize_t w = write(fileno(stdout), buffer + written, n - written);
                    if (w < 0) {
                        LOG(ERROR) << "write stdout failed: " << strerror(errno);
                        break;
                    }
                    written += w;
                }
            } else if (eventlist[i].filter == EVFILT_WRITE) {
                // socket 发送缓冲区可写，可在此写数据
            }
            if (eventlist[i].flags & EV_EOF) {
                // 对端关闭
                close(socketFd);
                close(kq);
                return;
            }
        }
    }

    close(kq);
}
