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

#ifndef CNN_TRASH_CLASSIFY_H
#define CNN_TRASH_CLASSIFY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "hi_comm_video.h"
#include "osd_img.h"

#if __cplusplus
extern "C" {
#endif

#define TINY_BUF_SIZE       64 // tiny buf size
#define SMALL_BUF_SIZE      128 // small buf size
#define NORM_BUF_SIZE       256 // normal buf size
#define LARGE_BUF_SIZE      1024 // normal buf size
#define HUGE_BUF_SIZE       9120 // huge buf size

/* ARGB1555 common colors */
#define ARGB1555_RED        0xFC00 // 1 11111 00000 00000
#define ARGB1555_GREEN      0x83E0 // 1 00000 11111 00000
#define ARGB1555_BLUE       0x801F // 1 00000 00000 11111
#define ARGB1555_YELLOW     0xFFE0 // 1 11111 11111 00000
#define ARGB1555_YELLOW2    0xFF00 // 1 11111 11111 00000
#define ARGB1555_WHITE      0xFFFF // 1 11111 11111 11111
#define ARGB1555_BLACK      0x8000 // 1 00000 00000 00000

/* cnn trash classify load model */
HI_S32 CnnTrashClassifyLoadModel(uintptr_t* model, OsdSet* osds);

/* cnn trash classify unload model */
HI_S32 CnnTrashClassifyUnloadModel(uintptr_t model);

/* cnn trash classify calculation */
HI_S32 CnnTrashClassifyCal(uintptr_t model, VIDEO_FRAME_INFO_S *srcFrm, VIDEO_FRAME_INFO_S *resFrm);

#ifdef __cplusplus
}
#endif
#endif
