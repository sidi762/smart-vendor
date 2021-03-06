/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Copyright (c) 2022 Sidi Liang.
 *
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

#ifndef SMART_VENDOR_AI_CLASSIFICATION_H
#define SMART_VENDOR_AI_CLASSIFICATION_H

#include "sample_media_ai.h"
#include "ai_infer_process.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static HI_VOID VendorHandClassificationProcess(VIDEO_FRAME_INFO_S frm, VO_LAYER voLayer, VO_CHN voChn, AiPlugLib* g_workPlug, AicMediaInfo* g_aicMediaInfo);
HI_VOID* VendorGetVpssChnFrameAndClassify(HI_VOID* arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MEDIA_AI_H__ */
