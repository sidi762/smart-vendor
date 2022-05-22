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

#include <stddef.h>
#include <stdio.h>
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_gpio_ex.h"
#include "oled_fonts.h"
#include "oled_ssd1306.h"

#define OLED_I2C_IDX 0

#define OLED_WIDTH    (128)
#define OLED_I2C_ADDR 0x78 // 默认地址为 0x78
#define OLED_I2C_CMD 0x00 // 0000 0000       写命令
#define OLED_I2C_DATA 0x40 // 0100 0000(0x40) 写数据
#define OLED_I2C_BAUDRATE (400 * 1000) // 400k

#define DELAY_100_MS (100 * 1000)

typedef struct {
    /** Pointer to the buffer storing data to send */
    unsigned char *sendBuf;
    /** Length of data to send */
    unsigned int  sendLen;
    /** Pointer to the buffer for storing data to receive */
    unsigned char *receiveBuf;
    /** Length of data received */
    unsigned int  receiveLen;
} IotI2cData;

static uint32_t I2cWiteByte(uint8_t regAddr, uint8_t byte)
{
    unsigned int id = OLED_I2C_IDX;
    uint8_t buffer[] = {regAddr, byte};
    IotI2cData i2cData = {0};

    i2cData.sendBuf = buffer;
    i2cData.sendLen = sizeof(buffer) / sizeof(buffer[0]);

    return IoTI2cWrite(id, OLED_I2C_ADDR, i2cData.sendBuf, i2cData.sendLen);
}

/**
 * @brief Write a command byte to OLED device.
 *
 * @param cmd the commnad byte to be writen.
 * @return Returns {@link IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 */
static uint32_t WriteCmd(uint8_t cmd)
{
    return I2cWiteByte(OLED_I2C_CMD, cmd);
}

/**
 * @brief Write a data byte to OLED device.
 *
 * @param cmd the data byte to be writen.
 * @return Returns {@link IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 */
static uint32_t WriteData(uint8_t data)
{
    return I2cWiteByte(OLED_I2C_DATA, data);
}

/**
 * @brief ssd1306 OLED Initialize.
 */
uint32_t OledInit(void)
{
    static const uint8_t initCmds[] = {
        0xAE, // --display off
        0x00, // ---set low column address
        0x10, // ---set high column address
        0x40, // --set start line address
        0xB0, // --set page address
        0x81, // contract control
        0xFF, // --128
        0xA1, // set segment remap
        0xA6, // --normal / reverse
        0xA8, // --set multiplex ratio(1 to 64)
        0x3F, // --1/32 duty
        0xC8, // Com scan direction
        0xD3, // -set display offset
        0x00,
        0xD5, // set osc division
        0x80,
        0xD8, // set area color mode off
        0x05,
        0xD9, // Set Pre-Charge Period
        0xF1,
        0xDA, // set com pin configuartion
        0x12,
        0xDB, // set Vcomh
        0x30,
        0x8D, // set charge pump enable
        0x14,
        0xAF, // --turn on oled panel
    };

    IoTGpioInit(13); /* 初始化gpio13 */
    IoSetFunc(13, 6); /* gpio13使用6功能 */
    IoTGpioInit(14); /* 初始化gpio14 */
    IoSetFunc(14, 6); /* gpio14使用6功能 */

    IoTI2cInit(0, OLED_I2C_BAUDRATE);

    for (size_t i = 0; i < ARRAY_SIZE(initCmds); i++) {
        uint32_t status = WriteCmd(initCmds[i]);
        if (status != IOT_SUCCESS) {
            return status;
        }
    }
    return IOT_SUCCESS;
}

void OledSetPosition(uint8_t x, uint8_t y)
{
    WriteCmd(0xb0 + y);
    WriteCmd(((x & 0xf0) >> 4) | 0x10); /* 在0xf0右移4位，与0x10或，实现了写数据 */
    WriteCmd(x & 0x0f);
}

/* 全屏填充 */
void OledFillScreen(uint8_t fillData)
{
    for (uint8_t m = 0; m < 8; m++) { /* 循环8次实现横屏填充 */
        WriteCmd(0xb0 + m);
        WriteCmd(0x00);
        WriteCmd(0x10);

        for (uint8_t n = 0; n < 128; n++) { /* 循环128次实现竖屏填充 */
            WriteData(fillData);
        }
    }
}

/**
 * @brief 8*16 typeface
 * @param x: write positon start from x axis
 * @param y: write positon start from y axis
 * @param ch: write data
 * @param font: selected font
 */
void OledShowChar(uint8_t x, uint8_t y, uint8_t ch, Font font)
{
    uint8_t c = ch - ' '; // 得到偏移后的值
    uint8_t b = x;
    uint8_t d = y;
    if (b > OLED_WIDTH - 1) {
        b = 0;
        d = d + 2; /* 得到偏移后的值2 */
    }

    if (font == FONT8_X16) {
        OledSetPosition(b, d);
        for (uint8_t i = 0; i < 8; i++) { /* 循环8次实现横屏填充 */
            WriteData(g_f8X16[c * 16 + i]); /* 循环16次实现横屏填充 */
        }

        OledSetPosition(b, d + 1);
        for (uint8_t i = 0; i < 8; i++) { /* 循环8次实现横屏填充 */
            WriteData(g_f8X16[c * 16 + i + 8]); /* 循环16次实现横屏填充8列 */
        }
    } else {
        OledSetPosition(b, d);
        for (uint8_t i = 0; i < 6; i++) { /* 循环6次实现横屏填充 */
            WriteData(g_f6X8[c][i]);
        }
    }
}

void OledShowString(uint8_t x, uint8_t y, const char* str, Font font)
{
    uint8_t j = 0;
    uint8_t b = x;
    uint8_t d = y;
    if (str == NULL) {
        printf("param is NULL,Please check!!!\r\n");
        return;
    }

    while (str[j]) {
        OledShowChar(b, d, str[j], font);
        b += 8; /* 循环8次实现横屏填充 */
        if (b > 120) { /* 循环120次实现横屏填充 */
            b = 0;
            d += 2; /* 循环2次实现横屏填充 */
        }
        j++;
    }
}