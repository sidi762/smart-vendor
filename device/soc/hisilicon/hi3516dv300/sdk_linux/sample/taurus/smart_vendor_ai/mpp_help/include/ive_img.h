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

#ifndef IVE_IMG_H
#define IVE_IMG_H

#include <stdbool.h>
#include <stdint.h>

#include "hi_common.h"
#include "hi_comm_ive.h"
#include "hi_comm_video.h"
#include "hi_comm_venc.h"
#include "hi_ive.h"
#include "mpi_ive.h"
#include "ai_infer_process.h"

#if __cplusplus
extern "C" {
#endif

/* consts */
#define IMG_FULL_CHN    3 // Full channel / three channel, for YUV444, RGB888
#define IMG_HALF_CHN    2 // Half channel, for YUV420/422
#define THREE_TIMES     3
#define TWO_TIMES       2

/* Byte alignment calculation */
HI_U32 HiAlign16(HI_U32 num);
HI_U32 HiAlign32(HI_U32 num);

/* Alignment type */
typedef enum AlignType {
    ALIGN_TYPE_2 = 2, // Align by 2 bytes
    ALIGN_TYPE_16 = 16, // Align by 16 bytes
    ALIGN_TYPE_32 = 32, // Align by 32 bytes
} AlignType;

/* Create ive image buffer based on type and size */
int IveImgCreate(IVE_IMAGE_S* img,
    IVE_IMAGE_TYPE_E enType, uint32_t width, uint32_t height);

/*
 * video frame to ive image.
 * Copy the data pointer, do not copy the data.
 */
int FrmToOrigImg(const VIDEO_FRAME_INFO_S* frm, IVE_IMAGE_S *img);

/* yuv file crop */
int ImgYuvCrop(const IVE_IMAGE_S *src, IVE_IMAGE_S *dst, const RectBox* origBox);

/* Destory ive Image */
void IveImgDestroy(IVE_IMAGE_S* img);

int OrigImgToFrm(const IVE_IMAGE_S *img, VIDEO_FRAME_INFO_S* frm);

#ifdef __cplusplus
}
#endif
#endif
