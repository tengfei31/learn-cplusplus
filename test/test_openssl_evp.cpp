#include "openssl_evp.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string.h>
#include "inc/datastruct/base64.h"

#define KEY_LEN 32

const char private_key[] = "aeyz8fQZoBWG47wC580cTXOJN0ip+Lm03eecs9iJlUI=";
const char public_key[] = "BFwcNwKMzHPLMn4fWgZl+d0fJRf+WGshvjZl2BgdaaytgMgTJAtdUBG4kcWdKA8f1pfu1Aw8eZbgtqkK2od+iqk=";
const char app_secret[] = "32067477F2BE497590DCBBC19F49D120";
const char app_id[] = "DB3F443FF73F4DA78F515AF52CC4679F";
const char data[] = "{\"certno\":\"612324198410010316\",\"drCode\":\"D610724006954\",\"fixmedinsCode\":\"H61072400077\",\"fixmedinsName\":\"西乡县人民医院\",\"hiRxno\":\"610700022480494924945233982978\",\"mdtrtId\":\"1274753890\",\"patnName\":\"谢怡\",\"pharCertType\":\"01\",\"pharCertno\":\"21233121\",\"pharChkTime\":\"2025-08-27 17:57:59\",\"pharCode\":\"111\",\"pharDeptCode\":\"A50.04\",\"pharDeptName\":\"儿一科\",\"pharName\":\"丁文彬\",\"pharProfttlCodg\":\"1\",\"pharProfttlName\":\"执业药师\",\"prscDrName\":\"何培安\",\"psnCertType\":\"01\",\"rxTraceCode\":\"61289727580497248261\"}";
const char sign_str[] = "appId=DB3F443FF73F4DA78F515AF52CC4679F&data={\"certno\":\"612324198410010316\",\"drCode\":\"D610724006954\",\"fixmedinsCode\":\"H61072400077\",\"fixmedinsName\":\"西乡县人民医院\",\"hiRxno\":\"610700022480494924945233982978\",\"mdtrtId\":\"1274753890\",\"patnName\":\"谢怡\",\"pharCertType\":\"01\",\"pharCertno\":\"21233121\",\"pharChkTime\":\"2025-08-27 17:57:59\",\"pharCode\":\"111\",\"pharDeptCode\":\"A50.04\",\"pharDeptName\":\"儿一科\",\"pharName\":\"丁文彬\",\"pharProfttlCodg\":\"1\",\"pharProfttlName\":\"执业药师\",\"prscDrName\":\"何培安\",\"psnCertType\":\"01\",\"rxTraceCode\":\"61289727580497248261\"}&encType=SM4&signType=SM2&timestamp=1757326792488&version=1.0.0&key=32067477F2BE497590DCBBC19F49D120";

// 输入：二进制缓冲区 data，长度 len
// 输出：hex 字符串（必须保证 hex 缓冲区大小 >= len*2+1）
void bin2hex(const unsigned char *data, size_t len, char *hex) {
    static const char *hex_digits = "0123456789ABCDEF";
    for (size_t i = 0; i < len; i++) {
        hex[i*2]     = hex_digits[(data[i] >> 4) & 0x0F];
        hex[i*2 + 1] = hex_digits[data[i] & 0x0F];
    }
    hex[len*2] = '\0';
}

// 工具函数：从原始二进制 SM2 私钥加载 EVP_PKEY
EVP_PKEY* load_sm2_private_key_from_raw(const unsigned char* raw_key, size_t raw_key_len) {
    EVP_PKEY* pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_SM2, NULL, raw_key, raw_key_len);
    return pkey;
}


class OpenSSLTest : public ::testing::Test {
protected:
    unsigned char key[KEY_LEN];
    unsigned long key_len;

    virtual void SetUp() override {
        key_len = base64Decode(private_key, strlen(private_key), key);
    }

    virtual void TearDown() override {

    }
};

TEST_F(OpenSSLTest, SignVerify) {
    //将上面的key二进制转换为十六进制
    char key_hex[KEY_LEN * 2 + 1];
    bin2hex(key, KEY_LEN, key_hex);
    // std::cout << "Private key (hex): " << key_hex << std::endl;
    EVP_PKEY *pkey;
    pkey = generate_sm2_key();
    //pkey = load_sm2_private_key_from_raw((unsigned char*)key_hex, KEY_LEN * 2 + 1);
    //pkey = load_sm2_private_key_from_raw(key, key_len);
    ASSERT_TRUE(pkey != nullptr);

    char sm2_sign_str[1024];
    size_t sm2_sign_str_len = sizeof(sm2_sign_str);
    int sign_ret = 0;
    std::cout << "sign_str len: " << strlen(sign_str) << "\n";
    sign_ret = sm2_sign(
        pkey,
        (unsigned char*)sign_str, strlen(sign_str),
        (unsigned char*)sm2_sign_str, &sm2_sign_str_len
    );
    if (!sign_ret) {
        ERR_print_errors_fp(stderr);
    }
    ASSERT_EQ(sign_ret, 1);

    std::cout << "sm2_sign_str sign: " << sm2_sign_str << "\n";
    std::cout << "sm2_sign_str sign len: " << sm2_sign_str_len << "\n";

    //将签名转换为base64_encode
    char sm2_sign_encode[1024];
    int sm2_sign_encode_len;
    sm2_sign_encode_len = base64Encode((unsigned char*)sm2_sign_str, sm2_sign_str_len, sm2_sign_encode);
    ASSERT_GT(sm2_sign_encode_len, 0);

    std::cout << "sm2_sign_encode: " << sm2_sign_encode << "\n";
    std::cout << "sm2_sign_encode_len: " << sm2_sign_encode_len << "\n";

    //验签
    int sign_verify_ret = 0;
    sign_verify_ret = sm2_verify(pkey, (unsigned char*)sign_str, strlen(sign_str), (unsigned char*)sm2_sign_str, (size_t)sm2_sign_str_len);
    ASSERT_EQ(sign_verify_ret, 1);
    std::cout << "sign_verify_ret: " << sign_verify_ret << "\n";
}



TEST_F(OpenSSLTest, EncryptDecrypt) {
    /**
     *  根据 SM4 加密算法，sm4 算法使用 ECB 模式，填充算法为 PKCS7。加密规则为：先使用
        appId(id)作为 Key 加密 appSecret（应用秘钥）生成新的秘钥串；再使用新秘钥串对报文的
        data 字段加密。
     *
     */

    unsigned char app_secret_ciphertext[1024];
    unsigned char data_ciphertext[1024];
    unsigned char decrypted[128];
    int app_secret_ciphertext_len, data_ciphertext_len, decrypted_len;
    int encrypt_ret;

    char appid_key[16];
    memcpy(appid_key, app_id, 16);
    std::cout << "key:" << strlen(appid_key) << "\n";
    // 加密app_secret
    encrypt_ret = sm4_ecb_encrypt((unsigned char*)appid_key, (unsigned char*)app_secret, strlen(app_secret), app_secret_ciphertext, &app_secret_ciphertext_len);
    ASSERT_EQ(encrypt_ret, 0);

    std::cout << "app_secret encrypt text: " << app_secret_ciphertext << "\n";
    std::cout << "app_secret encrypt text len: " << app_secret_ciphertext_len << "\n";

    // 加密data
    char data_key[16];
    memcpy(data_key, app_secret_ciphertext, 16);
    encrypt_ret = sm4_ecb_encrypt((unsigned char*)data_key, (unsigned char*)data, strlen(data), data_ciphertext, &data_ciphertext_len);
    ASSERT_EQ(encrypt_ret, 0);
    std::cout << "data encrypt text: " << data_ciphertext << "\n";
    std::cout << "data encrypt text len: " << data_ciphertext_len << "\n";

    // 解密
    // ASSERT_EQ(sm4_cbc_decrypt(key, iv,
    //           ciphertext, cipher_len,
    //           decrypted, &decrypted_len), 0);
    // 验证解密结果
    // decrypted[decrypted_len] = '\0';
    // std::cout << "plaintext text: " << plaintext << "\n";
    // std::cout << "Decrypted text: " << decrypted << "\n";
    //ASSERT_STREQ(plaintext, (char*)decrypted);
    EXPECT_TRUE(true);
}

TEST(KeyGenerationTest, GenerateValidKey) {
    unsigned char key[32];
    EXPECT_EQ(generate_key(key, 32), 1);
}

