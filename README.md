# learn-cplusplus

C++ / Linux-Mac 网络编程学习项目：从基础 socket，到 select、kqueue/epoll 事件驱动，再到协程与国密加解密，最终目标是构建一个平台无关（kqueue / epoll / io_uring）的迷你网络库。

- 工具链：cmake (>= 3.24)、clang++ (LLVM)、make、vcpkg
- 依赖库：glog、gflags、Boost、OpenSSL、async_simple（通过 [vcpkg](https://vcpkg.io) manifest 模式管理）

## 目录结构

```
├── main.cpp                  # 主程序入口（当前演示协程）
├── CMakeLists.txt            # 顶层构建（vcpkg manifest 模式）
├── include/utils/            # 通用工具头文件
│   ├── logger.h              # glog 封装（Logger：初始化、日志目录、自定义前缀格式）
│   ├── thread_pool.h         # 线程池（std::packaged_task + 条件变量，enqueue 返回 future）
│   └── EndianUtils.h         # 字节序工具
├── src/
│   ├── server/               # TCP Server 三种实现（可独立构建，入口在 tcp_server.cpp）
│   │   ├── tcp_server.cpp        # 阻塞版：一连接一线程
│   │   ├── tcp_server_select.cpp # select 版：fd_set 多路复用 + 线程池
│   │   ├── tcp_server_event.cpp  # kqueue/epoll 事件驱动版（当前迭代）
│   │   └── CMakeLists.txt        # server 子项目独立构建脚本
│   ├── client/
│   │   └── tcp_client.cpp    # TCP 客户端
│   ├── dns/
│   │   └── server.cpp        # 简易 DNS 服务器（手工构造/解析 DNS 报文）
│   ├── route/
│   │   └── unproute.h        # 路由表相关学习（源自 UNP）
│   ├── coroutine.cpp         # C++20 协程练习（awaitable、调度器）
│   ├── kqueue.cpp            # kqueue 单独练习：监听已连接 socket 并回显
│   ├── state_machine.cpp     # 状态机练习
│   ├── thread.cpp            # 线程基础练习
│   ├── handle_file.cpp       # 文件处理练习
│   └── utils/
│       ├── logger.cpp        # Logger 实现（基于 glog）
│       ├── thread_pool.cpp   # 线程池实现
│       ├── openssl_evp.h     # OpenSSL EVP 接口声明
│       └── openssl_evp.cpp   # 国密实现：SM2 签名/验签，SM4 ECB/CBC 加解密，key/iv 生成
└── test/
    ├── test_openssl_evp.cpp  # SM2/SM4 测试
    └── sm2_private_key.pem   # 测试用 SM2 私钥
```

## 构建与运行

### 主程序

```bash
cmake -B build -S . -G "Unix Makefiles"
cmake --build build
./build/main
```

> 注意：`CMakeLists.txt` 中 vcpkg 工具链、LLVM clang++ 路径、async_simple 路径目前为绝对路径，
> 换机器需要相应修改。首次配置时 vcpkg 会按 `vcpkg.json` 自动安装依赖。

### TCP Server（独立子项目）

```bash
cd src/server
cmake -B build -S .
cmake --build build
./build/tcp_server
```

Server 监听 `9999` 端口，可用 `nc` 验证：

```bash
nc localhost 9999
```

连接后服务端回显 `welcome to cpp world`；当前入口调用 `tcp_server_event()`（kqueue 版），
切换实现修改 `tcp_server.cpp` 中 `main` 的调用即可。

## TCP Server 演进路线

| 阶段 | 文件 | 模型 | 特点 / 遗留问题 |
|---|---|---|---|
| 1. 阻塞 | `tcp_server.cpp` | accept 后每连接开线程 | 连接数受限，线程开销大 |
| 2. select | `tcp_server_select.cpp` | 单线程 `select` + 线程池 | fd 上限 1024；每轮 O(n) 扫描；close 经由队列回主线程 |
| 3. 事件驱动 | `tcp_server_event.cpp` | kqueue（macOS）/ epoll（Linux，规划） | 事件循环 + 线程池；IO 与业务耦合，存在 fd 竞争 |
| 4. 抽象层 | `src/net/`（规划中） | Poller 抽象 + EventLoop | 业务代码不出现平台 API |
| 5. io_uring | `poller_io_uring.cpp`（规划中） | completion-based | Linux 5.1+，multishot accept/recv |

## 事件抽象层设计（进行中）

目标：`tcp_server_event.cpp` 里不再出现 `kevent` / `epoll` 等平台调用，
业务只面向统一接口，平台差异收敛在各自的 Poller 实现里。

### 接口草案

```cpp
// include/net/Event.h —— 统一事件位
enum Event : uint32_t {
    kNone   = 0,
    kRead   = 1u << 0,
    kWrite  = 1u << 1,
    kError  = 1u << 2,   // EPOLLERR / kevent EV_ERROR
    kClosed = 1u << 3,   // EPOLLRDHUP|EPOLLHUP / kevent EV_EOF
};

// include/net/Poller.h —— 平台无关的事件多路复用接口
struct FiredEvent {
    int      fd;
    uint32_t events;      // 上枚举的组合
    void*    userData;    // 注册时携带的上下文
};

class Poller {
public:
    virtual ~Poller() = default;
    virtual bool add(int fd, uint32_t events, void* userData = nullptr) = 0;
    virtual bool mod(int fd, uint32_t events, void* userData = nullptr) = 0;
    virtual bool del(int fd) = 0;
    // timeoutMs: -1 永久阻塞；0 非阻塞；返回就绪事件数
    virtual int wait(std::vector<FiredEvent>& fired, int timeoutMs = -1) = 0;

    static std::unique_ptr<Poller> create();   // 工厂：按平台选择实现
};
```

### 平台事件映射

| 统一事件 | kqueue | epoll |
|---|---|---|
| kRead | `EVFILT_READ` 触发 | `EPOLLIN` |
| kWrite | `EVFILT_WRITE` 触发 | `EPOLLOUT` |
| kClosed | `EV_EOF`（READ 下为对端关闭） | `EPOLLRDHUP` / `EPOLLHUP` |
| kError | `EV_ERROR` | `EPOLLERR` |

实现时的关键差异：

1. **注册粒度**：kqueue 按 filter 分条目注册（一个 fd 可同时挂 READ、WRITE 两条 kevent），
   epoll 是一个 fd 一个事件集。因此 `KqueuePoller` 内部需要维护 `fd -> 已注册 filter` 的映射，
   `add/mod/del` 时按需 `EV_ADD/EV_DELETE` 对应的 filter；`epoll` 实现则直接 `EPOLL_CTL_ADD/MOD/DEL`。
2. **触发模式**：统一先做 level-triggered（kqueue 天然 LT；epoll 默认 LT）。
   kqueue 的 `EV_CLEAR` 对应 epoll 的 EPOLLET，后续可作选项。
3. **io_uring**：completion-based，与 readiness 模型不同。第一阶段方案：
   在 `IoUringPoller` 内部用 multishot accept/recv 提交请求，CQE 到达时翻译成 `FiredEvent(kRead)`
   对外维持 readiness 语义；后期再把 TcpServer 的 IO 操作本身异步化（提交式接口）。

### 线程模型

参考 muduo：**IO（read/write/accept/close）固定在事件循环线程完成，线程池只做无 IO 的业务计算**，
结果通过 `loop->runInLoop()/queueInLoop()` 投回循环线程写回。
这样可以根治当前 `tcp_server_event.cpp` 的几个并发问题：
同一 fd 被主循环与多个 worker 同时 read、worker 中 close 与主循环事件派发的 fd 复用竞争。

### 目标目录

```
include/net/     Event.h  Poller.h  EventLoop.h
src/net/         poller_kqueue.cpp (macOS/BSD)
                 poller_epoll.cpp  (Linux)
                 poller_io_uring.cpp (Linux 5.1+)
                 event_loop.cpp
src/server/      tcp_server_event.cpp 只 include net/，不再有平台宏
```

### 迁移步骤

1. 抽出 `Event/Poller` 接口，实现 `KqueuePoller`，改造 `tcp_server_event.cpp` 去平台宏
2. 实现 `EpollPoller`，在 Linux 上验证（接口不动）
3. 实现 `IoUringPoller`（multishot 伪装 readiness）
4. 抽象 `TcpConnection/Buffer/回调`，向迷你 asio 演进

## 其他模块

- **协程**：`src/coroutine.cpp` 基于 C++20 co_await 实现 Task/Awaitable 与简单调度器，
  `main.cpp` 入口演示；另探索了 async_simple 库。
- **国密**：`src/utils/openssl_evp.cpp` 封装 SM2 签名/验签、SM4 ECB/CBC 加解密，`test/` 下有对应用例。
- **DNS**：`src/dns/server.cpp` 手工构造 DNS 报文头/问题段/应答段，实现简易 UDP DNS 服务。
