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

#ifndef SAMPLE_MEDIA_AI_H
#define SAMPLE_MEDIA_AI_H

#include "sample_comm.h"
#include "list.h"
#include "osd_img.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef SAMPLE_PRT
#define SAMPLE_PRT(fmt...) \
    do { \
        printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
        printf(fmt); \
    } while (0)
#endif

#define APIPE0    0
#define APIPE1    1
#define APIPE2    2
#define APIPE3    3

#define AIC_VPSS_GRP            0 // default use VPSS group
#define AIC_VPSS_ZIN_CHN        0 // default use VPSS amplification channel
#define AIC_VPSS_ZOUT_CHN       1 // default use VPSS narrowing channel

#define AICSTART_VI_OUTWIDTH    1920
#define AICSTART_VI_OUTHEIGHT   1080

/* bit constant */
#define HI_BIT0             0x01U
#define HI_BIT1             0x02U
#define HI_BIT2             0x04U
#define HI_BIT3             0x08U
#define HI_BIT4             0x10U
#define HI_BIT5             0x20U
#define HI_BIT6             0x40U
#define HI_BIT7             0x80U
#define HI_BIT8             0x0100U
#define HI_BIT9             0x0200U
#define HI_BIT10            0x0400U
#define HI_BIT11            0x0800U
#define HI_BIT12            0x1000U
#define HI_BIT13            0x2000U
#define HI_BIT14            0x4000U
#define HI_BIT15            0x8000U

/* Flags to mark whether the component is enabled */
#define MPP_VI      HI_BIT0
#define MPP_VDEC    HI_BIT1
#define MPP_VPSS    HI_BIT2
#define MPP_VENC    HI_BIT3
#define MPP_VO      HI_BIT5

typedef SAMPLE_VI_CONFIG_S ViCfg;
typedef SAMPLE_VO_CONFIG_S VoCfg;
typedef VB_CONFIG_S VbCfg;

typedef struct SampleVoModeMux {
    HI_U32 u32WndNum;
    HI_U32 u32Square;
    HI_U32 u32Row;
    HI_U32 u32Col;
} SampleVoModeMux;

/*
 * VPSS channel config.
 * Used to set the attributes of a channel.
 */
typedef struct VpssChnCfg {
    int id; // VpssChn ID
    VPSS_CHN_ATTR_S attr; // VpssChn attributes
} VpssChnCfg;

/*
 * VPSS config.
 * Each VpssCfg corresponds to 1 VPSS group and 1 or more VPSS channels.
 */
typedef struct VpssCfg {
    VPSS_GRP grpId; // VpssGrp ID
    VPSS_GRP_ATTR_S grpAttr; // VpssGrp attributes

    int chnNum; // Configure the number of chnnels used
    VpssChnCfg chnCfgs[VPSS_MAX_PHY_CHN_NUM]; // pssChnCfg array, the first chnNum-1 elements are valid
} VpssCfg;

/*
 * MppSess superset.
 * The create() function corresponding to MppSess will copy the required cfg value to the object and start the session.
 * MppSess does not provide a constructor. The user can only create an object through the create() function
 * and destroy the object with MppSess_destroy().
 * MppSess defines the resource ID, vpssGrp, vpssChn0, vpssChn1, vdecChn, vencChn currently used by MppSess.
 * These values are set by create(), and the corresponding IDs of components not identified by used will be set to -1.
 * These resource IDs are obtained from the parameter xxxCfg passed in by create()
 * and copied to the object to simplify APP use.
 * There is currently no defined channel ID of the VI, it will always be used after being bound to the VPSS,
 * and the VI data can be obtained through the VPSS chn.
 */
typedef struct MppSess {
    uint32_t used; // The set of enabled components, set by the create() function

    ViCfg viCfg; // Optional ViCfg
    VpssCfg vpssCfg; // Optional VpssCfg

    VPSS_GRP vpssGrp; // VPSS group ID, -1 means that the corresponding MPP component is not started.
    VPSS_CHN vpssChn0; // VPSS channel[0] ID, -1 means that the corresponding MPP component is not started.
    VPSS_CHN vpssChn1; // VPSS channel[1] ID, -1 means that the corresponding MPP component is not started.
} MppSess;

typedef struct AicMediaInfo {
    VPSS_GRP vpssGrp;
    VPSS_CHN vpssChn0;
    VPSS_CHN vpssChn1;
    VDEC_CHN vdecChn;
    VENC_CHN vencChn;

    ViCfg viCfg;
    VpssCfg vpssCfg;
    VoCfg voCfg;
    VbCfg vbCfg;

    // MppSess
    MppSess *viSess; // VI(sensor)+VPSS
    int vpssFd;
    SIZE_S stSize;
    PIC_SIZE_E enPicSize;
    HI_U32  u32BlkSize;
	
    OsdSet *osds; // OSD set for Plug to output OSD in resFrm
} AicMediaInfo;

HI_S32 SAMPLE_MEDIA_CNN_TRASH_CLASSIFY(HI_VOID);
HI_S32 SampleCommVoStartDevMipi(VO_DEV VoDev, VO_PUB_ATTR_S* pstPubAttr);
HI_S32 SAMPLE_MEDIA_HAND_CLASSIFY(HI_VOID);

/* init ViCfg */
void ViCfgInit(ViCfg* self);

/* Initialize VpssCfg */
void VpssCfgInit(VpssCfg* self);

/* Set up VPSS Group */
void VpssCfgSetGrp(VpssCfg* self,
    int grpId, const VPSS_GRP_ATTR_S* grpAttr, int maxWidth, int maxHeight);

/* Add a VPSS channel */
VPSS_CHN_ATTR_S* VpssCfgAddChn(VpssCfg* self,
    int chnId, const VPSS_CHN_ATTR_S* chnAttr, int width, int height);

/* Set VI DEV information */
void ViCfgSetDev(ViCfg* self, int devId, WDR_MODE_E wdrMode);

/* Set the PIPE information of the VI */
void ViCfgSetPipe(ViCfg* self, int pipe0Id, int pipe1Id, int pipe2Id, int pipe3Id);

/* Set up the VI channel */
void ViCfgSetChn(ViCfg* self, int chnId, PIXEL_FORMAT_E pixFormat,
    VIDEO_FORMAT_E videoFormat, DYNAMIC_RANGE_E dynamicRange);

/* Create and start {VI->VPSS}MppSess */
int ViVpssCreate(MppSess** sess, const ViCfg* viCfg, const VpssCfg* vpssCfg);

/* Terminate VPSS started with VpssCfg */
int VpssStop(const VpssCfg* cfg);

/* Terminate VIs started with ViCfg */
int ViStop(const ViCfg* viCfg);

/* start vo to mipi lcd */
HI_S32 SampleCommVoStartMipi(SAMPLE_VO_CONFIG_S *pstVoConfig);

/* onfig mipi */
HI_S32 SAMPLE_VO_CONFIG_MIPI(HI_S32* mipiFD);

/* disable mipi tx */
HI_S32 SAMPLE_VO_DISABLE_MIPITx(HI_S32 fd);

/* close mipi_tx device */
HI_VOID SampleCloseMipiTxFd(HI_S32 fd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MEDIA_AI_H__ */
