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

#ifndef HAND_CLASSIFY_H
#define HAND_CLASSIFY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "hi_comm_video.h"

#if __cplusplus
extern "C" {
#endif

/* load hand detect and classify model */
HI_S32 Yolo2HandDetectResnetClassifyLoad(uintptr_t* model);

/* unload hand detect and classify model */
HI_S32 Yolo2HandDetectResnetClassifyUnload(uintptr_t model);

/* hand detect and classify calculation */
HI_S32 Yolo2HandDetectResnetClassifyCal(uintptr_t model, VIDEO_FRAME_INFO_S *srcFrm, VIDEO_FRAME_INFO_S *dstFrm);

#ifdef __cplusplus
}
#endif
#endif
