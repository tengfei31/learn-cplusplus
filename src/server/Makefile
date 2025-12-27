
PROJECT_NAME=tcp_server
CXX=clang++
CXX_STD = -std=c++20
JZ_UTIL=/Users/wtf/Documents/www/c/util
INCLUDE_DIR=-I../src/utils -I../../include -I$(JZ_UTIL)
CPPFLAGS+=$(INCLUDE_DIR)
LDFLAGS+=
DEBUG= -g -O0

.phony: all build clean

all: build

build:
	$(CXX) $(CXX_STD) $(DEBUG) -o $(PROJECT_NAME) $(CPPFLAGS) tcp_server.cpp tcp_server_select.cpp tcp_server_event.cpp ../utils/thread_pool.cpp

clean:
	rm -rf $(BINARY_FILE)




