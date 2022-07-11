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

#ifndef APP_DEMO_UART_H
#define APP_DEMO_UART_H

#define UART_BUFF_SIZE           1024
#define WRITE_BY_INT
#define UART_DEMO_TASK_STAK_SIZE 2048
#define UART_DEMO_TASK_PRIORITY  25
#define DEMO_UART_NUM            HI_UART_IDX_1

typedef enum {
    UART_RECEIVE_FLAG = 0,
    UART_RECVIVE_LEN,
    UART_SEND_FLAG = 2,
    UART_SEND_LEN
}UartDefType;

typedef enum {
    UART_RECV_TRUE = 0,
    UART_RECV_FALSE,
}UartRecvDef;

typedef struct {
    unsigned int uartChannel;
    unsigned char g_receiveUartBuff[UART_BUFF_SIZE];
    int g_uartReceiveFlag;
    int g_uartLen;
}UartDefConfig;

int SetUartRecvFlag(UartRecvDef def);
int GetUartConfig(UartDefType type);
void ResetUartReceiveMsg(void);
unsigned char *GetUartReceiveMsg(void);
#endif