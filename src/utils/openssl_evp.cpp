#include "openssl/evp.h"
#include "openssl/rand.h"
#include "openssl_evp.h"

#ifdef __cplusplus
// extern  "C" {
#endif

int sm4_cbc_encrypt(unsigned char *key, unsigned char *iv, unsigned char *in, int in_len, unsigned char *out, int *out_len) {
    EVP_CIPHER_CTX *ctx = NULL;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    EVP_EncryptInit_ex(ctx, EVP_sm4_cbc(), NULL, key, iv);
    //EVP_CIPHER_CTX_set_padding(ctx, 0);

    if (!EVP_EncryptUpdate(ctx, out, out_len, in, in_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    int tmp_len = 0;
    if (!EVP_EncryptFinal(ctx, out + *out_len, &tmp_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    *out_len += tmp_len;
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

int sm4_cbc_decrypt(unsigned char *key, unsigned char *iv, unsigned char *in, int in_len, unsigned char *out, int *out_len) {
    EVP_CIPHER_CTX *ctx = NULL;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    EVP_DecryptInit_ex(ctx, EVP_sm4_cbc(), NULL, key, iv);

    if (!EVP_DecryptUpdate(ctx, out, out_len, in, in_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    int tmp_len = 0;
    int ret = 0;
    ret = EVP_DecryptFinal(ctx, out + *out_len, &tmp_len);
    if (ret <= 0) {
        EVP_CIPHER_CTX_free(ctx);
        return -2;
    }
    *out_len += tmp_len;
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

// 生成key,默认生成16位
int generate_key(unsigned char *key, int key_len) {
    if (key_len <= 0) {
        key_len = 16;
    }
    return RAND_bytes(key, key_len);
}
// 生成iv,默认生成16位
int generate_iv(unsigned char *iv, int iv_len) {
    if (iv_len <= 0) {
        iv_len = 16;
    }
    return RAND_bytes(iv, iv_len);
}

#ifdef __cplusplus
// } //   extern "C"
#endif
