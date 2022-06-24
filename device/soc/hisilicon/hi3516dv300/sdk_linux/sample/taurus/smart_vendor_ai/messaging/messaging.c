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

void messageUARTSendData(int fd, unsigned char *payload)
{
    unsigned char len = 0;
    while(*(payload + len) != '\0'){
        printf("%c", *(payload + len));
        len += 1;
    }

    //Data packging
    unsigned char frameHeader[2] = {0xAA,0x55}; //Frame header
    unsigned char *dataBuffer;
    (void)memcpy_s(dataBuffer, 2, frameHeader, 2); //length of frame header = 2 bytes
    (void)memcpy_s(&dataBuffer[2], 1, len, 1);
    (void)memcpy_s(&dataBuffer[2], len, payload, len);

    printf("\n");
    printf("length: %u\n", len);
    for (int i = 0; i < len + 3; i++) {
        printf("send data = 0x%x \r\n", *(dataBuffer + i));
    }
    UartSend(fd, dataBuffer, len); //send frame via UART
}
