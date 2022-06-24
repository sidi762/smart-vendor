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
    char* payload = slotSelectionToJson(selectedSlot);

    #ifdef  EXPANSION_BOARD
    messageUARTSendData(uartFd, payload);
    //HisignallingMsgSend(uartFd, writeBuffer, len);
    /*
    unsigned char writeBuffer1[4] = {0, 2, 0, 1};
    unsigned char writeBuffer2[4] = {0, 2, 0, 2};
    unsigned char writeBuffer3[4] = {0, 2, 0, 3};
    unsigned char writeBuffer4[4] = {0, 2, 0, 4};
    switch (selectedSlot.slot_num) {
        case 1:
            HisignallingMsgSend(uartFd, writeBuffer1, sizeof(writeBuffer1)/sizeof(writeBuffer1[0]));
            break;
        case 2:
            HisignallingMsgSend(uartFd, writeBuffer2, sizeof(writeBuffer2)/sizeof(writeBuffer2[0]));
            break;
        case 3:
            HisignallingMsgSend(uartFd, writeBuffer3, sizeof(writeBuffer3)/sizeof(writeBuffer3[0]));
            break;
        case 4:
            HisignallingMsgSend(uartFd, writeBuffer4, sizeof(writeBuffer4)/sizeof(writeBuffer4[0]));
            break;
    }
    */
    printf("Product selection result sent\r\n");

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
    //SAMPLE_PRT("hand gesture success\n");
}

/* hand gesture recognition info */
static void HandDetectFlag(const RecogNumInfo resBuf)
{
    HI_CHAR *gestureName = NULL;
    //SAMPLE_PRT("resBuf.num: %u\n",resBuf.num);
    SlotSelection selectedSlot;
    switch (resBuf.num) {
        case 0u:
            gestureName = "gesture eight";
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
            selectedSlot.slot_num = 4;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
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
            selectedSlot.slot_num = 1;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
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
            selectedSlot.slot_num = 3;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        case 14u:
            gestureName = "gesture two";
            selectedSlot.slot_num = 2;
            UARTSendResult(selectedSlot); //Send result to 3861 via UART
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
        default:
            gestureName = "gesture others";
            //UartSendRead(uartFd, InvalidGesture); // 无效值
            SAMPLE_PRT("----gesture name----:%s\n", gestureName);
            break;
    }
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
        HandDetectFlagSample(numInfo);



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
