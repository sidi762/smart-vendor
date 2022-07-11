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

#ifndef AI_INFER_PROCESS_H
#define AI_INFER_PROCESS_H

#include <stdint.h>
#include "sample_comm_nnie.h"

#if __cplusplus
extern "C" {
#endif

#define HI_PER_BASE         100
#define HI_OVEN_BASE        2 // Even base

/* rect box */
typedef struct RectBox {
    int xmin;
    int xmax;
    int ymin;
    int ymax;
} RectBox;

/* recognized number information */
typedef struct RecogNumInfo {
    uint32_t num; // Recognized numeric value, 0~9
    uint32_t score; // The credibility score of a number, the value range of which is defined by a specific model
} RecogNumInfo;

/* plug related information */
typedef struct AiPlugLib {
    int width;
    int height;
    uintptr_t model;
} AiPlugLib;

/* Information of detected objects */
typedef struct DetectObjInfo {
    int cls; // The category of the object, an integer> 0
    RectBox box; // The rectangular area of the object (pixels)
    float score; // Object's credibility score
} DetectObjInfo;

void CnnDestroy(SAMPLE_SVP_NNIE_CFG_S *self);
int CnnCreate(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile);

/* Calculate a U8C1 image */
int CnnCalU8c1Img(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, RecogNumInfo resBuf[], int resSize, int* resLen);

/* creat yolo2 model basad mode file */
int Yolo2Create(SAMPLE_SVP_NNIE_CFG_S **model, const char* modelFile);

/* destory yolo2 model */
void Yolo2Destory(SAMPLE_SVP_NNIE_CFG_S *self);

/* cal U8C3 image */
int Yolo2CalImg(SAMPLE_SVP_NNIE_CFG_S* self,
    const IVE_IMAGE_S *img, DetectObjInfo resBuf[], int resSize, int* resLen);

#ifdef __cplusplus
}
#endif
#endif
