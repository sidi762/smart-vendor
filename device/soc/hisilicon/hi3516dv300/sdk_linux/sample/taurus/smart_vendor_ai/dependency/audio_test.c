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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/eventfd.h>

#include "hi_resampler_api.h"
#include "sample_comm.h"
#include "acodec.h"
#include "audio_aac_adp.h"
#include "audio_dl_adp.h"
#include "audio_mp3_adp.h"
#include "sample_comm_ive.h"
#include "sample_media_ai.h"

static PAYLOAD_TYPE_E gs_enPayloadType = PT_AAC;
static HI_BOOL gs_bAioReSample = HI_FALSE;
static HI_BOOL gs_bUserGetMode = HI_FALSE;
static HI_BOOL gs_bAoVolumeCtrl = HI_FALSE;
static AUDIO_SAMPLE_RATE_E g_in_sample_rate = AUDIO_SAMPLE_RATE_BUTT;
static AUDIO_SAMPLE_RATE_E g_out_sample_rate = AUDIO_SAMPLE_RATE_BUTT;
/* 0: close, 1: record, 2:reserved, 3:talkv2 */
static HI_U32 g_ai_vqe_type = 1;
static HI_BOOL g_sample_audio_exit = HI_FALSE;

static AI_RECORDVQE_CONFIG_S g_ai_record_vqe_attr;
static AI_TALKVQEV2_CONFIG_S g_ai_talkv2_vqe_attr;

static int audio_wait_quit(int quitFd)
{
    eventfd_t eval;
    int ret = eventfd_read(quitFd, &eval);
    assert(ret == 0);

    return 0;
}

/* function : PT Number to String */
static char *SAMPLE_AUDIO_Pt2Str(PAYLOAD_TYPE_E enType)
{
    if (enType == PT_G711A) {
        return "g711a";
    } else if (enType == PT_G711U) {
        return "g711u";
    } else if (enType == PT_ADPCMA) {
        return "adpcm";
    } else if (enType == PT_G726) {
        return "g726";
    } else if (enType == PT_LPCM) {
        return "pcm";
    } else if (enType == PT_AAC) {
        return "aac";
    } else if (enType == PT_MP3) {
        return "mp3";
    } else {
        return "data";
    }
}

/* function : Add dynamic load path */
#ifndef HI_VQE_USE_STATIC_MODULE_REGISTER
static HI_VOID SAMPLE_AUDIO_AddLibPath(HI_VOID)
{
    HI_S32 s32Ret;
    HI_CHAR aszLibPath[FILE_NAME_LEN] = {0};
#if defined(__HuaweiLite__) && (!defined(__OHOS__))
    s32Ret = snprintf_s(aszLibPath, FILE_NAME_LEN, FILE_NAME_LEN - 1, "/sharefs/");
    if (s32Ret <= EOK) {
        SAMPLE_PRT("\n snprintf_s fail! ret = 0x%x", s32Ret);
        return;
    }
#else
#endif
    s32Ret = Audio_Dlpath(aszLibPath);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT("%s: add lib path %s failed\n", __FUNCTION__, aszLibPath);
    }
    return;
}
#endif

/* function : Open Adec File */
static FILE* SAMPLE_AUDIO_OpenAdecFile_AAC(int num, ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
    FILE* pfd;
    HI_CHAR aszFileName[FILE_NAME_LEN] = {0};

    /* create file for save stream */
#ifdef __HuaweiLite__
    if (snprintf_s(aszFileName, FILE_NAME_LEN, FILE_NAME_LEN - 1, "/sharefs/audio_chn%d.%s",
        AdChn, SAMPLE_AUDIO_Pt2Str(enType)) < 0) {
        HI_ASSERT(0);
    }
#else
    if (num == -1) {
        if (snprintf_s(aszFileName, FILE_NAME_LEN, FILE_NAME_LEN - 1,
            "audio_chn%d.%s", AdChn, SAMPLE_AUDIO_Pt2Str(enType)) < 0) {
            HI_ASSERT(0);
        }
    } else {
        SAMPLE_PRT("SAMPLE_AUDIO_OpenAdecFile_AAC\n");
        SAMPLE_PRT("filename:%d.%s\n", num, SAMPLE_AUDIO_Pt2Str(enType));
        if (snprintf_s(aszFileName, FILE_NAME_LEN, FILE_NAME_LEN - 1,
            "/userdata/aac_file/%d.%s", num, SAMPLE_AUDIO_Pt2Str(enType)) < 0) {
            HI_ASSERT(0);
        }
    }
#endif

    pfd = fopen(aszFileName, "rb");
    if (pfd == NULL) {
        SAMPLE_PRT("%s: open file %s failed\n", __FUNCTION__, aszFileName);
        return NULL;
    }
    SAMPLE_PRT("open stream file:\"%s\" for adec ok\n", aszFileName);
    return pfd;
}

static hi_void sample_audio_adec_ao_init_param(AIO_ATTR_S *attr)
{
    attr->enSamplerate = AUDIO_SAMPLE_RATE_48000;
    attr->u32FrmNum = FPS_30;
    attr->enBitwidth = AUDIO_BIT_WIDTH_16;
    attr->enWorkmode = AIO_MODE_I2S_MASTER;
    attr->enSoundmode = AUDIO_SOUND_MODE_STEREO;
    attr->u32ChnCnt = 2; /* 2: chn num */
    attr->u32ClkSel = 1;
    attr->enI2sType = AIO_I2STYPE_INNERCODEC;
    attr->u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    attr->u32EXFlag = 0;

    gs_bAioReSample = HI_FALSE;
    g_in_sample_rate = AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = AUDIO_SAMPLE_RATE_BUTT;
}

static HI_VOID SAMPLE_AUDIO_AdecAoInner(HI_S32 num, HI_S32 fd, AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    HI_S32 s32Ret;
    FILE *pfd = NULL;

    s32Ret = SAMPLE_COMM_AUDIO_AoBindAdec(AoDev, AoChn, AdChn);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
        return;
    }

    pfd = SAMPLE_AUDIO_OpenAdecFile_AAC(num, AdChn, gs_enPayloadType);
    if (pfd == NULL) {
        SAMPLE_PRT(HI_FAILURE);
        SAMPLE_CHECK_EXPR_GOTO(pfd == NULL, ADECAO_ERR0,
            "SAMPLE_AUDIO_OpenAdecFile_AAC FAIL, ret=%#x\n", s32Ret);
    }

    s32Ret = SAMPLE_COMM_AUDIO_CreateTrdFileAdec(AdChn, pfd);
    if (s32Ret != HI_SUCCESS) {
        fclose(pfd);
        pfd = HI_NULL;
        SAMPLE_PRT(s32Ret);
        SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, ADECAO_ERR0,
            "SAMPLE_COMM_AUDIO_CreateTrdFileAdec FAIL, ret=%#x\n", s32Ret);
    }

    printf("bind adec:%d to ao(%d,%d) ok \n", AdChn, AoDev, AoChn);

    if (num == -1) {
        audio_wait_quit(fd);
    } else {
        sleep(3); // 3: sleep time
    }

    s32Ret = SAMPLE_COMM_AUDIO_DestroyTrdFileAdec(AdChn);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
        return;
    }

ADECAO_ERR0:
    s32Ret = SAMPLE_COMM_AUDIO_AoUnbindAdec(AoDev, AoChn, AdChn);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
    }

    return;
}

/* function : file -> Adec -> Ao */
static HI_S32 SAMPLE_AUDIO_AdecAo(HI_S32 num, HI_S32 quitFd)
{
    HI_S32 s32Ret, s32AoChnCnt;
    const AO_CHN AoChn = 0;
    const ADEC_CHN AdChn = 0;
    AIO_ATTR_S stAioAttr;
    AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;

    sample_audio_adec_ao_init_param(&stAioAttr);

    s32Ret = SAMPLE_COMM_AUDIO_StartAdec(AdChn, gs_enPayloadType);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, ADECAO_ERR3,
        "SAMPLE_COMM_AUDIO_StartAdec FAIL, ret=%#x\n", s32Ret);

    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, g_in_sample_rate, gs_bAioReSample);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, ADECAO_ERR2,
        "SAMPLE_COMM_AUDIO_StartAo FAIL, ret=%#x\n", s32Ret);

    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, ADECAO_ERR1,
        "SAMPLE_COMM_AUDIO_CfgAcodec FAIL, ret=%#x\n", s32Ret);

    SAMPLE_AUDIO_AdecAoInner(num, quitFd, AoDev, AoChn, AdChn);

ADECAO_ERR1:
    s32Ret = SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
    }
ADECAO_ERR2:
    s32Ret = SAMPLE_COMM_AUDIO_StopAdec(AdChn);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
    }

ADECAO_ERR3:
    return s32Ret;
}

static HI_VOID SAMPLE_AUDIO_StartAdecAo(HI_BOOL bSendAdec, AUDIO_DEV AoDev, AENC_CHN AeChn, AIO_ATTR_S *pstAioAttr)
{
    HI_S32 s32Ret, s32AoChnCnt;
    const AO_CHN AoChn = 0;
    const ADEC_CHN AdChn = 0;
    FILE *pfd = NULL;

    if (bSendAdec == HI_TRUE) {
        s32Ret = SAMPLE_COMM_AUDIO_StartAdec(AdChn, gs_enPayloadType);
        if (s32Ret != HI_SUCCESS) {
            return;
        }

        s32AoChnCnt = pstAioAttr->u32ChnCnt;
        s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, pstAioAttr, g_in_sample_rate, gs_bAioReSample);
        SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, STOP_ADEC,
            "SAMPLE_COMM_AUDIO_StartAo FAIL. s32Ret: 0x%x\n", s32Ret);

        pfd = SAMPLE_AUDIO_OpenAencFile(AdChn, gs_enPayloadType);
        SAMPLE_CHECK_EXPR_GOTO(pfd == NULL, STOP_AO, "SAMPLE_AUDIO_OpenAencFile FAIL. s32Ret: 0x%x\n", s32Ret);

        s32Ret = SAMPLE_COMM_AUDIO_CreateTrdAencAdec(AeChn, AdChn, pfd);
        if (s32Ret != HI_SUCCESS) {
            fclose(pfd);
            pfd = HI_NULL;
            SAMPLE_PRT(s32Ret);
            SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, STOP_AO,
                "SAMPLE_COMM_AUDIO_CreateTrdAencAdec FAIL. s32Ret: 0x%x\n", s32Ret);
        }

        s32Ret = SAMPLE_COMM_AUDIO_AoBindAdec(AoDev, AoChn, AdChn);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
            SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, DESTORY_AENC_THREAD,
                "SAMPLE_COMM_AUDIO_AoBindAdec FAIL. s32Ret: 0x%x\n", s32Ret);
        }

        SAMPLE_PRT("bind adec:%d to ao(%d,%d) ok \n", AdChn, AoDev, AoChn);
    }

    if (bSendAdec == HI_TRUE) {
        s32Ret = SAMPLE_COMM_AUDIO_AoUnbindAdec(AoDev, AoChn, AdChn);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
        }

    DESTORY_AENC_THREAD:
        (HI_VOID)SAMPLE_COMM_AUDIO_DestroyTrdAencAdec(AdChn);

    STOP_AO:
        (HI_VOID)SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);

    STOP_ADEC:
        (HI_VOID)SAMPLE_COMM_AUDIO_StopAdec(AdChn);
    }
}

static hi_void sample_audio_ai_ao_init_param(AIO_ATTR_S *attr)
{
    attr->enSamplerate = AUDIO_SAMPLE_RATE_48000;
    attr->enBitwidth = AUDIO_BIT_WIDTH_16;
    attr->enWorkmode = AIO_MODE_I2S_MASTER;
    attr->enSoundmode = AUDIO_SOUND_MODE_STEREO;
    attr->u32EXFlag = 0;
    attr->u32FrmNum = FPS_30;
    attr->u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    attr->u32ChnCnt = 2; /* 2: chn num */
    attr->u32ClkSel = 1;
    attr->enI2sType = AIO_I2STYPE_INNERCODEC;

    gs_bAioReSample = HI_FALSE;
    /* config ao resample attr if needed */
    if (gs_bAioReSample == HI_TRUE) {
        /* ai 48k -> 32k */
        g_out_sample_rate = AUDIO_SAMPLE_RATE_32000;

        /* ao 32k -> 48k */
        g_in_sample_rate = AUDIO_SAMPLE_RATE_32000;
    } else {
        g_in_sample_rate = AUDIO_SAMPLE_RATE_BUTT;
        g_out_sample_rate = AUDIO_SAMPLE_RATE_BUTT;
    }

    gs_bUserGetMode = (gs_bAioReSample == HI_TRUE) ? HI_TRUE : HI_FALSE;
}

static HI_VOID SAMPLE_AUDIO_AiAoInner(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    HI_S32 s32Ret;

    /* bind AI to AO channel */
    if (gs_bUserGetMode == HI_TRUE) {
        s32Ret = SAMPLE_COMM_AUDIO_CreateTrdAiAo(AiDev, AiChn, AoDev, AoChn);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
            return;
        }
    } else {
        s32Ret = SAMPLE_COMM_AUDIO_AoBindAi(AiDev, AiChn, AoDev, AoChn);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
            return;
        }
    }
    SAMPLE_PRT("ai(%d,%d) bind to ao(%d,%d) ok\n", AiDev, AiChn, AoDev, AoChn);

    if (gs_bAoVolumeCtrl == HI_TRUE) {
        s32Ret = SAMPLE_COMM_AUDIO_CreateTrdAoVolCtrl(AoDev);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
            SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, AIAO_ERR0,
                "SAMPLE_COMM_AUDIO_CreateTrdAoVolCtrl FAIL. s32Ret: 0x%x\n", s32Ret);
        }
    }

    if (gs_bAoVolumeCtrl == HI_TRUE) {
        s32Ret = SAMPLE_COMM_AUDIO_DestroyTrdAoVolCtrl(AoDev);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
            return;
        }
    }

AIAO_ERR0:
    if (gs_bUserGetMode == HI_TRUE) {
        s32Ret = SAMPLE_COMM_AUDIO_DestroyTrdAi(AiDev, AiChn);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
        }
    } else {
        s32Ret = SAMPLE_COMM_AUDIO_AoUnbindAi(AiDev, AiChn, AoDev, AoChn);
        if (s32Ret != HI_SUCCESS) {
            SAMPLE_PRT(s32Ret);
        }
    }

    return;
}

/* function : Ai -> Ao(with fade in/out and volume adjust) */
static HI_S32 SAMPLE_AUDIO_AiAo(HI_VOID)
{
    HI_S32 s32Ret;
    HI_S32 s32AiChnCnt;
    HI_S32 s32AoChnCnt;
    const AI_CHN AiChn = 0;
    const AO_CHN AoChn = 0;
    AIO_ATTR_S stAioAttr;
    AUDIO_DEV AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;

    sample_audio_ai_ao_init_param(&stAioAttr);

    /* enable AI channel */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, g_out_sample_rate, gs_bAioReSample, NULL, 0);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
        SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, AIAO_ERR3,
            "SAMPLE_COMM_AUDIO_StartAi FAIL. s32Ret: 0x%x\n", s32Ret);
    }

    /* enable AO channel */
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, g_in_sample_rate, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
        SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, AIAO_ERR2,
            "SAMPLE_COMM_AUDIO_StartAo FAIL. s32Ret: 0x%x\n", s32Ret);
    }

    /* config internal audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, AIAO_ERR1,
        "SAMPLE_COMM_AUDIO_CfgAcodec FAIL. s32Ret: 0x%x\n", s32Ret);

    SAMPLE_AUDIO_AiAoInner(AiDev, AiChn, AoDev, AoChn);

AIAO_ERR1:
    s32Ret = SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
    }

AIAO_ERR2:
    s32Ret = SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, HI_FALSE);
    if (s32Ret != HI_SUCCESS) {
        SAMPLE_PRT(s32Ret);
    }

AIAO_ERR3:
    return s32Ret;
}

/* function : main */
HI_S32 AudioTest(HI_U32 num, HI_S32 quitFd)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_AUDIO_AddLibPath();

    HI_MPI_AENC_AacInit();
    HI_MPI_ADEC_AacInit();
    SAMPLE_AUDIO_AdecAo(num, quitFd);

    HI_MPI_AENC_AacDeInit();
    HI_MPI_ADEC_AacDeInit();

    return s32Ret;
}