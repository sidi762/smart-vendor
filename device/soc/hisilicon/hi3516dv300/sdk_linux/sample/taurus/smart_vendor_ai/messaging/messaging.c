/*
 * Copyright (c) 2022 Sidi Liang.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cJSON.h"
#include "messaging.h"
#include "json_helper.h"
#include "data_store.h"

#define START_COMMAND "start"
#define DATA_COMMAND "jdata"
#define SUCCESS_COMMAND "success"
#define NUMBER_OF_SLOTS 4

/* message receive */
int messageUARTRcvData(int fd, unsigned char *buf, unsigned int len)
{
    int i = 0, readLen = 0;
    unsigned int RecvLen = len;

    /* Hi3516dv300 uart read */
    readLen = UartRead(fd, buf, RecvLen, 5000); /* 5000 :time out */
    if (readLen <= 0) {
        printf("\nWaiting for START signal from UART\r\n");
        return 0;
    }
    printf("read_len=%d\r\n", readLen);

    /* 输出收到的数据 */
    for (i = 0; i < RecvLen; i++) {
        printf("%c", buf[i]);
    }
    printf("\r\n");

    return 1;
}

int waitForStartSignal()
{
    int uartFd = 0;
    /* uart open init */
    uartFd = UartOpenInit();
    if (uartFd < 0) {
        printf("uart1 open failed\r\n");
    } else {
        printf("uart1 open successed\r\n");
    }
    unsigned char* dataBuffer = (char *) malloc(5);
    while (1){
        if(messageUARTRcvData(uartFd, dataBuffer, 5)){
            if(strstr(dataBuffer, START_COMMAND)){
                free(dataBuffer);
                return 1;
            }else if(strstr(dataBuffer, DATA_COMMAND)){
                unsigned char* jsonDataBuffer = (char *) malloc(255);
                messageUARTRcvData(uartFd, jsonDataBuffer, 255);
                SlotInfo recvVendorData[NUMBER_OF_SLOTS];
                jsonToVendorData(jsonDataBuffer, recvVendorData);
                updateMemVendorData(recvVendorData);
                saveVendorDataToFile(recvVendorData,
                    (sizeof(recvVendorData)/sizeof(recvVendorData[0])));
                free(jsonDataBuffer);
            }
        }
    }
}

int waitForSuccessSignal()
{
    int uartFd = 0;
    /* uart open init */
    uartFd = UartOpenInit();
    if (uartFd < 0) {
        printf("uart1 open failed\r\n");
    } else {
        printf("uart1 open successed\r\n");
    }
    unsigned char* dataBuffer = (char *) malloc(10);//To be confirmed
    while (1){
        if(messageUARTRcvData(uartFd, dataBuffer, 10)){
            if(strstr(dataBuffer, SUCCESS_COMMAND)){
                free(dataBuffer);
                //TBD: Update data
                remainingNumUpdate(1);
                //Sync data
                messageUARTSendData(uartFd, memVendorDataToShadow());
                return 1;
            }
        }
    }
}

void messageUARTSendData(int fd, char *payload)
{
    int lengthInt = 0;
    while(*(payload + lengthInt) != '\0'){
        printf("%c", *(payload + lengthInt));
        lengthInt += 1;
    }
    if(lengthInt > 255){
        printf("Error: payload too long! Max length is 255");
        return;
    }
    unsigned char len = (unsigned char)lengthInt;
    //Data packging
    unsigned char frameHeader[2] = {0xAA,0x55}; //Frame header
    unsigned char *dataBuffer = (char *) malloc(lengthInt + 4);

    //Frame header
    *dataBuffer = 0xAA;
    *(dataBuffer + 1) = 0x55;

    //Payload length
    *(dataBuffer + 2) = len;

    //Payload
    for(int i = 3; i <= lengthInt + 3; i+=1){
        *(dataBuffer + i) = *(payload + i - 3);
    }

    printf("\n");
    printf("length: %u\n", len);
    for (int i = 0; i < len + 3; i++) {
        printf("send data = 0x%x \r\n", *(dataBuffer + i));
    }

    UartSend(fd, dataBuffer, len + 3); //send frame via UART
    free(dataBuffer);
}
