/*
 ** SQLCipher
 ** http://sqlcipher.net
 **
 ** Copyright (c) 2008 - 2013, ZETETIC LLC
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are met:
 **     * Redistributions of source code must retain the above copyright
 **       notice, this list of conditions and the following disclaimer.
 **     * Redistributions in binary form must reproduce the above copyright
 **       notice, this list of conditions and the following disclaimer in the
 **       documentation and/or other materials provided with the distribution.
 **     * Neither the name of the ZETETIC LLC nor the
 **       names of its contributors may be used to endorse or promote products
 **       derived from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY ZETETIC LLC ''AS IS'' AND ANY
 ** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL ZETETIC LLC BE LIABLE FOR ANY
 ** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 ** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 ** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 ** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 */
/* BEGIN SQLCIPHER */
#ifdef SQLITE_HAS_CODEC
#ifdef SQLCIPHER_CRYPTO_MBEDTLS

// HACK: Get around including parse.h, which is ignored in the sqlcipher git repo
// but included by sqliteInt.h
typedef struct Pager Pager;
#ifndef UINT32_TYPE
# ifdef HAVE_UINT32_T
#  define UINT32_TYPE uint32_t
# else
#  define UINT32_TYPE unsigned int
# endif
#endif
typedef UINT32_TYPE Pgno;
typedef struct Db Db;

#include "sqlite3.h"
#include "crypto.h"
#include "sqlcipher.h"

#include <string.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/md.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/cipher.h>
#include <assert.h>

#if defined(_MSC_VER) && defined(MBEDTLS_NO_PLATFORM_ENTROPY)
#include <Windows.h>
#include <bcrypt.h>
static int uwp_entropy_poll(void *data, unsigned char *output, size_t len,
                            size_t *olen)
{
    NTSTATUS status = BCryptGenRandom(NULL, output, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (status < 0) {
        return(MBEDTLS_ERR_ENTROPY_SOURCE_FAILED);
    }
    
    return 0;
}
#endif

typedef struct {
    const mbedtls_cipher_info_t *cipher_info;
} mbedtls_ctx;

static int mbedtls_initialized = 0;
static mbedtls_entropy_context mbedtls_entropy;
static mbedtls_ctr_drbg_context mbedtls_ctr_drbg;
static sqlite3_mutex* mbedtls_rand_mutex = NULL;

static int sqlcipher_mbedtls_add_random(void *ctx, void *buffer, int length) {
#ifndef SQLCIPHER_MBEDTLS_NO_MUTEX_RAND
    sqlite3_mutex_enter(mbedtls_rand_mutex);
#endif
    
    mbedtls_entropy_update_manual(&mbedtls_entropy, (const unsigned char*)buffer, length);
    
#ifndef SQLCIPHER_MBEDTLS_NO_MUTEX_RAND
    sqlite3_mutex_leave(mbedtls_rand_mutex);
#endif
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_activate(void *ctx) {
    int rc = 0;
    
    if(!mbedtls_initialized) {
        mbedtls_entropy_init(&mbedtls_entropy);
#if defined(_MSC_VER) && defined(MBEDTLS_NO_PLATFORM_ENTROPY)
        mbedtls_entropy_add_source(&mbedtls_entropy, uwp_entropy_poll, NULL, 32, MBEDTLS_ENTROPY_SOURCE_STRONG);
#endif
        mbedtls_ctr_drbg_init(&mbedtls_ctr_drbg);
        const unsigned char* val = (const unsigned char *)"Salty McNeal";
        rc = mbedtls_ctr_drbg_seed(&mbedtls_ctr_drbg, mbedtls_entropy_func, &mbedtls_entropy, val, strlen((const char*)val));
        if (rc != 0) {
            return rc;
        }
        
        mbedtls_initialized = 1;
    }
    
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_deactivate(void *ctx) {
    if(mbedtls_initialized) {
        mbedtls_ctr_drbg_free(&mbedtls_ctr_drbg);
        mbedtls_entropy_free(&mbedtls_entropy);
        mbedtls_initialized = 0;
    }
    
    return SQLITE_OK;
}

static const char* sqlcipher_mbedtls_get_provider_name(void *ctx) {
    return "mbedtls";
}

/* generate a defined number of random bytes */
static int sqlcipher_mbedtls_random (void *ctx, void *buffer, int length) {
    int rc = 0;
#ifndef SQLCIPHER_mbedtls_NO_MUTEX_RAND
    sqlite3_mutex_enter(mbedtls_rand_mutex);
#endif
    
    rc = mbedtls_ctr_drbg_random(&mbedtls_ctr_drbg, (unsigned char*)buffer, length);
    
#ifndef SQLCIPHER_mbedtls_NO_MUTEX_RAND
    sqlite3_mutex_leave(mbedtls_rand_mutex);
#endif
    return (rc == 0) ? SQLITE_OK : SQLITE_ERROR;
}

static int sqlcipher_mbedtls_hmac(void *ctx, unsigned char *hmac_key, int key_sz, unsigned char *in, int in_sz, unsigned char *in2, int in2_sz, unsigned char *out) {
    const mbedtls_md_info_t* hashInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    
    mbedtls_md_context_t context;
    mbedtls_md_init_ctx(&context, hashInfo);
    mbedtls_md_hmac_starts(&context, hmac_key, key_sz);
    mbedtls_md_hmac_update(&context, in, in_sz);
    mbedtls_md_hmac_update(&context, in2, in2_sz);
    mbedtls_md_hmac_finish(&context, out);
    mbedtls_md_free(&context);
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_kdf(void *ctx, const unsigned char *pass, int pass_sz, unsigned char* salt, int salt_sz, int workfactor, int key_sz, unsigned char *key) {
    const mbedtls_md_info_t* hashInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    
    mbedtls_md_context_t context;
    mbedtls_md_init_ctx(&context, hashInfo);
    mbedtls_pkcs5_pbkdf2_hmac(&context, pass, pass_sz, salt, salt_sz, workfactor, key_sz, key);
    mbedtls_md_free(&context);
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_cipher(void *ctx, int mode, unsigned char *key, int key_sz, unsigned char *iv, unsigned char *in, int in_sz, unsigned char *out) {
    size_t csz;
    const mbedtls_cipher_info_t* cipher_info = ((mbedtls_ctx *)ctx)->cipher_info;
    mbedtls_cipher_context_t context;
    mbedtls_cipher_init(&context);
    mbedtls_cipher_setup(&context, cipher_info);
    mbedtls_cipher_set_padding_mode(&context, MBEDTLS_PADDING_NONE);
    mbedtls_cipher_setkey(&context, key, key_sz * 8, mode ? MBEDTLS_ENCRYPT : MBEDTLS_DECRYPT);
    mbedtls_cipher_crypt(&context, iv, cipher_info->iv_size, in, in_sz, out, &csz);
    mbedtls_cipher_free(&context);
    assert(in_sz == csz);
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_set_cipher(void *ctx, const char *cipher_name) {
    mbedtls_ctx *o_ctx = (mbedtls_ctx *)ctx;
    const mbedtls_cipher_info_t* cipher = mbedtls_cipher_info_from_string(cipher_name);
    if(cipher != NULL) {
        o_ctx->cipher_info = cipher;
    }
    
    return cipher != NULL ? SQLITE_OK : SQLITE_ERROR;
}

static const char* sqlcipher_mbedtls_get_cipher(void *ctx) {
    return ((mbedtls_ctx *)ctx)->cipher_info->name;
}

static int sqlcipher_mbedtls_get_key_sz(void *ctx) {
    return ((mbedtls_ctx *)ctx)->cipher_info->key_bitlen / 8;
}

static int sqlcipher_mbedtls_get_iv_sz(void *ctx) {
    return ((mbedtls_ctx *)ctx)->cipher_info->iv_size;
}

static int sqlcipher_mbedtls_get_block_sz(void *ctx) {
    return ((mbedtls_ctx *)ctx)->cipher_info->block_size;
}

static int sqlcipher_mbedtls_get_hmac_sz(void *ctx) {
    return mbedtls_md_get_size(mbedtls_md_info_from_type(MBEDTLS_MD_SHA1));
}

static int sqlcipher_mbedtls_ctx_copy(void *target_ctx, void *source_ctx) {
    memcpy(target_ctx, source_ctx, sizeof(mbedtls_ctx));
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_ctx_cmp(void *c1, void *c2) {
    return ((mbedtls_ctx *)c1)->cipher_info == ((mbedtls_ctx *)c2)->cipher_info;
}

static int sqlcipher_mbedtls_ctx_init(void **ctx) {
    *ctx = sqlcipher_malloc(sizeof(mbedtls_ctx));
    if(*ctx == NULL) return SQLITE_NOMEM;
    sqlcipher_mbedtls_activate(*ctx);
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_ctx_free(void **ctx) {
    sqlcipher_mbedtls_deactivate(*ctx);
    sqlcipher_free(*ctx, sizeof(mbedtls_ctx));
    return SQLITE_OK;
}

static int sqlcipher_mbedtls_fips_status(void *ctx) {
    return 0;
}

int sqlcipher_mbedtls_setup(sqlcipher_provider *p) {
    p->activate = sqlcipher_mbedtls_activate;
    p->deactivate = sqlcipher_mbedtls_deactivate;
    p->get_provider_name = sqlcipher_mbedtls_get_provider_name;
    p->random = sqlcipher_mbedtls_random;
    p->hmac = sqlcipher_mbedtls_hmac;
    p->kdf = sqlcipher_mbedtls_kdf;
    p->cipher = sqlcipher_mbedtls_cipher;
    p->set_cipher = sqlcipher_mbedtls_set_cipher;
    p->get_cipher = sqlcipher_mbedtls_get_cipher;
    p->get_key_sz = sqlcipher_mbedtls_get_key_sz;
    p->get_iv_sz = sqlcipher_mbedtls_get_iv_sz;
    p->get_block_sz = sqlcipher_mbedtls_get_block_sz;
    p->get_hmac_sz = sqlcipher_mbedtls_get_hmac_sz;
    p->ctx_copy = sqlcipher_mbedtls_ctx_copy;
    p->ctx_cmp = sqlcipher_mbedtls_ctx_cmp;
    p->ctx_init = sqlcipher_mbedtls_ctx_init;
    p->ctx_free = sqlcipher_mbedtls_ctx_free;
    p->add_random = sqlcipher_mbedtls_add_random;
    p->fips_status = sqlcipher_mbedtls_fips_status;
    return SQLITE_OK;
}

#endif
#endif
/* END SQLCIPHER */
