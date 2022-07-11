/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef POSIX_HELP_H
#define POSIX_HELP_H

#include <errno.h>
#include <strings.h>
#include <stdint.h>
#include <netinet/in.h>
#include <pthread.h>

#if __cplusplus
extern "C" {
#endif

/*
 * usleep time length
 * The unit of usleep() is microseconds
 */
#define HI_USLEEP_MS    1000

/* Socketpair channel */
typedef struct SkPair {
    int in; // in fd
    int out; // out fd
} SkPair;

/* SkPair create */
int SkPairCreate(SkPair* chn);

/* Read complete message */
int FdReadMsg(int fd, HI_VOID* msgBuf, int msgSize);

/* Write complete message */
int FdWriteMsg(int fd, const HI_VOID* msgData, int msgLen);

/* SkPair destroy */
void SkPairDestroy(SkPair* chn);

void RecurMutexInit(pthread_mutex_t* mutex);

#ifdef __cplusplus
}
#endif
#endif
