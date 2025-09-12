#include "openssl_evp.h"

#ifdef __cplusplus
// extern  "C" {
#endif

// 生成SM2密钥
EVP_PKEY *generate_sm2_key() {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_SM2, NULL);
    if (!ctx) {
        return NULL;
    }
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }
    EVP_PKEY *pkey = NULL;
    //pkey = (EVP_PKEY*) malloc(sizeof(EVP_PKEY*));
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }
    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

// SM2签名
int sm2_sign(EVP_PKEY *pkey, const unsigned char *msg, size_t msg_len, unsigned char *sig, size_t *sig_len) {
    //EVP_PKEY_set_type(pkey, EVP_PKEY_SM2);
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        return 0;
    }
    const EVP_MD *md = EVP_sm3();
    if (!md) {
        printf("EVP_sm3() not supported\n");
        return 0;
    }
    if (EVP_DigestSignInit(ctx, NULL, md, NULL, pkey) <= 0) {
        EVP_MD_CTX_free(ctx);
        ERR_print_errors_fp(stderr);
        return 0;
    }
    if (EVP_DigestSign(ctx, sig, sig_len, msg, msg_len) <= 0) {
        EVP_MD_CTX_free(ctx);
        ERR_print_errors_fp(stderr);
        return 0;
    }
    EVP_MD_CTX_free(ctx);
    return 1;
}

// SM2验签
int sm2_verify(EVP_PKEY *pkey, const unsigned char *msg, size_t msg_len, const unsigned char *sig, size_t sig_len) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        return 0;
    }
    if (EVP_DigestVerifyInit(ctx, NULL, EVP_sm3(), NULL, pkey) <= 0) {
        EVP_MD_CTX_free(ctx);
        return 0;
    }
    int ret = 0;
    ret = EVP_DigestVerify(ctx, sig, sig_len, msg, msg_len);
    EVP_MD_CTX_free(ctx);

    return ret;
}


int sm4_ecb_encrypt(unsigned char *key, unsigned char *in, int in_len, unsigned char *out, int *out_len) {
    EVP_CIPHER_CTX *ctx = NULL;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    EVP_EncryptInit_ex(ctx, EVP_sm4_ecb(), NULL, key, NULL);

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

int sm4_ecb_decrypt(unsigned char *key, unsigned char *in, int in_len, unsigned char *out, int *out_len) {
    EVP_CIPHER_CTX *ctx = NULL;
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    EVP_DecryptInit_ex(ctx, EVP_sm4_ecb(), NULL, key, NULL);

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
