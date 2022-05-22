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
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_watchdog.h"
#include "iot_pwm.h"

#define LED_INTERVAL_TIME_US 300000
#define LED_TASK_STACK_SIZE 512
#define LED_TASK_PRIO 25

static int g_beepState = 1;
static int g_iState = 0;
#define IOT_PWM_PORT_PWM0   0
#define IOT_PWM_BEEP        9
#define IOT_GPIO_KEY        8

static void *PWMBeepTask(const char *arg)
{
    while (1) {
        if (g_beepState) {
            IoTPwmStart(IOT_PWM_PORT_PWM0, 50, 4000); /* 占空比50 / 4000,频率4000 */
        } else {
            IoTPwmStop(IOT_PWM_PORT_PWM0);
        }
        if (g_iState == 0xffff) {
            g_iState = 0;
            break;
        }
    }
}

static void OnButtonPressed(const char *arg)
{
    (void) arg;
    g_beepState = !g_beepState;
    printf("PRESS_KEY!\n");
    g_iState++;
}

static void StartPWMBeepTask(void)
{
    osThreadAttr_t attr;

    IoTGpioInit(IOT_GPIO_KEY);
    IoSetFunc(IOT_GPIO_KEY, 0);
    IoTGpioSetDir(IOT_GPIO_KEY, IOT_GPIO_DIR_IN);
    IoSetPull(IOT_GPIO_KEY, IOT_IO_PULL_UP);
    IoTGpioRegisterIsrFunc(IOT_GPIO_KEY, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW, OnButtonPressed, NULL);
    IoTGpioInit(IOT_PWM_BEEP);
    IoSetFunc(IOT_PWM_BEEP, 5); /* 设置IO5的功能 */
    IoTGpioSetDir(IOT_PWM_BEEP, IOT_GPIO_DIR_OUT);
    IoTPwmInit(IOT_PWM_PORT_PWM0);
    IoTWatchDogDisable();

    attr.name = "PWMBeepTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024; /* 堆栈大小为1024 */
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)PWMBeepTask, NULL, &attr) == NULL) {
        printf("[StartPWMBeepTask] Falied to create PWMBeepTask!\n");
    }
}

APP_FEATURE_INIT(StartPWMBeepTask);