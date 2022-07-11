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

/**
 * STEPS:
 * 1, CONNECT TO THE IOT SERVER
 * 2, SUBSCRIBE  THE DEFAULT TOPIC
 * 3, WAIT FOR ANY MESSAGE COMES OR ANY MESSAGE TO SEND
*/

#include <securec.h>
#include <hi_task.h>
#include <hi_msg.h>
#include <hi_mem.h>
#include <string.h>
#include <stdbool.h>
#include "iot_config.h"
#include "iot_log.h"
#include "iot_hmac.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_watchdog.h"
#include "MQTTClient.h"
#include "iot_errno.h"
#include "iot_main.h"
#include "iot_gpio.h"


// < this is the configuration head
#define CN_IOT_SERVER           "tcp://106.55.124.154:1883" // Tencent iot cloud address
#define CONFIG_COMMAND_TIMEOUT     10000L
#define CN_KEEPALIVE_TIME 50
#define CN_CLEANSESSION   1
#define CN_HMAC_PWD_LEN   65     // < SHA256 IS 32 BYTES AND END APPEND '\0'
#define CN_EVENT_TIME     "1970000100"
#define CN_CLIENTID_FMT   "%s_0_0_%s"      // < This is the cient ID format, deviceID_0_0_TIME
#define CN_QUEUE_WAITTIMEOUT   1000
#define CN_QUEUE_MSGNUM 16
#define CN_QUEUE_MSGSIZE (sizeof(hi_pvoid))

#define CN_TASK_PRIOR 28
#define CN_TASK_STACKSIZE 0X2000
#define CN_TASK_NAME "IoTMain"
#define Signal_LED_IO 12

typedef enum {
    EN_IOT_MSG_PUBLISH = 0,
    EN_IOT_MSG_RECV,
}EnIotMsg;

typedef struct {
    EnIotMsg type;
    int qos;
    const char *topic;
    const char *payload;
}IoTMsg_t;

typedef struct {
    bool  stop;
    unsigned  int conLost;
    unsigned  int queueID;
    unsigned  int iotTaskID;
    fnMsgCallBack msgCallBack;
    MQTTClient_deliveryToken tocken;
}IotAppCb_t;
static IotAppCb_t   gIoTAppCb;

static const char *gDefaultSubscribeTopic[] = {
    /* Tencent iot cloud topic */
    "YT32IOSCAL/Hi38611_mqtt/data",
    "YT32IOSCAL/Hi38611_mqtt/event",
    "YT32IOSCAL/Hi38611_mqtt/control",
};

#define CN_TOPIC_SUBSCRIBE_NUM     (sizeof(gDefaultSubscribeTopic) / sizeof(const char *))
static int MsgRcvCallBack(char *context, char *topic, int topicLen, MQTTClient_message *message)
{
    IoTMsg_t  *msg;
    char *buf;
    unsigned  int bufSize;
    int topiLen = topicLen;

    if (topiLen == 0) {
        topiLen = strlen(topic);
    }
    bufSize = topiLen + 1  + message->payloadlen + 1 + sizeof(IoTMsg_t);
    buf = hi_malloc(0, bufSize);
    if (buf != NULL) {
        msg = (IoTMsg_t *)buf;
        buf += sizeof(IoTMsg_t);
        bufSize -= sizeof(IoTMsg_t);
        msg->qos = message->qos;
        msg->type = EN_IOT_MSG_RECV;
        (void)memcpy_s(buf, bufSize, topic, topiLen);
        buf[topiLen] = '\0';
        msg->topic = buf;
        buf += topiLen + 1;
        bufSize -= (topiLen + 1);
        (void)memcpy_s(buf, bufSize, message->payload, message->payloadlen);
        buf[message->payloadlen] = '\0';
        msg->payload = buf;
        IOT_LOG_DEBUG("RCVMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", msg->qos, msg->topic, msg->payload);
        if (IOT_SUCCESS != osMessageQueuePut(gIoTAppCb.queueID, &msg, 0, CN_QUEUE_WAITTIMEOUT)) {
            IOT_LOG_ERROR("Wrie queue failed\r\n");
            hi_free(0, msg);
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic);
    return 1;
}

// < when the connect lost and this callback will be called
static void ConnLostCallBack(char *context, char *cause)
{
    IOT_LOG_DEBUG("Connection lost:caused by:%s\r\n", cause == NULL? "Unknown" : cause);
    return;
}

static int MqttProcessQueueMsg(MQTTClient client, IoTMsg_t  *msg, MQTTClient_message pubMsg)
{
    int ret = 0;

    switch (msg->type) {
        case EN_IOT_MSG_PUBLISH:
            pubMsg.payload = (void *)msg->payload;
            pubMsg.payloadlen = (int)strlen(msg->payload);
            pubMsg.qos = msg->qos;
            pubMsg.retained = 0;
            ret = MQTTClient_publishMessage(client, msg->topic, &pubMsg, &gIoTAppCb.tocken);
            if (ret != MQTTCLIENT_SUCCESS) {
                IOT_LOG_ERROR("MSGSEND:failed\r\n");
            }
            IOT_LOG_DEBUG("MSGSEND:SUCCESS\r\n");
            gIoTAppCb.tocken++;
            break;
        case EN_IOT_MSG_RECV:
            if (gIoTAppCb.msgCallBack != NULL) {
                gIoTAppCb.msgCallBack(msg->qos, msg->topic, msg->payload); /*接受信息并进行相应的操作，函数体在IoTSetMsgCallback中定义*/
            }
            break;
        default:
            break;
    }
}

// <use this function to deal all the comming message
static int ProcessQueueMsg(MQTTClient client)
{
    unsigned  int     ret;
    unsigned  int     msgSize;
    IoTMsg_t  *msg;
    unsigned  int     timeout;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    timeout = CN_QUEUE_WAITTIMEOUT;
    do {
        msg = NULL;
        msgSize = sizeof(hi_pvoid);
        ret = osMessageQueueGet(gIoTAppCb.queueID, &msg, &msgSize, timeout);/*得到数据，存入msg*/
        if (msg != NULL) {
            IOT_LOG_DEBUG("QUEUEMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", msg->qos, msg->topic, msg->payload);
            MqttProcessQueueMsg(client, msg, pubmsg);
            hi_free(0, msg);
        }
        timeout = 0;  // < continous to deal the message without wait here
    } while (ret == IOT_SUCCESS);

    return 0;
}

int MqttDestory(int ret, MQTTClient client)
{
    if (ret != MQTTCLIENT_SUCCESS) {
        MQTTClient_destroy(&client);
        return -1;
    }
    return MQTTCLIENT_SUCCESS;
}

static void MainEntryProcess(void)
{
    int rc = 0, subQos[CN_TOPIC_SUBSCRIBE_NUM] = {1};

    MQTTClient client = NULL;
    MQTTClient_connectOptions connOpts = MQTTClient_connectOptions_initializer;
    char *clientID = CN_CLIENTID;
    char *userID = CONFIG_USER_ID;
    char *userPwd = hi_malloc(0, CN_HMAC_PWD_LEN);
    if (userPwd == NULL) {
        hi_free(0, clientID);
        return;
    }
    userPwd = CONFIG_USER_PWD;
    connOpts.keepAliveInterval = CN_KEEPALIVE_TIME;
    connOpts.cleansession = CN_CLEANSESSION;
    connOpts.username = userID;
    connOpts.password = userPwd;
    connOpts.MQTTVersion = MQTTVERSION_3_1_1;
    IOT_LOG_DEBUG("CLIENTID:%s USERID:%s USERPWD:%s, IOTSERVER:%s\r\n",
        clientID, userID, userPwd==NULL?"NULL" : userPwd, CN_IOT_SERVER);
    rc = MQTTClient_create(&client, CN_IOT_SERVER, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        if (userPwd != NULL) {
            hi_free(0, userPwd);
        }
        return;
    }
    rc = MQTTClient_setCallbacks(client, NULL, ConnLostCallBack, MsgRcvCallBack, NULL);
    if (MqttDestory(rc, client) != MQTTCLIENT_SUCCESS) {
        return;
    }
    rc = MQTTClient_connect(client, &connOpts);
    if (MqttDestory(rc, client) != MQTTCLIENT_SUCCESS) {
        return;
    }
    for (int i = 0; i < CN_TOPIC_SUBSCRIBE_NUM; i++) {
        rc = MQTTClient_subscribeMany(client, CN_TOPIC_SUBSCRIBE_NUM,
            (char *const *)gDefaultSubscribeTopic, (int *)&subQos[0]);
        if (MqttDestory(rc, client) != MQTTCLIENT_SUCCESS) {
            return;
        }
    }
    IOT_LOG_DEBUG ("Connect success and Subscribe success\r\n");
    IoTGpioSetOutputVal(Signal_LED_IO, IOT_GPIO_VALUE1);
    while (MQTTClient_isConnected(client)) {
        ProcessQueueMsg(client); // < do the job here
        MQTTClient_yield();  // < make the keepalive done
    }
    MQTTClient_disconnect(client, CONFIG_COMMAND_TIMEOUT);
    return;
}

/* MQTT processing entry */
static hi_void *MainEntry(char *arg)
{
    (void)arg;
    while (gIoTAppCb.stop == false) {
        MainEntryProcess();
        IOT_LOG_DEBUG("The connection lost and we will try another connect\r\n");
        IoTGpioSetOutputVal(Signal_LED_IO, IOT_GPIO_VALUE0);
        hi_sleep(1000); /* 1000: cpu sleep 1000ms */
    }
    return NULL;
}
static void engine_init(unsigned int IO){
    IoTGpioInit(IO);
    IoSetFunc(IO, 0);
    IoTGpioSetDir(IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(IO, IOT_GPIO_VALUE0);
    IOT_LOG_DEBUG("GPIO:%u initialized!", IO);
}
int IoTMain(void)
{
    unsigned  int ret = 0;
    hi_task_attr attr = {0};
    engine_init(6);
    engine_init(7);
    engine_init(9);
    engine_init(10);
    IoTGpioInit(Signal_LED_IO);
    IoSetFunc(Signal_LED_IO, 0);
    IoTGpioSetDir(Signal_LED_IO, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(Signal_LED_IO, IOT_GPIO_VALUE0);

    gIoTAppCb.queueID = osMessageQueueNew(CN_QUEUE_MSGNUM, CN_QUEUE_MSGSIZE, NULL);
    if (ret != IOT_SUCCESS) {
        IOT_LOG_ERROR("Create the msg queue Failed\r\n");
    }

    attr.stack_size = CN_TASK_STACKSIZE;
    attr.task_prio = CN_TASK_PRIOR;
    attr.task_name = CN_TASK_NAME;
    ret = hi_task_create(&gIoTAppCb.iotTaskID, &attr, MainEntry, NULL);
    if (ret != IOT_SUCCESS) {
        IOT_LOG_ERROR("Create the Main Entry Failed\r\n");
    }

    return 0;
}

int IoTSetMsgCallback(fnMsgCallBack msgCallback)
{
    gIoTAppCb.msgCallBack = msgCallback;/*赋予函数*/
    return 0;
}

int IotSendMsg(int qos, const char *topic, const char *payload)
{
    int rc = -1;
    IoTMsg_t  *msg;
    char *buf;
    unsigned  int bufSize;

    bufSize = strlen(topic) + 1 +strlen(payload) + 1 + sizeof(IoTMsg_t);
    buf = hi_malloc(0, bufSize);
    if (buf != NULL) {
        msg = (IoTMsg_t *)buf;
        buf += sizeof(IoTMsg_t);
        bufSize -= sizeof(IoTMsg_t);
        msg->qos = qos;
        msg->type = EN_IOT_MSG_PUBLISH;
        (void)memcpy_s(buf, bufSize, topic, strlen(topic));
        buf[strlen(topic)] = '\0';
        msg->topic = buf;
        buf += strlen(topic) + 1;
        bufSize -= (strlen(topic) + 1);
        (void)memcpy_s(buf, bufSize, payload, strlen(payload));
        buf[strlen(payload)] = '\0';
        msg->payload = buf;
        IOT_LOG_DEBUG("SNDMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", msg->qos, msg->topic, msg->payload);
        if (IOT_SUCCESS != osMessageQueuePut(gIoTAppCb.queueID, &msg, 0, CN_QUEUE_WAITTIMEOUT)) {
            IOT_LOG_ERROR("Write queue failed\r\n");
            hi_free(0, msg);
        } else {
            rc = 0;
        }
    }
    return rc;
}