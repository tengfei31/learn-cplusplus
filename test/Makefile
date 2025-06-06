PROJECT_NAME = test
CXX=clang++
CXX_STD = -std=c++20
openssl=$(shell pkg-config --cflags --libs openssl)
gtest=$(shell pkg-config --cflags --libs gtest gtest_main)
INCLUDE_DIR=-I../src/utils
CPPFLAGS+=$(INCLUDE_DIR)
LDFLAGS+=

.phony: all test_openssl_evp clean

all: test_openssl_evp

TEST_OPENSSL_EVP_BINARY=test_openssl_evp.out
test_openssl_evp:
	$(CXX) $(CXX_STD) -o $(TEST_OPENSSL_EVP_BINARY) $(CPPFLAGS) $(openssl) $(gtest) ../src/utils/openssl_evp.cpp test_openssl_evp.cpp -pthread

clean:
	rm -rf $(TEST_OPENSSL_EVP_BINARY)

