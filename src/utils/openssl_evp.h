
#ifndef OPENSSL_EVP_SM4
#define OPENSSL_EVP_SM4

#include "openssl/evp.h"
#include "openssl/rand.h"
#include <openssl/err.h>

/** SM2 start */
// 生成SM2密钥
EVP_PKEY *generate_sm2_key();
// SM2签名
int sm2_sign(EVP_PKEY *pkey, const unsigned char *msg, size_t msg_len, unsigned char *sig, size_t *sig_len);
// SM2验签
int sm2_verify(EVP_PKEY *pkey, const unsigned char *msg, size_t msg_len, const unsigned char *sig, size_t sig_len);
/** SM2 end */

/** SM4 start */
// 加密
int sm4_ecb_encrypt(unsigned char *key, unsigned char *in, int in_len, unsigned char *out, int *out_len);
int sm4_cbc_encrypt(unsigned char *key, unsigned char *iv, unsigned char *in, int in_len, unsigned char *out, int *out_len);
// 解密
int sm4_cbc_decrypt(unsigned char *key, unsigned char *iv, unsigned char *in, int in_len, unsigned char *out, int *out_len);
int sm4_ecb_decrypt(unsigned char *key, unsigned char *in, int in_len, unsigned char *out, int *out_len);
/** SM4 end */

// 生成key
int generate_key(unsigned char *key, int key_len);
// 生成iv
int generate_iv(unsigned char *iv, int iv_len);

#endif
