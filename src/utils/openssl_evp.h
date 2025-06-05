
#ifndef OPENSSL_EVP_H
#define OPENSSL_EVP_H

// 加密
int sm4_cbc_encrypt(unsigned char *key, unsigned char *iv, unsigned char *in, int in_len, unsigned char *out, int *out_len);
// 解密
int sm4_cbc_decrypt(unsigned char *key, unsigned char *iv, unsigned char *in, int in_len, unsigned char *out, int *out_len);

// 生成key
int generate_key(unsigned char *key, int key_len);
// 生成iv
int generate_iv(unsigned char *iv, int iv_len);

#endif
