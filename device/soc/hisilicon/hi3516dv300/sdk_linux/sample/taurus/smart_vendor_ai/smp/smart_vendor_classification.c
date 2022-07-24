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

#include "hi_mipi_tx.h"
#include "sdk.h"
#include "sample_comm.h"
#include "ai_infer_process.h"
#include "hand_classify.h"
#include "vgs_img.h"
#include "osd_img.h"
#include "posix_help.h"
#include "sample_media_ai.h"
#include "smart_vendor_classification.h"
#include "hisignalling.h"
#include "messaging.h"
#include "json_helper.h"


#define HAND_FRM_WIDTH    640
#define HAND_FRM_HEIGHT   384

RecogNumInfo g_numInfo;


/*----------------------------------------------------------------
@brief Send the product selection result to 3618 via UART
@param int fd: UART file descriptor
@param SlotSelection selectedSlot: The selected slot
----------------------------------------------------------------*/
void UARTSendResult(SlotSelection selectedSlot)
{
    int uartFd = 0;
    /* uart open init */
    uartFd = UartOpenInit();
    if (uartFd < 0) {
        printf("uart1 open failed\r\n");
    } else {
        printf("uart1 open successed\r\n");
    }
    char* payload = SlotSelectionToJson(selectedSlot);

    #ifdef  EXPANSION_BOARD

    messageUARTSendData(uartFd, payload);
    printf("Product selection result sent\r\n");

    #endif
}

/*----------------------------------------------------------------
@brief Send the UI control message to 3618 via UART
@param int fd: UART file descriptor
@param SlotSelection selectedSlot: The selected slot
----------------------------------------------------------------*/
void UARTSendUIControl(UIControl UIController)
{
    int uartFd = 0;
    /* uart open init */
    uartFd = UartOpenInit();
    if (uartFd < 0) {
        printf("uart1 open failed\r\n");
    } else {
        printf("uart1 open successed\r\n");
    }
    //char* payload = slotSelectionToJson(UIController);

    #ifdef  EXPANSION_BOARD

    //messageUARTSendData(payload);
    printf("UIController sent\r\n");

    #endif
}

/* hand gesture recognition info */
static int HandDetectFlag(const RecogNumInfo resBuf)
{
    HI_CHAR *gestureName = NULL;
    SAMPLE_PRT("resBuf.num: %u\n",resBuf.num);
    if(resBuf.num == 1000) //No hand was detected
        return 0;

    switch (resBuf.num) {
        case 0u:
            gestureName = "gesture one";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 1;
        case 1u:
            gestureName = "gesture two";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 2;
        case 2u:
            gestureName = "gesture three";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 3;
        case 3u:
            gestureName = "gesture four";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 4;
        case 4u:
            gestureName = "gesture five";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 5;
        case 5u:
            gestureName = "gesture six";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 6;
        case 6u:
            gestureName = "gesture seven";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 7;
        case 7u:
            gestureName = "gesture eight";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 8;
        case 8u:
            gestureName = "gesture nine";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 9;
        case 9u:
            gestureName = "gesture fist";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 10;
        case 10u:
            gestureName = "gesture rh_left";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 11;
        case 11u:
            gestureName = "gesture rh_right";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 12;
        case 12u:
            gestureName = "gesture lh_left";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 13;
        case 13u:
            gestureName = "gesture lh_right";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 14;
        case 14u:
            gestureName = "gesture empty";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 15;
        default:
            gestureName = "gesture others";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            return 16;
    }
    return 1;
}


int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

static HI_VOID VendorHandClassificationProcess(VIDEO_FRAME_INFO_S frm, VO_LAYER voLayer,
    VO_CHN voChn, AiPlugLib* g_workPlug, AicMediaInfo* g_aicMediaInfo)
{
    int ret;
    if (GetCfgBool("hand_classify_switch:support_hand_classify", true)) {
        if (g_workPlug->model == 0) {
            ret = Yolo2HandDetectResnetClassifyLoad(&(g_workPlug->model));
            if (ret < 0) {
                g_workPlug->model = 0;
                SAMPLE_CHECK_EXPR_GOTO(ret < 0, HAND_RELEASE,
                    "load hand classify model err, ret=%#x\n", ret);
            }
        }

        VIDEO_FRAME_INFO_S resizeFrm;
        ret = MppFrmResize(&frm, &resizeFrm, HAND_FRM_WIDTH, HAND_FRM_HEIGHT);

        g_numInfo.num = 1000;
        g_numInfo.score = 1000;//Use 1000 to denote the case of no hand

        //Get result from model, returns the return value of CnnCalU8c1Img()
        ret = Yolo2HandDetectResnetClassifyCal(g_workPlug->model, &resizeFrm,
            &frm, &g_numInfo);

        //system("cat /proc/umap/vi");


        SAMPLE_CHECK_EXPR_GOTO(ret < 0, HAND_RELEASE,
            "hand classify plug cal FAIL, ret=%#x\n", ret);

        ret = HI_MPI_VO_SendFrame(voLayer, voChn, &frm, 0);
        SAMPLE_CHECK_EXPR_GOTO(ret != HI_SUCCESS, HAND_RELEASE,
            "HI_MPI_VO_SendFrame fail, Error(%#x)\n", ret);

        MppFrmDestroy(&resizeFrm);
    }

    HAND_RELEASE:
        ret = HI_MPI_VPSS_ReleaseChnFrame(g_aicMediaInfo->vpssGrp,
            g_aicMediaInfo->vpssChn0, &frm);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed, Grp(%d) chn(%d)!\n",
                ret, g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0);
        }

}

HI_VOID* VendorGetVpssChnFrameAndClassify(void* arguments)
{
    struct arg_struct {
        AiPlugLib* g_workPlug;
        AicMediaInfo* g_aicMediaInfo;
    };
    struct arg_struct *args = arguments;
    AiPlugLib* g_workPlug = args -> g_workPlug;
    AicMediaInfo* g_aicMediaInfo = args -> g_aicMediaInfo;
    int ret;
    VIDEO_FRAME_INFO_S frm;
    HI_S32 s32MilliSec = 2000;
    VO_LAYER voLayer = 0;
    VO_CHN voChn = 0;

    SAMPLE_PRT("vpssGrp:%d, vpssChn0:%d\n", g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0);
    int timeoutCount = 0;
    int timeoutStop = 0;
    while (HI_FALSE == g_bAiProcessStopSignal) {
        timeoutCount += 1;
        printf("\n timeoutCount = %d \n", timeoutCount);
        system("sleep 0.1");
        if(timeoutCount > 200){
            g_bAiProcessStopSignal = HI_TRUE;
            SAMPLE_PRT("Timeout!\n");
            break;
        }
        int retResult = 0;
        int lastResult = 5000;//initial value
        for(int i = 0; i < 5; i += 1){
            timeoutCount += 1;
            printf("\n timeoutCount = %d \n", timeoutCount);
            system("sleep 0.1");
            if(timeoutCount > 200){
                g_bAiProcessStopSignal = HI_TRUE;
                timeoutStop = 1;
                SAMPLE_PRT("Timeout!\n");
                break;
            }
            ret = HI_MPI_VPSS_GetChnFrame(g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0,
                &frm, s32MilliSec);
            if (ret != 0) {
                SAMPLE_PRT("HI_MPI_VPSS_GetChnFrame FAIL, err=%#x, grp=%d, chn=%d\n",
                    ret, g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0);
                ret = HI_MPI_VPSS_ReleaseChnFrame(g_aicMediaInfo->vpssGrp,
                    g_aicMediaInfo->vpssChn0, &frm);
                if (ret != HI_SUCCESS) {
                    SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed, grp(%d) chn(%d)!\n",
                        ret, g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0);
                }
                continue;
            }
            SAMPLE_PRT("get vpss frame success, weight:%d, height:%d\n", frm.stVFrame.u32Width,
                frm.stVFrame.u32Height);

            VendorHandClassificationProcess(frm, voLayer, voChn, g_workPlug, g_aicMediaInfo);

            if(lastResult == 5000)
                lastResult = HandDetectFlag(g_numInfo);

            retResult = HandDetectFlag(g_numInfo);
            if(!retResult || retResult == 15){//No hand
                i = 0;
                continue;
            }
            if(retResult != lastResult){
                i = 0;
                lastResult = retResult;
            }
        }
        if(timeoutStop)
            break;
        SlotSelection selectedSlot;
        selectedSlot.slot_num = retResult;
        UARTSendResult(selectedSlot); //Send result to 3861 via UART
        g_bAiProcessStopSignal = HI_TRUE;
    }

    return HI_NULL;
}
