/*
  Copyright (c) 2022 Sidi Liang

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cJSON.h"
#include "messaging.h"
#include "json_helper.h"

/* message receive */
int messageUARTRcvData(int fd, unsigned char *buf, unsigned int len)
{
    int i = 0, readLen = 0;
    unsigned int RecvLen = len;

    /* Hi3516dv300 uart read */
    readLen = UartRead(fd, buf, RecvLen, 1000); /* 1000 :time out */
    if (readLen <= 0) {
        printf("uart_read data failed\r\n");
        return 0;
    }
    printf("read_len=%d\r\n", readLen);

    /* 输出收到的数据 */
    for (i = 0; i < RecvLen; i++) {
        printf("0x%x ", buf[i]);
    }
    printf("\r\n");

    return 1;
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
