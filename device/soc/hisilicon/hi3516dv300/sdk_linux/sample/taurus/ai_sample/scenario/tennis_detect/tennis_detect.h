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

#ifndef TENNIS_DETECT_H
#define TENNIS_DETECT_H

#include <iostream>
#include "sample_comm_nnie.h"

#if __cplusplus
extern "C" {
#endif

typedef struct tagIPC_IMAGE {
    HI_U64 u64PhyAddr;
    HI_U64 u64VirAddr;
    HI_U32 u32Width;
    HI_U32 u32Height;
} IPC_IMAGE;

class tennis_detect {
public:
    HI_S32 TennisDetectLoad(uintptr_t* model);
    HI_S32 TennisDetectUnload(uintptr_t model);
    /* Tennis detect calculation */
    HI_S32 TennisDetectCal(uintptr_t model, VIDEO_FRAME_INFO_S *srcFrm, VIDEO_FRAME_INFO_S *dstFrm);
};

#ifdef __cplusplus
}
#endif
#endif