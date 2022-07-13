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

#include <hi_task.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <hi_wifi_api.h>
#include <hi_mux.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include "iot_config.h"
#include "iot_log.h"
#include "iot_main.h"
#include "iot_profile.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include <hi_stdlib.h>
#include <hisignalling_protocol.h>
#include <hi_uart.h>
#include <app_demo_uart.h>
#include <iot_uart.h>
#include "iot_gpio_ex.h"




/* attribute initiative to report */
#define TAKE_THE_INITIATIVE_TO_REPORT
#define TWO_SECOND                          (2000)
/* oc request id */
#define CN_COMMADN_INDEX                    "commands/request_id="
#define WECHAT_SUBSCRIBE_control            "slot_num"
#define WECHAT_SUBSCRIBE_channel1           "slot_1"
#define WECHAT_SUBSCRIBE_channel2           "slot_2"
#define WECHAT_SUBSCRIBE_channel3           "slot_3"
#define WECHAT_SUBSCRIBE_channel4           "4"
#define topic_data                          "YT32IOSCAL/Hi38611_mqtt/data"
#define topic_event                         "YT32IOSCAL/Hi38611_mqtt/event"
#define topic_control                       "YT32IOSCAL/Hi38611_mqtt/control"
#define human_detect_io                     7

int g_ligthStatus = -1;
int control_success = 0;
char *control_flag = "failed";
char *message = NULL;
UartDefConfig uartDefConfig = {0};
//int slot1 = 20,slot2 = 20, slot3 = 20, slot4 = 20;
typedef void (*FnMsgCallBack)(hi_gpio_value val);

typedef struct FunctionCallback {
    hi_bool  stop;
    hi_u32 conLost;
    hi_u32 queueID;
    hi_u32 iotTaskID;
    FnMsgCallBack    msgCallBack;
}FunctionCallback;
FunctionCallback g_functinoCallback;

// uart

static void Uart1GpioCOnfig(void)
{
#ifdef ROBOT_BOARD
    IoSetFunc(HI_IO_NAME_GPIO_5, IOT_IO_FUNC_GPIO_5_UART1_RXD);
    IoSetFunc(HI_IO_NAME_GPIO_6, IOT_IO_FUNC_GPIO_6_UART1_TXD);
    /* IOT_BOARD */
#elif defined (EXPANSION_BOARD)
    IoSetFunc(HI_IO_NAME_GPIO_0, IOT_IO_FUNC_GPIO_0_UART1_TXD);
    IoSetFunc(HI_IO_NAME_GPIO_1, IOT_IO_FUNC_GPIO_1_UART1_RXD);
#endif
}

/* CPU Sleep time Set */
unsigned int TaskMsleep(unsigned int ms)
{
    if (ms <= 0) {
        return HI_ERR_FAILURE;
    }
    return hi_sleep((hi_u32)ms);
}

static void DeviceConfigInit(hi_gpio_value val)
{
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_GPIO_IDX_9, val);
}

static void engine_control(unsigned int IO){
    IoTGpioInit(IO);
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE1);
    IOT_LOG_DEBUG("Engine at GPIO:%u started!\n", IO);
    hi_udelay(20000);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("Engine at GPIO:%u stopped!\n", IO);
}

static void engine_start(unsigned int IO){
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE1);
    IOT_LOG_DEBUG("Engine at GPIO:%u started!\n", IO);
}

static void engine_stop(unsigned int IO){
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("Engine at GPIO:%u stopped!\n", IO);
}

static void engine_reinit(unsigned int IO){
    IoTGpioInit(IO);
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("Engine at GPIO:%u initialized!\n", IO);
}

static void all_engine_reinit(){
    engine_reinit(6);
    engine_reinit(7);
    engine_reinit(9);
    engine_reinit(10);
    IOT_LOG_DEBUG("All engines initialized!\n");
}

static void Sensor_init(){
    IoTGpioInit(human_detect_io);
    IoSetFunc(human_detect_io, 0);
    IoTGpioSetDir(human_detect_io, IOT_GPIO_DIR_IN);
    IOT_LOG_DEBUG("Sensor initialized!\n");
}

static int  DeviceMsgCallback(FnMsgCallBack msgCallBack)
{
    g_functinoCallback.msgCallBack = msgCallBack;
    return 0;
}

static void wechatControlDeviceMsg(hi_gpio_value val)
{
    DeviceConfigInit(val);
}


// < this is the callback function, set to the mqtt, and if any messages come, it will be called
// < The payload here is the json string
static void DemoMsgRcvCallBack(int qos, const char *topic, const char *payload)/*定义3861接收到json文件后的操作*/
{
    IOT_LOG_DEBUG("RCVMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", qos, topic, payload);
    printf(strstr(payload, WECHAT_SUBSCRIBE_control));
    /* 云端下发命令后，板端的操作处理 */
     if (strstr(payload, WECHAT_SUBSCRIBE_control) != NULL) {
        //printf(strstr(payload, WECHAT_SUBSCRIBE_control));

        all_engine_reinit();
        if (strstr(payload, WECHAT_SUBSCRIBE_channel1) != NULL) {
            engine_start(6);
            hi_udelay(4000000);
            engine_stop(6);
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
            control_success = 1;
            control_flag = payload;
        }
        else if (strstr(payload, WECHAT_SUBSCRIBE_channel2) != NULL) {
            engine_start(7);
            hi_udelay(4000000);
            engine_stop(7);
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
            control_success = 1;
            control_flag = payload;
        }
        else if (strstr(payload, WECHAT_SUBSCRIBE_channel3) != NULL) {
            engine_start(2);
            hi_udelay(4000000);
            engine_stop(2);
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
            control_success = 1;
            control_flag = payload;
        }
        else if (strstr(payload, WECHAT_SUBSCRIBE_channel4) != NULL) {
            engine_start(10);
            hi_udelay(4000000);
            engine_stop(10);
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
            control_success = 1;
            control_flag = payload;
        }
       
     }

    return HI_NULL;
}

static void DemoMsgRcvsynchronize(int qos, const char *topic, const char *payload)
{
    message = payload;

}

/* publish sample */
/*hi_void IotPublishSample(int slot1, int slot2, int slot3, int slot4)
{
    // reported attribute 
    WeChatProfile weChatProfile = {
        .subscribeType = "type",
        .status.subState = "state",
        .status.subReport = "reported",
        .status.reportVersion = "version",
        .status.Token = "clientToken",
        // report motor 
        .reportAction.subDeviceActionMotor = "slot_1",
        .reportAction.motorActionStatus = slot1, // 0 : motor off 
        // report temperature 
        .reportAction.subDeviceActionTemperature = "slot_2",
        .reportAction.temperatureData = slot2, // 30 :temperature data 
        // report humidity 
        .reportAction.subDeviceActionHumidity = "slot_3",
        .reportAction.humidityActionData = slot3, // humidity data 
        // report light_intensity 
        .reportAction.subDeviceActionLightIntensity = "slot_4",
        .reportAction.lightIntensityActionData = slot4, // 60 : light_intensity 
    };

    // report light 
    if (g_ligthStatus == HI_TRUE) {
        weChatProfile.reportAction.subDeviceActionLight = "light";
        weChatProfile.reportAction.lightActionStatus = 1; // 1: light on 
    } else if (g_ligthStatus == HI_FALSE) {
        weChatProfile.reportAction.subDeviceActionLight = "light";
        weChatProfile.reportAction.lightActionStatus = 0; // 0: light off 
    } else {
        weChatProfile.reportAction.subDeviceActionLight = "light";
        weChatProfile.reportAction.lightActionStatus = 0; // 0: light off 
    }
    //profile report 
    IoTProfilePropertyReport(CONFIG_USER_ID, &weChatProfile1);
}*/

// < this is the demo main task entry,here we will set the wifi/cjson/mqtt ready and
// < wait if any work to do in the while
//static hi_void *DemoEntry(const char *arg)
//{
    
    //int feedback = 0;


    /*6WifiStaReadyWait();
    cJsonInit();
    IoTMain();
    /* 云端下发回调 */
    //IoTSetMsgCallback(DemoMsgRcvCallBack);

    /*if (control_success == 1)
    {
        //Uart1GpioCOnfig();
        feedback = IoTUartWrite(DEMO_UART_NUM, control_flag, 7);
        printf("feedback: %d", feedback);
     
    }*/
    
    /* 主动上报 */
/*#ifdef TAKE_THE_INITIATIVE_TO_REPORT
    while (1) {
        // 用户可以在这调用发布函数进行发布，需要用户自己写调用函数 
        IotPublishSample(slot1,slot2,slot3,slot4); // 发布例程
#endif*/
        //TaskMsleep(TWO_SECOND);
    
    //return NULL;
//}

///////////////////////////////////////////////////////////////////////////////////////////
int SetUartRecvFlag(UartRecvDef def)
{
    if (def == UART_RECV_TRUE) {
        uartDefConfig.g_uartReceiveFlag = HI_TRUE;
    } else {
        uartDefConfig.g_uartReceiveFlag = HI_FALSE;
    }
    
    return uartDefConfig.g_uartReceiveFlag;
}

int GetUartConfig(UartDefType type)
{
    int receive = 0;

    switch (type) {
        case UART_RECEIVE_FLAG:
            receive = uartDefConfig.g_uartReceiveFlag;
            break;
        case UART_RECVIVE_LEN:
            receive = uartDefConfig.g_uartLen;
            break;
        default:
            break;
    }
    return receive;
}

void ResetUartReceiveMsg(void)
{
    (void)memset_s(uartDefConfig.g_receiveUartBuff, sizeof(uartDefConfig.g_receiveUartBuff),
        0x0, sizeof(uartDefConfig.g_receiveUartBuff));
}

unsigned char *GetUartReceiveMsg(void)
{
    return uartDefConfig.g_receiveUartBuff;
}

static hi_void *UartDemoTask(char *param)
{
    hi_u8 uartBuff[UART_BUFF_SIZE] = {0};
    char *recBuff = NULL;
    int feedback_1 = 0;
    int feedback_2 = 0;
    int length = 0;
    unsigned char *detect_flag = "start";
    unsigned char *data_send = "jdata";
    IotGpioValue value = IOT_GPIO_VALUE0;

    hi_unref_param(param);
    printf("Initialize uart demo successfully, please enter some datas via DEMO_UART_NUM port...\n");
    Uart1GpioCOnfig();
    Sensor_init();
    printf("Please wait for 60 seconds for the system to start...\n");
    TaskMsleep(60000);
    printf("Done!\n");
    
    for (;;) 
    {
        int isTimeOut = 0;
      
        IoTGpioGetInputVal(human_detect_io, &value);

        if(value == IOT_GPIO_VALUE1)
        {
            //get from cloud and send to 3516
            IoTUartWrite(DEMO_UART_NUM, detect_flag, 5);
            IoTSetMsgCallback(DemoMsgRcvsynchronize);
            //start to work
            feedback_1 = IoTUartWrite(DEMO_UART_NUM, detect_flag, 5);//
            printf("uart feedback %d\n", feedback_1);
           
            //TaskMsleep(20);
            uartDefConfig.g_uartLen = IoTUartRead(DEMO_UART_NUM, uartBuff, UART_BUFF_SIZE);
            int waitCount = 0;
            while (!((uartDefConfig.g_uartLen > 0) && (uartBuff[0] == 0xaa) && (uartBuff[1] == 0x55))) {
                uartDefConfig.g_uartLen = IoTUartRead(DEMO_UART_NUM, uartBuff, UART_BUFF_SIZE);
                TaskMsleep(50);
                waitCount += 1;
                if(waitCount > 400){
                    isTimeOut = 1;
                    printf("Timeout!\n");
                    break;
                }
                    
            }
            if(isTimeOut)
                continue;
            if (GetUartConfig(UART_RECEIVE_FLAG) == HI_FALSE) {
                (void)memcpy_s(uartDefConfig.g_receiveUartBuff, uartDefConfig.g_uartLen,
                    uartBuff, uartDefConfig.g_uartLen);/*uartBuff中的信息放到uartDefConfig.g_receiveUartBuff*/
                (void)SetUartRecvFlag(UART_RECV_TRUE); 
            }
            printf("UART start\n");
            printf("len:%d\n",  uartDefConfig.g_uartLen);
            recBuff = (char*)malloc(uartDefConfig.g_uartLen-3);

            for (int i = 0; i<uartDefConfig.g_uartLen; i++)
            {
                if(i <= 2)
                {
                    printf("0x%x ", uartBuff[i]);
                }
                else
                {
                    printf("%c ", uartBuff[i]);
                    recBuff[i-3] = uartBuff[i];
                }

            }

            //TaskMsleep(20); /* 20:sleep 20ms */

            /*send to cloud*/
       
            IoTProfilePropertyReport_uart(CONFIG_USER_ID, recBuff);
            //printf("communicatuon completed\n");
            free(recBuff);
            TaskMsleep(50);
            IoTSetMsgCallback(DemoMsgRcvCallBack);
            while (control_success == 0)
            {
                TaskMsleep(50);
            }

            length = strlen(control_flag);
            char len = length+'0';
            IoTUartWrite(DEMO_UART_NUM, len, strlen(len));
            TaskMsleep(10);
            feedback_2 = IoTUartWrite(DEMO_UART_NUM, control_flag, length);
            printf("feedback: %d\n", feedback_2);
        
        }
        TaskMsleep(50);
    }
    return HI_NULL;
}

/*
 * This demo simply shows how to read datas from UART2 port and then echo back.
 */
/*hi_void UartTransmit(hi_void)
{
    hi_u32 ret = 0;

    IotUartAttribute uartAttr = {
        .baudRate = 115200, /* baudRate: 115200 
        .dataBits = 8, /* dataBits: 8bits 
        .stopBits = 1, /* stop bit 
        .parity = 0,
    };
    /* Initialize uart driver 
    ret = IoTUartInit(DEMO_UART_NUM, &uartAttr);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to init uart! Err code = %d\n", ret);
        return;
    }
    /* Create a task to handle uart communication 
    osThreadAttr_t attr = {0};
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 8096;
    attr.priority = UART_DEMO_TASK_PRIORITY;
    attr.name = (hi_char*)"uart demo";
    if (osThreadNew((osThreadFunc_t)UartDemoTask, NULL, &attr) == NULL) {
        printf("Falied to create uart demo task!\n");
    }
}*/
//SYS_RUN(UartTransmit);
///////////////////////////////////////////////////////////////////////////////////////////
// < This is the demo entry, we create a task here,
// and all the works has been done in the demo_entry
#define CN_IOT_TASK_STACKSIZE  0x1000
#define CN_IOT_TASK_PRIOR 25
#define CN_IOT_TASK_NAME "IOTDEMO"

static void AppDemoIot(void)
{
    hi_u32 ret = 0;

    IotUartAttribute uartAttr = {
        .baudRate = 115200, /* baudRate: 115200 */
        .dataBits = 8, /* dataBits: 8bits */
        .stopBits = 1, /* stop bit */
        .parity = 0,
    };
    /* Initialize uart driver */
    ret = IoTUartInit(DEMO_UART_NUM, &uartAttr);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to init uart! Err code = %d\n", ret);
        return;
    }
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    WifiStaReadyWait();
    cJsonInit();
    IoTMain();

    attr.name = "IOTDEMO";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    
    if (osThreadNew((osThreadFunc_t)UartDemoTask, NULL, &attr) == NULL) {
        printf("Falied to create uart demo task!\n");
    }

}

SYS_RUN(AppDemoIot);