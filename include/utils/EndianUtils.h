#ifndef ENDIAN_UTILS_H
#define ENDIAN_UTILS_H 

#include <cstdint>
#include <iostream>

class EndianUtils {
  public:
    // 判断当前环境是否是小端字节序
    static bool isLittleEndian() {
        uint16_t num = 0x1;
        return *(char *)&num == 1;
    }
    // 大端转小端
    static uint32_t conversion32(uint32_t val) {
        return ((val & 0xFF000000) >> 24 | (val & 0x00FF0000) >> 8 |
                (val & 0x0000FF00) << 8 | (val & 0x000000FF) << 24);
    }
    // 大端转小端
    static uint32_t bigToLittle32(uint32_t val) { return conversion32(val); }
    // 小端转大端，其实就是字节序反转
    static uint32_t littleToBig32(uint32_t val) { return conversion32(val); }
};


#endif
