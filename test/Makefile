PROJECT_NAME = test
CXX=clang++
CXX_STD = -std=c++20
openssl=$(shell pkg-config --cflags --libs openssl)
gtest=$(shell pkg-config --cflags --libs gtest gtest_main)
JZ_UTIL=/Users/wtf/Documents/www/c/util
INCLUDE_DIR=-I../src/utils -I$(JZ_UTIL)
CPPFLAGS+=$(INCLUDE_DIR)
LDFLAGS+=
DEBUG= -g -O0

.phony: all test_openssl_evp clean

all: test_openssl_evp

TEST_OPENSSL_EVP_BINARY=test_openssl_evp.out
test_openssl_evp:
	$(CXX) $(CXX_STD) $(DEBUG) -o $(TEST_OPENSSL_EVP_BINARY) $(CPPFLAGS) $(openssl) $(gtest) ../src/utils/openssl_evp.cpp test_openssl_evp.cpp -pthread -L$(JZ_UTIL) -lUtilStatic

clean:
	rm -rf $(TEST_OPENSSL_EVP_BINARY)

