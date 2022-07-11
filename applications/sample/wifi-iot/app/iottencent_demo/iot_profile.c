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

#include <hi_mem.h>
#include <cJSON.h>
#include <string.h>
#include "iot_main.h"
#include "iot_log.h"
#include "iot_config.h"
#include "iot_profile.h"

// < format the report data to json string mode
static cJSON  *FormateProflleValue(IoTProfileKV_t  *kv)
{
    cJSON  *ret = NULL;
    switch (kv->type) {
        case EN_IOT_DATATYPE_INT:
            ret = cJSON_CreateNumber(kv->iValue);
            break;
        case EN_IOT_DATATYPE_LONG:
            ret = cJSON_CreateNumber((double)(*(long *)kv->value));
            break;
        case EN_IOT_DATATYPE_STRING:
            ret = cJSON_CreateString((const char *)kv->value);
            break;
        default:
            break;
    }
    return ret;
}

static cJSON *MakeKvs(IoTProfileKV_t *kvlst)
{
    cJSON *root;
    cJSON *kv;
    IoTProfileKV_t  *kvInfo;

    // < build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        cJSON_Delete(root);
        root = NULL;
        return root;
    }
    // < add all the property to the properties
    kvInfo = kvlst;
    while (kvInfo != NULL) {
        kv = FormateProflleValue(kvInfo);
        if (kv == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
                root = NULL;
            }
            return root;
        }
        cJSON_AddItemToObject(root, kvInfo->key, kv);
        kvInfo = kvInfo->nxt;
    }
    // < OK, now we return it
    return root;
}

#define CN_PROFILE_SERVICE_KEY_SERVICEID "service_id"
#define CN_PROFILE_SERVICE_KEY_PROPERTIIES "properties"
#define CN_PROFILE_SERVICE_KEY_EVENTTIME "event_time"
#define CN_PROFILE_KEY_SERVICES   "services"
static cJSON *MakeService(IoTProfileService_t *serviceInfo)
{
    cJSON *root;
    cJSON *serviceID;
    cJSON *properties;
    cJSON *eventTime;

    // < build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        cJSON_Delete(root);
        root = NULL;
        return root;
    }
    // < add the serviceID node to the root node
    serviceID = cJSON_CreateString(serviceInfo->serviceID);
    if (serviceID == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        return root;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_SERVICEID, serviceID);
    // < add the properties node to the root
    properties = MakeKvs(serviceInfo->serviceProperty);
    if (properties == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        return root;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_PROPERTIIES, properties);
    // < add the event time (optional) to the root
    if (serviceInfo->eventTime != NULL) {
        eventTime = cJSON_CreateString(serviceInfo->eventTime);
        if (eventTime == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
                root = NULL;
            }
            return root;
        }
        cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_EVENTTIME, eventTime);
    }
    // < OK, now we return it
    cJSON_Delete(properties);
    return root;
}

static cJSON *MakeServices(IoTProfileService_t *serviceInfo)
{
    cJSON *services = NULL;
    cJSON *service;
    IoTProfileService_t  *serviceTmp;

    // < create the services array node
    services = cJSON_CreateArray();
    if (services == NULL) {
        cJSON_Delete(services);
        services = NULL;
        return services;
    }

    serviceTmp = serviceInfo;
    while (serviceTmp != NULL) {
        service = MakeService(serviceTmp);
        if (service == NULL) {
            if (services != NULL) {
                cJSON_Delete(services);
                services = NULL;
            }
            return services;
        }
        cJSON_AddItemToArray(services, service);
        serviceTmp = serviceTmp->nxt;
    }
    // < now we return the services
    return services;
}

// < use this function to make a topic to publish
// < if request_id  is needed depends on the fmt
static char *MakeTopic(const char *fmt, const char *deviceID, const char *requestID)
{
    int len;
    char *ret = NULL;

    len = strlen(fmt) + strlen(deviceID);
    if (requestID != NULL) {
        len += strlen(requestID);
    }

    ret = hi_malloc(0, len);
    if (ret != NULL) {
        if (requestID != NULL) {
            (void)snprintf_s(ret, len, len, fmt, deviceID, requestID);
        } else {
            (void)snprintf_s(ret, len, len, fmt, deviceID);
        }
    }
    return ret;
}

#define CN_PROFILE_CMDRESP_KEY_RETCODE        "result_code"
#define CN_PROFILE_CMDRESP_KEY_RESPNAME       "response_name"
#define CN_PROFILE_CMDRESP_KEY_PARAS          "paras"
static char *MakeProfileCmdResp(IoTCmdResp_t *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *retCode;
    cJSON *respName;
    cJSON *paras;

    // < create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        cJSON_Delete(root);
        return ret;
    }
    // < create retcode and retdesc and add it to the root
    retCode = cJSON_CreateNumber(payload->retCode);
    if (retCode == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_RETCODE, retCode);
    if (payload->respName != NULL) {
        respName = cJSON_CreateString(payload->respName);
        if (respName == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
            }
            return ret;
        }
        cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_RESPNAME, respName);
    }

    if (payload->paras != NULL) {
        paras = MakeKvs(payload->paras);
        if (paras == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
            }
            return ret;
        }
        cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_PARAS, paras);
    }
    // < OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return ret;
}

static char *MakeProfilePropertyReport(IoTProfileService_t *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *services;

    // < create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        cJSON_Delete(root);
        return ret;
    }
    // < create the services array node to the root
    services = MakeServices(payload);
    if (services == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_KEY_SERVICES, services);
    // < OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return ret;
}

#define WECHAT_SUBSCRIBE_TYPE       "update"
#define WECHAT_SUBSCRIBE_VERSION    "1.0.0"
#define WECHAT_SUBSCRIBE_TOKEN      "DeviceSubscribe"
static char *MakeProfileReport(WeChatProfile *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *state;
    cJSON *reported;

    /* create json root node */
    root = cJSON_CreateObject();
    if (root == NULL) {
        return;
    }
    /* state create */
    state = cJSON_CreateObject();
    if (state == NULL) {
        return;
    }
    /* reported create */
    reported = cJSON_CreateObject();
    if (reported == NULL) {
        return;
    }
    /* add root object */
    cJSON_AddItemToObject(root, payload->subscribeType, cJSON_CreateString(WECHAT_SUBSCRIBE_TYPE));
    cJSON_AddItemToObject(root, payload->status.subState, state);
    cJSON_AddItemToObject(state, payload->status.subReport, reported);
    cJSON_AddItemToObject(root, payload->status.reportVersion, cJSON_CreateString(WECHAT_SUBSCRIBE_VERSION));
    cJSON_AddItemToObject(root, payload->status.Token, cJSON_CreateString(WECHAT_SUBSCRIBE_TOKEN));
    /* add reported item */
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionLight,
                            payload->reportAction.lightActionStatus);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionMotor,
                            payload->reportAction.motorActionStatus);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionTemperature,
                            payload->reportAction.temperatureData);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionHumidity,
                            payload->reportAction.humidityActionData);
    cJSON_AddNumberToObject(reported, payload->reportAction.subDeviceActionLightIntensity,
                            payload->reportAction.lightIntensityActionData);
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(state);
    cJSON_Delete(reported);
    cJSON_Delete(root);
    return ret;
}

#define CN_PROFILE_TOPICFMT_TOPIC            "$shadow/operation/19VUBHD786/mqtt"
int IoTProfilePropertyReport(char *deviceID, WeChatProfile *payload)
{
    int ret = -1;
    char *topic;
    char *msg;

    if ((deviceID == NULL) || (payload== NULL)) {
        return ret;
    }
    topic = MakeTopic(CN_PROFILE_TOPICFMT_TOPIC, deviceID, NULL);
    if (topic == NULL) {
        return;
    }
    msg = MakeProfileReport(payload);
    if ((topic != NULL) && (msg != NULL)) {
        ret = IotSendMsg(0, topic, msg);
    }

    hi_free(0, topic);
    cJSON_free(msg);

    return ret;
}