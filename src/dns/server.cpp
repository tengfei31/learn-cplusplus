#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unordered_map>

typedef unsigned short uint16;

// DNS报文头
struct DNSHeader {
    uint16 id;//会话标识
    uint16 flags;//标志
    uint16 qdcount;//问题计数
    uint16 ancount;//回答资源记录数
    uint16 nscount;//授权资源记录数
    uint16 arcount;//附加资源记录数
};

// DNS问题结构
struct  DNSQuestion {
    uint16 qtype : 16;//查询类型
    uint16 qclass : 16;//查询类
};

// DNS资源记录结构
struct DNSRecord {
    uint16 type; //类型
    uint16 cls;//类
    uint32_t ttl;//生存时间
    uint16 rdlength;//资源数据长度
};

// 自定义域名映射表
std::unordered_map<std::string, std::string> domain_map = {
    {"example.com", "192.168.1.100"},
    {"test.com", "10.0.0.5"},
    {"mywebsite.com", "127.0.0.1"},
    {"google.com", "127.0.0.1"},
};

std::string dnsNameToStr(const unsigned char* buffer, int* offset) {
    std::string name;
    int len = 0;

    while((len = buffer[*offset]) != 0) {
        // 检查是否是指针，最高位为1
        if ((len & 0xC0) == 0xC0) {
            // 计算指针地址
            int pointer = ((len & 0x3F) << 8) | buffer[*offset + 1];
        }
    }
    (*offset)++;
    if (!name.empty()) {
        name.pop_back();
    }
    return name;
}



