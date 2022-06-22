#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>

#include "hi_mipi_tx.h"
#include "sdk.h"
#include "sample_comm.h"
#include "ai_infer_process.h"
#include "hand_classify.h"
#include "vgs_img.h"
#include "osd_img.h"
#include "posix_help.h"
#include "sample_media_ai.h"
#include "smart_vendor_ai_classification.h"
#include "hisignalling.h"



/* hand gesture recognition info */
static void HandDetectFlagSample(const RecogNumInfo resBuf)
{
    int uartFd = 0;
    /* uart open init */
    uartFd = UartOpenInit();
    if (uartFd < 0) {
        printf("uart1 open failed\r\n");
    } else {
        printf("uart1 open successed\r\n");
    }
    HI_CHAR *gestureName = NULL;
    SAMPLE_PRT("resBuf.num: %u\n",resBuf.num);
    switch (resBuf.num) {
        case 0u:
            gestureName = "gesture empty";
            UartSendRead(uartFd, FistGesture); // 拳头手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 1u:
            gestureName = "gesture fist";
            UartSendRead(uartFd, ForefingerGesture); // 食指手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 2u:
            gestureName = "gesture OK";
            UartSendRead(uartFd, OkGesture); // OK手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 3u:
            gestureName = "gesture palm";
            UartSendRead(uartFd, PalmGesture); // 手掌手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 4u:
            gestureName = "gesture yes";
            UartSendRead(uartFd, YesGesture); // yes手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 5u:
            gestureName = "gesture pinchOpen";
            UartSendRead(uartFd, ForefingerAndThumbGesture); // 食指 + 大拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 6u:
            gestureName = "gesture phoneCall";
            UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        default:
            gestureName = "gesture others";
            UartSendRead(uartFd, InvalidGesture); // 无效值
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
    }
    //SAMPLE_PRT("hand gesture success\n");
}


/* hand gesture recognition info */
static void HandDetectFlag(const RecogNumInfo resBuf)
{
    HI_CHAR *gestureName = NULL;
    SAMPLE_PRT("resBuf.num: %u\n",resBuf.num);
    switch (resBuf.num) {
        case 0u:
            gestureName = "gesture eight";
            //UartSendRead(uartFd, FistGesture); // 拳头手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 1u:
            gestureName = "gesture empty";
            //UartSendRead(uartFd, ForefingerGesture); // 食指手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 2u:
            gestureName = "gesture fist";
            //UartSendRead(uartFd, OkGesture); // OK手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 3u:
            gestureName = "gesture five";
            //UartSendRead(uartFd, PalmGesture); // 手掌手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 4u:
            gestureName = "gesture four";
            //UartSendRead(uartFd, YesGesture); // yes手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 5u:
            gestureName = "gesture lh_left";
            //UartSendRead(uartFd, ForefingerAndThumbGesture); // 食指 + 大拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 6u:
            gestureName = "gesture lh_right";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 7u:
            gestureName = "gesture nine";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 8u:
            gestureName = "gesture one";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 9u:
            gestureName = "gesture rh_left";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 10u:
            gestureName = "gesture rh_right";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 11u:
            gestureName = "gesture seven";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 12u:
            gestureName = "gesture six";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 13u:
            gestureName = "gesture three";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 14u:
            gestureName = "gesture two";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        default:
            gestureName = "gesture others";
            //UartSendRead(uartFd, InvalidGesture); // 无效值
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
    }
}

/*
 * Display the data collected by sensor to LCD screen
 * VI->VPSS->VO->MIPI
 */
HI_S32 SMART_VENDOR_HAND_CLASSIFY(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             fd = 0;

    /* config vi */
    ViPramCfg();

    /* get picture size */
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(g_aicMediaInfo.viCfg.astViInfo[0].stSnsInfo.enSnsType,
        &g_aicMediaInfo.enPicSize);
    SAMPLE_CHECK_EXPR_RET(s32Ret != HI_SUCCESS, s32Ret, "get pic size by sensor fail, s32Ret=%#x\n", s32Ret);

    /* get picture size(w*h), according enPicSize */
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(g_aicMediaInfo.enPicSize, &g_aicMediaInfo.stSize);
    SAMPLE_PRT("AIC: snsMaxSize=%ux%u\n", g_aicMediaInfo.stSize.u32Width, g_aicMediaInfo.stSize.u32Height);
    SAMPLE_CHECK_EXPR_RET(s32Ret != HI_SUCCESS, s32Ret, "get picture size failed, s32Ret=%#x\n", s32Ret);

    /* config vb */
    StVbParamCfg(&g_aicMediaInfo.vbCfg);

    /* vb init & MPI system init */
    s32Ret = SAMPLE_COMM_SYS_Init(&g_aicMediaInfo.vbCfg);
    SAMPLE_CHECK_EXPR_RET(s32Ret != HI_SUCCESS, s32Ret, "system init failed, s32Ret=%#x\n", s32Ret);

    /* set VO config to mipi, get mipi device */
    s32Ret = SAMPLE_VO_CONFIG_MIPI(&fd);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, EXIT, "CONFIG MIPI FAIL.s32Ret:0x%x\n", s32Ret);

    /* config vpss */
    VpssParamCfg();
    s32Ret = ViVpssCreate(&g_aicMediaInfo.viSess, &g_aicMediaInfo.viCfg, &g_aicMediaInfo.vpssCfg);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, EXIT1, "ViVpss Sess create FAIL, ret=%#x\n", s32Ret);
    g_aicMediaInfo.vpssGrp = AIC_VPSS_GRP;
    g_aicMediaInfo.vpssChn0 = AIC_VPSS_ZOUT_CHN;

    /* config vo */
    StVoParamCfg(&g_aicMediaInfo.voCfg);

    /* start vo */
    s32Ret = SampleCommVoStartMipi(&g_aicMediaInfo.voCfg);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, EXIT1, "start vo FAIL. s32Ret: 0x%x\n", s32Ret);

    /* vpss bind vo */
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(g_aicMediaInfo.vpssGrp, g_aicMediaInfo.vpssChn0, g_aicMediaInfo.voCfg.VoDev, 0);
    SAMPLE_CHECK_EXPR_GOTO(s32Ret != HI_SUCCESS, EXIT2, "vo bind vpss FAIL. s32Ret: 0x%x\n", s32Ret);
    SAMPLE_PRT("vpssGrp:%d, vpssChn:%d\n", g_aicMediaInfo.vpssGrp, g_aicMediaInfo.vpssChn0);

    /* create work thread to run ai */
    s32Ret = VendorHandClassificationProcess();
    SAMPLE_CHECK_EXPR_RET(s32Ret != HI_SUCCESS, s32Ret, "failed to create ai proccess thread: %s\n", strerror(s32Ret));

    Pause();
    g_bAiProcessStopSignal = HI_TRUE;
    // Waiting for the end of a thread, the operation of synchronization between threads
    pthread_join(g_aiProcessThread, NULL);
    g_aiProcessThread = 0;
    PauseDoUnloadHandClassifyModel();

    SAMPLE_COMM_VPSS_UnBind_VO(g_aicMediaInfo.vpssGrp, g_aicMediaInfo.vpssChn0, g_aicMediaInfo.voCfg.VoDev, 0);
    SAMPLE_VO_DISABLE_MIPITx(fd);
    SampleCloseMipiTxFd(fd);
    system("echo 0 > /sys/class/gpio/gpio55/value");

EXIT2:
    SAMPLE_COMM_VO_StopVO(&g_aicMediaInfo.voCfg);
EXIT1:
    VpssStop(&g_aicMediaInfo.vpssCfg);
    SAMPLE_COMM_VI_UnBind_VPSS(g_aicMediaInfo.viCfg.astViInfo[0].stPipeInfo.aPipe[0],
        g_aicMediaInfo.viCfg.astViInfo[0].stChnInfo.ViChn, g_aicMediaInfo.vpssGrp);
    ViStop(&g_aicMediaInfo.viCfg);
    free(g_aicMediaInfo.viSess);
EXIT:
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}

static HI_S32 VendorHandClassificationCreateThread(HI_VOID)
{
    HI_S32 s32Ret;
    if (snprintf_s(acThreadName, BUFFER_SIZE, BUFFER_SIZE - 1, "AIProcess") < 0) {
        HI_ASSERT(0);
    }
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0, 0, 0);
    s32Ret = pthread_create(&g_aiProcessThread, NULL, GetVpssChnFrameHandClassify, NULL);

    return s32Ret;
}

static HI_VOID VendorHandClassificationProcess(VIDEO_FRAME_INFO_S frm, VO_LAYER voLayer, VO_CHN voChn)
{
    int ret;
    if (GetCfgBool("hand_classify_switch:support_hand_classify", true)) {
        if (g_workPlug.model == 0) {
            ret = Yolo2HandDetectResnetClassifyLoad(&g_workPlug.model);
            if (ret < 0) {
                g_workPlug.model = 0;
                SAMPLE_CHECK_EXPR_GOTO(ret < 0, HAND_RELEASE,
                    "load hand classify model err, ret=%#x\n", ret);
            }
        }

        VIDEO_FRAME_INFO_S resizeFrm;
        ret = MppFrmResize(&frm, &resizeFrm, HAND_FRM_WIDTH, HAND_FRM_HEIGHT);
        static RecogNumInfo numInfo[4] = {0};
        RecogNumInfo hundredResults[100];
        unsigned int hundredResultsNum[100];
        for(int i = 0; i < 100; i++){
            ret = Yolo2HandDetectResnetClassifyCal(g_workPlug.model, &resizeFrm, &frm, numInfo); //Get result from model, returns the return value of CnnCalU8c1Img()
            hundredResults[i] = numInfo[0];
            hundredResultsNum[i] = numInfo[0].num;
        }
        int recogResultOccurences[15];
        int maxResultOccurence = 0;
        int maxOccuredResultNum = 0;
        qsort(hundredResultsNum, 100, sizeof(int), cmpfunc);
        int resultCounter = 0;
        for(int i = 1; i < 100; i++){
            if(hundredResultsNum[i] != hundredResultsNum[i - 1]){
                if(resultCounter <= maxResultOccurence){
                    maxResultOccurence = resultCounter;
                    maxOccuredResultNum = i;
                }
                resultCounter = 0;
            }else{
                resultCounter++;
            }
        }
        HandDetectFlagSample(hundredResults[maxOccuredResultNum]);

        SAMPLE_CHECK_EXPR_GOTO(ret < 0, HAND_RELEASE,
            "hand classify plug cal FAIL, ret=%#x\n", ret);

        ret = HI_MPI_VO_SendFrame(voLayer, voChn, &frm, 0);
        SAMPLE_CHECK_EXPR_GOTO(ret != HI_SUCCESS, HAND_RELEASE,
            "HI_MPI_VO_SendFrame fail, Error(%#x)\n", ret);

        MppFrmDestroy(&resizeFrm);
    }

    HAND_RELEASE:
        ret = HI_MPI_VPSS_ReleaseChnFrame(g_aicMediaInfo.vpssGrp, g_aicMediaInfo.vpssChn0, &frm);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                ret, g_aicMediaInfo.vpssGrp, g_aicMediaInfo.vpssChn0);
        }
}
