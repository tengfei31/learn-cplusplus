# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.24)
# 定义项目名称和使用的语言
project(main LANGUAGES CXX)
# 设置C++标准为C++20
set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")
execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE COMPILER_VERSION)
message(STATUS "Compiler version: ${COMPILER_VERSION}")

set(CMAKE_TOOLCHAIN_FILE /Users/wtf/vcpkg/scripts/buildsystems/vcpkg.cmake)

# 设置构建类型（Debug/Release），默认为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 启用新策略以支持现代 Boost 查找方式
cmake_policy(SET CMP0167 OLD)

find_package(Boost REQUIRED COMPONENTS system filesystem thread)
find_package(glog CONFIG REQUIRED)
find_package(gflags REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(async_simple CONFIG REQUIRED)
include(FindPkgConfig)
# 添加头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/utils
    "/Users/wtf/Documents/www/c/util"
    ${llvm_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${glog_INCLUDE_DIRS}
    ${gflags_INCLUDE_DIRS}
    ${async_simple_INCLUDE_DIRS}
)
# 添加库文件目录
# link_directories(
#     ${PROJECT_SOURCE_DIR}/lib
#     /Users/wtf/Documents/www/c/util
#     ${Boost_INCLUDE_DIRS}/lib
# )

# 收集所有源文件
file(GLOB SOURCES "src/*.cpp" "src/utils/*.cpp" "main.cpp")
# 添加可执行文件目标
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接所需的库（如果有的话）
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    glog::glog
    Boost::system
    Boost::filesystem
    Boost::thread
    OpenSSL::Crypto
    async_simple::async_simple
)
