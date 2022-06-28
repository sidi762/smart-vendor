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
static void HandDetectFlagSample(const RecogNumInfo resBuf)
{
    HI_CHAR *gestureName = NULL;
    //SAMPLE_PRT("resBuf.num: %u\n",resBuf.num);

    SlotSelection selectedSlot;

    switch (resBuf.num) {
        case 0u:
            gestureName = "gesture fist";
            selectedSlot.slot_num = 1;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 1u:
            gestureName = "gesture indexUp";
            selectedSlot.slot_num = 2;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 2u:
            gestureName = "gesture OK";
            selectedSlot.slot_num = 3;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 3u:
            gestureName = "gesture palm";
            selectedSlot.slot_num = 4;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 4u:
            gestureName = "gesture yes";
            //UartSendRead(uartFd, YesGesture); // yes手势
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 5u:
            gestureName = "gesture pinchOpen";
            //UartSendRead(uartFd, ForefingerAndThumbGesture); // 食指 + 大拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 6u:
            gestureName = "gesture phoneCall";
            //UartSendRead(uartFd, LittleFingerAndThumbGesture); // 大拇指 + 小拇指
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        default:
            gestureName = "gesture others";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
    }
    //g_bAiProcessStopSignal = HI_TRUE;
    //SAMPLE_PRT("hand gesture success\n");
}

/* hand gesture recognition info */
static int HandDetectFlag(const RecogNumInfo resBuf)
{
    HI_CHAR *gestureName = NULL;
    SAMPLE_PRT("resBuf.num: %u\n",resBuf.num);
    if(resBuf.num == 1000) //No hand was detected
        return 0;
    SlotSelection selectedSlot;
    switch (resBuf.num) {
        case 0u:
            gestureName = "gesture one";
            selectedSlot.slot_num = 1;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 1u:
            gestureName = "gesture two";
            selectedSlot.slot_num = 2;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 2u:
            gestureName = "gesture three";
            selectedSlot.slot_num = 3;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 3u:
            gestureName = "gesture four";
            selectedSlot.slot_num = 4;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 4u:
            gestureName = "gesture five";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 5u:
            gestureName = "gesture six";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 6u:
            gestureName = "gesture seven";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 7u:
            gestureName = "gesture eight";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 8u:
            gestureName = "gesture nine";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 9u:
            gestureName = "gesture fist";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 10u:
            gestureName = "gesture rh_left";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 11u:
            gestureName = "gesture rh_right";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 12u:
            gestureName = "gesture lh_left";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 13u:
            gestureName = "gesture lh_right";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 14u:
            gestureName = "gesture empty";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        default:
            gestureName = "gesture others";
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
    }
    return 1;
}


int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}
HI_VOID VendorHandClassificationProcess(VIDEO_FRAME_INFO_S frm, VO_LAYER voLayer, VO_CHN voChn, AiPlugLib* g_workPlug, AicMediaInfo* g_aicMediaInfo)
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
        RecogNumInfo numInfo;
        numInfo.num = 1000;
        numInfo.score = 1000;//Use 1000 to denote the case of no hand

        /*
        RecogNumInfo hundredResults[100];
        unsigned int hundredResultsNum[100];

        for(int i = 0; i < 100; i++){
            ret = Yolo2HandDetectResnetClassifyCal(g_workPlug->model, &resizeFrm, &frm, numInfo); //Get result from model, returns the return value of CnnCalU8c1Img()
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
        */



        ret = Yolo2HandDetectResnetClassifyCal(g_workPlug->model, &resizeFrm, &frm, &numInfo); //Get result from model, returns the return value of CnnCalU8c1Img()
        //HandDetectFlagSample(numInfo);
        if(HandDetectFlag(numInfo))
            g_bAiProcessStopSignal = HI_TRUE;
        //system("cat /proc/umap/vi");


        SAMPLE_CHECK_EXPR_GOTO(ret < 0, HAND_RELEASE,
            "hand classify plug cal FAIL, ret=%#x\n", ret);

        ret = HI_MPI_VO_SendFrame(voLayer, voChn, &frm, 0);
        SAMPLE_CHECK_EXPR_GOTO(ret != HI_SUCCESS, HAND_RELEASE,
            "HI_MPI_VO_SendFrame fail, Error(%#x)\n", ret);

        MppFrmDestroy(&resizeFrm);
    }

    HAND_RELEASE:
        ret = HI_MPI_VPSS_ReleaseChnFrame(g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0, &frm);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                ret, g_aicMediaInfo->vpssGrp, g_aicMediaInfo->vpssChn0);
        }

}
