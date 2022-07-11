/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hi_types_base.h>
#include <hi_early_debug.h>
#include <hi_stdlib.h>
#include <hi_uart.h>
#include <hi_task.h>
#include <app_demo_uart.h>
#include <iot_uart.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "hi_io.h"
#include "iot_gpio.h"
#include "hisignalling_protocol.h"

#define LED_TEST_GPIO 9
#define LED_INTERVAL_TIME_US 300000

hi_u8   g_sendUartBuff[UART_BUFF_SIZE];

UartDefConfig recConfig  = {0};

/*
 * crc32 Verification implementation
*/
static const unsigned int crc32table[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
    0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
    0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
    0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
    0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
    0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
    0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
    0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
    0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
    0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
    0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
    0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
    0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
    0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
    0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
    0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
    0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
    0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
    0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
    0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
    0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
    0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
    0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

static unsigned int crc32(const unsigned char *buf, unsigned int len)
{
    unsigned int i, crc = 0xFFFFFFFF;

    for (i = 0; i < len; i++) {
        crc = crc32table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8); /* 8: right move 8 bit */
    }
    return crc ^ 0xFFFFFFFF;
}
/* Log level look up table */
static const char *hisignallingLevelNames[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};
/* get hisignaling log level */
const char *HisignallingLevelNum (HisignallingLogType hisignallingLevel)
{
    if (hisignallingLevel >= HISIGNALLING_LEVEL_MAX) {
        return "NULL";
    } else {
        return hisignallingLevelNames[hisignallingLevel];
    }
}
#define RIGHT_MOVE_8_BIT (8)
#define RIGHT_MOVE_16_BIT (16)
#define RIGHT_MOVE_24_BIT (24)
/* hisignal Hi3861 message package */
static hi_u32 HisignallingDataPackage(HisignallingProtocalType *buf, hi_u32 len, hi_u8 *hisignallingDataBuf)
{
    hi_u32 crcCheckSend = 0;
    hi_u32 packageLen = 0;

    memcpy_s(hisignallingDataBuf, HISGNALLING_MSG_FRAME_HEADER_LEN,
        buf->frameHeader, HISGNALLING_MSG_FRAME_HEADER_LEN);
    memcpy_s(&hisignallingDataBuf[HISGNALLING_MSG_FRAME_HEADER_LEN],
        len, buf->hisignallingMsgBuf, len);
    memcpy_s(&hisignallingDataBuf[HISGNALLING_MSG_FRAME_HEADER_LEN + len],
        HISIGNALLING_MSG_HEADER_LEN, &(buf->endOfFrame), HISIGNALLING_MSG_HEADER_LEN);
    crcCheckSend = crc32(hisignallingDataBuf, (len + HISIGNALLING_MSG_HEADER_TAIL_LEN));
    hisignallingDataBuf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN] =
        (hi_u8) ((crcCheckSend & 0xff000000)>>RIGHT_MOVE_24_BIT);
    hisignallingDataBuf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN + 1] = /* 1: addr offset */
        (hi_u8) ((crcCheckSend & 0x00ff0000)>>RIGHT_MOVE_16_BIT);
    hisignallingDataBuf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN + 2] = /* 2: addr offset */
        (hi_u8) ((crcCheckSend & 0x0000ff00)>>RIGHT_MOVE_8_BIT);
    hisignallingDataBuf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN + 3] = /* 3: addr offset */
        (hi_u8)crcCheckSend;
    packageLen = len + HISIGNALLING_MSG_HEADER_TAIL_LEN + HISGNALLING_MSG_CRC32_LEN;
    return packageLen;
}

/* hisignal Hi3861 message recevice */
HisignallingErrorType HisignallingMsgReceive(hi_u8 *buf, hi_u32 len)
{
    hi_u32 crcCheckReceived = 0;
    unsigned char testBuff[7] = {0xaa, 0x55, 0x0, 0x2, 0x0, 0x8, 0xff};
    if (buf == HI_NULL && len > 0) {
        HISIGNALLING_LOG_FATAL("received buf is null");
        return HISGNALLING_RET_VAL_MAX;
    }
    printf("length:%u\n", len);

    /* 校验crc */
    if (len > HISGNALLING_MSG_CRC32_LEN) {
        crcCheckReceived = crc32(buf, len - HISGNALLING_MSG_CRC32_LEN);
        if (((hi_u8)((crcCheckReceived & 0xff000000)>>RIGHT_MOVE_24_BIT) != buf[len - 4]) && /* 4: addr offset */
            ((hi_u8)((crcCheckReceived & 0x00ff0000)>>RIGHT_MOVE_16_BIT) != buf[len - 3]) && /* 3: addr offset */
            ((hi_u8)((crcCheckReceived & 0x0000ff00)>>RIGHT_MOVE_8_BIT)  != buf[len - 2]) && /* 2: addr offset */
            ((hi_u8)crcCheckReceived != buf[len - 1])) {
            HISIGNALLING_LOG_ERROR("<Hi3861>: crc32 Verification failed!");
            HISIGNALLING_LOG_ERROR("<Hi3861>:crc_4=0x%x, crc_3=0x%x, crc_2=0x%x, crc_1=0x%x",
                buf[len - 4], buf[len - 3], buf[len - 2], buf[len - 1]); /* 4,3,2,1 addr offset */
            return HISIGNALLING_RET_VAL_ERROR;
        }
    }
    IoTGpioSetOutputVal(LED_TEST_GPIO, 0);
    usleep(LED_INTERVAL_TIME_US);
    IoTGpioSetOutputVal(LED_TEST_GPIO, 1);
    usleep(LED_INTERVAL_TIME_US);
    /* 输出回显收到的数据 */
    /*if ((buf[0] == HISIGNALLING_MSG_FRAME_HEADER_1) && (buf[1] == HISIGNALLING_MSG_FRAME_HEADER_2)) {
        for (int i = 0; i < len; i++) {
            HISIGNALLING_LOG_INFO("0x%x", buf[i]);
        }
    }*/
    return HISIGNALLING_RET_VAL_CORRECT;
}
/* hisignal Hi3861 message send */
hi_u32 HisignallingMsgSend(char *buf, hi_u32 dataLen)
{
    HisignallingProtocalType hisignallingMsg = {0};
    hi_u8 hisignallingSendBuf[HISIGNALLING_MSG_BUFF_LEN] = {0};
    hi_u32 hisignallingPackageLen = 0;
    hi_u32 writeDataLen = 0;

    hisignallingMsg.frameHeader[0]= HISIGNALLING_MSG_FRAME_HEADER_1;
    hisignallingMsg.frameHeader[1]= HISIGNALLING_MSG_FRAME_HEADER_2;
    (void)memcpy_s(hisignallingMsg.hisignallingMsgBuf, dataLen, buf, dataLen);
    hisignallingMsg.endOfFrame = HISIGNALLING_MSG_FRAME_TAIL;

    hisignallingPackageLen = HisignallingDataPackage(&hisignallingMsg, dataLen, hisignallingSendBuf);
    if (!hisignallingPackageLen) {
        HISIGNALLING_LOG_ERROR("hisignaling_data_package failed\r\n");
        return HI_ERR_FAILURE;
    }
    if (*hisignallingSendBuf == NULL) {
        HISIGNALLING_LOG_ERROR("hisignal send buf is null!\r\n");
        return HI_ERR_FAILURE;
    }
    writeDataLen = IoTUartWrite(HI_UART_IDX_1, hisignallingSendBuf, hisignallingPackageLen);
    if (!writeDataLen) {
        HISIGNALLING_LOG_ERROR("hi_uart_write failed\r\n");
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}

int SetUartReceiveFlag(void)
{
    return recConfig.g_uartReceiveFlag;
}

hi_void *HisignallingMsgHandle(char *param)
{
    unsigned char *recBuff = NULL;
    int len;

    while (1) {
        /*g_sendUartBuff清零*/
        (void)memset_s(g_sendUartBuff, sizeof(g_sendUartBuff) / sizeof(g_sendUartBuff[0]),
            0x0, sizeof(g_sendUartBuff)/sizeof(g_sendUartBuff[0]));
            /*GetUartConfig得到data的flag和length*/
        if (GetUartConfig(UART_RECEIVE_FLAG) == HI_TRUE) {
            /* 接收数据 */
            HisignallingMsgReceive(GetUartReceiveMsg(), GetUartConfig(UART_RECVIVE_LEN));
            /* 回显数据组包 */
            if (GetUartConfig(UART_RECVIVE_LEN) > (HISGNALLING_MSG_CRC32_LEN + HISIGNALLING_MSG_HEADER_TAIL_LEN)) {
                len = GetUartConfig(UART_RECVIVE_LEN);
                recBuff = GetUartReceiveMsg();
                for (int i = 0; i < len ; i++ ){
                    HISIGNALLING_LOG_INFO("0x%x", recBuff[i]);
                }
                printf("end\n");
                (void)memcpy_s(g_sendUartBuff,
                    (GetUartConfig(UART_RECVIVE_LEN) - HISGNALLING_MSG_CRC32_LEN - HISIGNALLING_MSG_HEADER_TAIL_LEN),
                    &recBuff[HISGNALLING_MSG_FRAME_HEADER_LEN],
                    (GetUartConfig(UART_RECVIVE_LEN) - HISGNALLING_MSG_CRC32_LEN - HISIGNALLING_MSG_HEADER_TAIL_LEN));
                /* 接收到Hi3516DV300数据后，发送回显 */
                HisignallingMsgSend(g_sendUartBuff,
                    (GetUartConfig(UART_RECVIVE_LEN) - HISGNALLING_MSG_CRC32_LEN - HISIGNALLING_MSG_HEADER_TAIL_LEN));
            }
            (void)SetUartRecvFlag(UART_RECV_FALSE);
            ResetUartReceiveMsg();
        }
        TaskMsleep(HISGNALLING_FREE_TASK_TIME);
    }
}

hi_u32 HisignalingMsgTask(hi_void)
{
    hi_u32 ret = 0;
    IoTGpioInit(LED_TEST_GPIO);
    IoTGpioSetDir(LED_TEST_GPIO, IOT_GPIO_DIR_OUT);
    osThreadAttr_t hisignallingAttr = {0};

    hisignallingAttr.stack_size = HISIGNALLING_MSG_TASK_STACK_SIZE;
    hisignallingAttr.priority = HISIGNALLING_MSG_TASK_PRIO;
    hisignallingAttr.name = (hi_char*)"hisignal msg task";

    if (osThreadNew((osThreadFunc_t)HisignallingMsgHandle, NULL, &hisignallingAttr) == NULL) {
        HISIGNALLING_LOG_ERROR("Failed to create hisignaling msg task\r\n");
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}
//SYS_RUN(HisignalingMsgTask);
