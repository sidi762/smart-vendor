/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * This file make use the hmac to make mqtt pwd.
 * The method is use the date string to hash the device passwd
 * Take care that this implement depends on the hmac of the mbedtls
*/
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "md.h"
#include "md_internal.h"

#define CN_HMAC256_LEN_MAX (65)
#define CN_HMAC256_LEN   32
#define RIGHT_MOVE_BIT_4 (4)
#define NUMBER_9 (9)
#define DECIMAL_BASE (10)
#define STRING_LEN_TIMES (2)
#define LEN_TIMES (2)
#define OFFSET (1)
// make a byte to 2 ascii hex
static int byte2hexstr(unsigned char *bufin, int len, char *bufout)
{
    int i = 0;
    unsigned char  tmp_l = 0x0;
    unsigned char  tmp_h = 0;
    if ((bufin == NULL)||(len <= 0)||(bufout == NULL)) {
        return -1;
    }
    for (i = 0; i < len; i++) {
        tmp_h = (bufin[i] >> RIGHT_MOVE_BIT_4) & 0X0F;
        tmp_l = bufin[i] & 0x0F;
        bufout[STRING_LEN_TIMES * i] = (tmp_h > NUMBER_9) ?
            (tmp_h - DECIMAL_BASE + 'a'):(tmp_h +'0');
        bufout[STRING_LEN_TIMES * i + OFFSET] = (tmp_l > NUMBER_9) ?
            (tmp_l - DECIMAL_BASE + 'a'):(tmp_l +'0');
    }
    bufout[STRING_LEN_TIMES * len] = '\0';

    return 0;
}

int HmacGeneratePwd(const unsigned char *content, int contentLen, const unsigned char *key,
    int keyLen, unsigned char *buf)
{
    int ret = -1;
    mbedtls_md_context_t mbedtls_md_ctx;
    const mbedtls_md_info_t *md_info;
    unsigned char hash[CN_HMAC256_LEN];

    if ((key == NULL)||(content == NULL)||(buf == NULL)||
        (keyLen == 0)||(contentLen == 0)||
        (CN_HMAC256_LEN_MAX < (CN_HMAC256_LEN * LEN_TIMES + OFFSET))) {
        return ret;
    }

    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if ((md_info == NULL)||((size_t)md_info->size > CN_HMAC256_LEN)) {
        return ret;
    }

    mbedtls_md_init(&mbedtls_md_ctx);
    ret = mbedtls_md_setup(&mbedtls_md_ctx, md_info, 1);
    if (ret != 0) {
        mbedtls_md_free(&mbedtls_md_ctx);
        return ret;
    }

    (void)mbedtls_md_hmac_starts(&mbedtls_md_ctx, key, keyLen);
    (void)mbedtls_md_hmac_update(&mbedtls_md_ctx, content, contentLen);
    (void)mbedtls_md_hmac_finish(&mbedtls_md_ctx, hash);

    // <transfer the hash code to the string mode
    ret = byte2hexstr(hash, CN_HMAC256_LEN, (char *)buf);
    if (ret != 0) {
        return ret;
    }

    return ret;
}