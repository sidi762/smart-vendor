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

#ifndef IOT_PROFILE_H
#define IOT_PROFILE_H

#include <hi_types_base.h>
#include "iot_config.h"

#define OC_BEEP_STATUS_ON       ((hi_u8) 0x01)
#define OC_BEEP_STATUS_OFF      ((hi_u8) 0x00)

// < enum all the data type for the oc profile
typedef enum {
    EN_IOT_DATATYPE_INT = 0,
    EN_IOT_DATATYPE_LONG,
    EN_IOT_DATATYPE_FLOAT,
    EN_IOT_DATATYPE_DOUBLE,
    EN_IOT_DATATYPE_STRING,           // < must be ended with '\0'
    EN_IOT_DATATYPE_LAST,
}IoTDataType_t;

typedef enum {
    OC_LED_ON = 1,
    OC_LED_OFF
}OcLedValue;

typedef struct {
    void                            *nxt; // < ponit to the next key
    const char                      *key;
    const char                      *value;
    hi_u32                          iValue;
    IoTDataType_t                   type;
}IoTProfileKV_t;

typedef struct {
    void *nxt;
    char *serviceID;
    char *eventTime;
    IoTProfileKV_t *serviceProperty;
}IoTProfileService_t;

typedef struct {
    int  retCode;           // < response code, 0 success while others failed
    const char   *respName; // < response name
    const char   *requestID; // < specified by the message command
    IoTProfileKV_t  *paras; // < the command paras
}IoTCmdResp_t;

typedef struct {
    const char *subState;
    const char *subReport;
    const char *reportVersion;
    const char *Token;
}WeChatProfileStatus;

typedef struct {
    int lightActionStatus;
    int motorActionStatus;
    int temperatureData;
    int humidityActionData;
    int lightIntensityActionData;
    const char *subDeviceActionLight;
    const char *subDeviceActionMotor;
    const char *subDeviceActionTemperature;
    const char *subDeviceActionHumidity;
    const char *subDeviceActionLightIntensity;
}WeChatProfileReporte;

typedef struct {
    const char *subscribeType;
    WeChatProfileStatus status;
    WeChatProfileReporte reportAction;
}WeChatProfile;

/**
 * use this function to report the property to the iot platform
*/
int IoTProfilePropertyReport(char *deviceID, WeChatProfile *payload);
void cJsonInit(void);
void WifiStaReadyWait(void);
#endif