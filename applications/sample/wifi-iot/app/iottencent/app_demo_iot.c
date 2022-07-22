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


/* attribute initiative to report */
#define TAKE_THE_INITIATIVE_TO_REPORT
#define TWO_SECOND                          (2000)
/* oc request id */
#define CN_COMMADN_INDEX                    "commands/request_id="
#define WECHAT_SUBSCRIBE_control            "chosen_slot"
#define WECHAT_SUBSCRIBE_channel1           "slot_1"
#define WECHAT_SUBSCRIBE_channel2           "slot_2"
#define WECHAT_SUBSCRIBE_channel3           "slot_3"
#define WECHAT_SUBSCRIBE_channel4           "slot_4"
#define topic_data                          "YT32IOSCAL/Hi38611_mqtt/data"
#define topic_event                         "YT32IOSCAL/Hi38611_mqtt/event"
#define topic_control                       "YT32IOSCAL/Hi38611_mqtt/control"

int g_ligthStatus = -1;
int slot1 = 20,slot2 = 20, slot3 = 20, slot4 = 20;
typedef void (*FnMsgCallBack)(hi_gpio_value val);

typedef struct FunctionCallback {
    hi_bool  stop;
    hi_u32 conLost;
    hi_u32 queueID;
    hi_u32 iotTaskID;
    FnMsgCallBack    msgCallBack;
}FunctionCallback;
FunctionCallback g_functinoCallback;

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
    IOT_LOG_DEBUG("Engine at GPIO:%u started!", IO);
    hi_udelay(20000);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("Engine at GPIO:%u stopped!", IO);
}

static void engine_start(unsigned int IO){
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE1);
    IOT_LOG_DEBUG("Engine at GPIO:%u started!", IO);
}

static void engine_stop(unsigned int IO){
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("Engine at GPIO:%u stopped!", IO);
}

static void engine_reinit(unsigned int IO){
    IoTGpioInit(IO);
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("Engine at GPIO:%u initialized!", IO);
}

static void all_engine_reinit(){
    engine_reinit(6);
    engine_reinit(7);
    engine_reinit(9);
    engine_reinit(10);
    IOT_LOG_DEBUG("All engines initialized!");
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
    int IO = 0;
    /* 云端下发命令后，板端的操作处理 */
    //if (strcmp(topic, topic_data)==0){

     if (strstr(payload, WECHAT_SUBSCRIBE_control) != NULL) {
        all_engine_reinit();
        if (strstr(payload, WECHAT_SUBSCRIBE_channel1) != NULL) {
            engine_start(6);
            hi_udelay(4000000);
            engine_stop(6);
            slot1--;
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
        }
        else if (strstr(payload, WECHAT_SUBSCRIBE_channel2) != NULL) {
            engine_start(7);
            hi_udelay(4000000);
            engine_stop(7);
            slot2--;
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
        }
        else if (strstr(payload, WECHAT_SUBSCRIBE_channel3) != NULL) {
            engine_start(2);
            hi_udelay(4000000);
            engine_stop(2);
            slot3--;
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
        }
        else if (strstr(payload, WECHAT_SUBSCRIBE_channel4) != NULL) {
            engine_start(10);
            hi_udelay(4000000);
            engine_stop(10);
            slot4--;
            wechatControlDeviceMsg(HI_GPIO_VALUE1);
        }
       
     }

    //}
    return HI_NULL;
}

/* publish sample */
hi_void IotPublishSample(int slot1, int slot2, int slot3, int slot4)
{
    /* reported attribute */
    WeChatProfile weChatProfile = {
        .subscribeType = "type",
        .status.subState = "state",
        .status.subReport = "reported",
        .status.reportVersion = "version",
        .status.Token = "clientToken",
        /* report motor */
        .reportAction.subDeviceActionMotor = "slot_1",
        .reportAction.motorActionStatus = slot1, /* 0 : motor off */
        /* report temperature */
        .reportAction.subDeviceActionTemperature = "slot_2",
        .reportAction.temperatureData = slot2, /* 30 :temperature data */
        /* report humidity */
        .reportAction.subDeviceActionHumidity = "slot_3",
        .reportAction.humidityActionData = slot3, /* humidity data */
        /* report light_intensity */
        .reportAction.subDeviceActionLightIntensity = "slot_4",
        .reportAction.lightIntensityActionData = slot4, /* 60 : light_intensity */
    };

    /* report light */
    if (g_ligthStatus == HI_TRUE) {
        weChatProfile.reportAction.subDeviceActionLight = "light";
        weChatProfile.reportAction.lightActionStatus = 1; /* 1: light on */
    } else if (g_ligthStatus == HI_FALSE) {
        weChatProfile.reportAction.subDeviceActionLight = "light";
        weChatProfile.reportAction.lightActionStatus = 0; /* 0: light off */
    } else {
        weChatProfile.reportAction.subDeviceActionLight = "light";
        weChatProfile.reportAction.lightActionStatus = 0; /* 0: light off */
    }
    /* profile report */
    IoTProfilePropertyReport(CONFIG_USER_ID, &weChatProfile);
}

// < this is the demo main task entry,here we will set the wifi/cjson/mqtt ready and
// < wait if any work to do in the while
static hi_void *DemoEntry(const char *arg)
{


    WifiStaReadyWait();
    cJsonInit();
    IoTMain();
    /* 云端下发回调 */
    IoTSetMsgCallback(DemoMsgRcvCallBack);
    
    /* 主动上报 */
#ifdef TAKE_THE_INITIATIVE_TO_REPORT
    while (1) {
        /* 用户可以在这调用发布函数进行发布，需要用户自己写调用函数 */
        IotPublishSample(slot1,slot2,slot3,slot4); // 发布例程
#endif
        TaskMsleep(TWO_SECOND);
    }
    return NULL;
}

// < This is the demo entry, we create a task here,
// and all the works has been done in the demo_entry
#define CN_IOT_TASK_STACKSIZE  0x1000
#define CN_IOT_TASK_PRIOR 25
#define CN_IOT_TASK_NAME "IOTDEMO"

static void AppDemoIot(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();

    attr.name = "IOTDEMO";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    if (osThreadNew((osThreadFunc_t)DemoEntry, NULL, &attr) == NULL) {
        printf("[mqtt] Falied to create IOTDEMO!\n");
    }
}

SYS_RUN(AppDemoIot);