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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_errno.h"

#include "oled_ssd1306.h"

#define AHT20_BAUDRATE (400 * 1000)
#define AHT20_I2C_IDX 0

static void OledmentTask(const char *arg)
{
    (void)arg;
    OledInit();
    OledFillScreen(0);
    IoTI2cInit(AHT20_I2C_IDX, AHT20_BAUDRATE);

    OledShowString(20, 3, "Hello  World", 1); /* 屏幕第20列3行显示1行 */
}

static void OledDemo(void)
{
    osThreadAttr_t attr;
    attr.name = "OledmentTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096; /* 任务大小4096 */
    attr.priority = osPriorityNormal;

    if (osThreadNew(OledmentTask, NULL, &attr) == NULL) {
        printf("[OledDemo] Falied to create OledmentTask!\n");
    }
}

APP_FEATURE_INIT(OledDemo);