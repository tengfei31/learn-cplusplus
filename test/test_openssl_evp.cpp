#include "openssl_evp.h"
#include <gtest/gtest.h>
#include <iostream>

class OpenSSLTest : public testing::Test {
protected:
    unsigned char key[16], iv[16];

    virtual void SetUp() override {
        ASSERT_EQ(generate_key(key, 16), 1);
        ASSERT_EQ(generate_iv(iv, 16), 1);
    }
};

TEST_F(OpenSSLTest, EncryptDecryptConsistency) {
    const char* plaintext = "Hello SM4 Encryption!";
    unsigned char ciphertext[128];
    unsigned char decrypted[128];
    int cipher_len, decrypted_len;

    ASSERT_EQ(sizeof(key), 16);  // SM4需要16字节密钥
    ASSERT_EQ(sizeof(iv), 16);   // CBC需要16字节IV

    // 加密
    ASSERT_EQ(sm4_cbc_encrypt(key, iv,
              (unsigned char*)plaintext, strlen(plaintext),
              ciphertext, &cipher_len), 0);

    // 解密
    ASSERT_EQ(sm4_cbc_decrypt(key, iv,
              ciphertext, cipher_len,
              decrypted, &decrypted_len), 0);

    // 验证解密结果
    decrypted[decrypted_len] = '\0';
    std::cout << "plaintext text: " << plaintext << "\n";
    std::cout << "Decrypted text: " << decrypted << "\n";
    ASSERT_STREQ(plaintext, (char*)decrypted);
}

TEST(KeyGenerationTest, GenerateValidKey) {
    unsigned char key[32];
    EXPECT_EQ(generate_key(key, 32), 1);
}

