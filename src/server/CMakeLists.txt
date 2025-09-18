cmake_minimum_required(VERSION 3.20)
project(tcp_server LANGUAGES CXX)

set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/../../include
    ${PROJECT_SOURCE_DIR}/../../include/utils
    ${PROJECT_SOURCE_DIR}/../utils
    "/Users/wtf/Documents/www/c/util"
)

# 源文件列表
set(SOURCES
    tcp_server.cpp
    tcp_server_select.cpp
    tcp_server_event.cpp
    ${PROJECT_SOURCE_DIR}/../utils/thread_pool.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# 如有需要可添加链接库
# target_link_libraries(${PROJECT_NAME} ...)