PROJECT_NAME = test
BUILD_DIR = ./build
BINARY_FILE = $(BUILD_DIR)/$(PROJECT_NAME)
SOURCE_DIR = ./src
INCLUDE_DIR = -I./include -I/Users/wtf/Documents/www/c/util -I/opt/homebrew/opt/boost/include -I/opt/homebrew/opt/glog/include -I/opt/homebrew/opt/gflags/include
LIB_DIR = -L./lib -L/Users/wtf/Documents/www/c/util -L/opt/homebrew/opt/boost/lib
CXX=clang++
CXX_STD = -std=c++20

.phony: all clean debug

debug:
	$(CXX) $(INCLUDE_DIR) $(LIB_DIR) $(CXX_STD) -g -o $(BINARY_FILE) main.cpp ./src/*.cpp

relase:
	$(CXX) $(INCLUDE_DIR) $(LIB_DIR) $(CXX_STD) -O2 -o $(BINARY_FILE) main.cpp ./src/*.cpp

clean:
	rm -rf $(BUILD_DIR)/*
