# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 3.24)
# 定义项目名称和使用的语言
project(learn-cplusplus LANGUAGES CXX)
# 设置C++标准为C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置构建类型（Debug/Release），默认为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 启用新策略以支持现代 Boost 查找方式
cmake_policy(SET CMP0167 OLD)

find_package(Boost REQUIRED COMPONENTS system filesystem thread)
# find_package(glog REQUIRED)
find_package(gflags REQUIRED)
include(FindPkgConfig)
# pkg_check_modules(glog REQUIRED IMPORTED_TARGET glog)
# pkg_check_modules(gflags REQUIRED IMPORTED_TARGET gflags)
# 添加头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    /Users/wtf/Documents/www/c/util
    ${Boost_INCLUDE_DIRS}
    ${glog_INCLUDE_DIRS}
    ${gflags_INCLUDE_DIRS}
)
# 添加库文件目录
# link_directories(
#     ${PROJECT_SOURCE_DIR}/lib
#     /Users/wtf/Documents/www/c/util
#     ${Boost_INCLUDE_DIRS}/lib
# )

# 收集所有源文件
file(GLOB SOURCES "src/*.cpp" "main.cpp")
# 添加可执行文件目标
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接所需的库（如果有的话）
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    glog
    Boost::system
    Boost::filesystem
    Boost::thread
)
