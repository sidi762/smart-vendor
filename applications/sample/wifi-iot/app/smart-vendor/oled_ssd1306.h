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

#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include <stdint.h>

/**
 * @brief ssd1306 OLED Initialize.
 */
uint32_t OledInit(void);

/**
 * @brief Set cursor position
 * @param x the horizontal posistion of cursor
 * @param y the vertical position of cursor
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 */
void OledSetPosition(uint8_t x, uint8_t y);

void OledFillScreen(uint8_t fillData);

enum Font {
    FONT6_X8 = 1,
    FONT8_X16
};
typedef enum Font Font;

void OledShowChar(uint8_t x, uint8_t y, uint8_t ch, Font font);
void OledShowString(uint8_t x, uint8_t y, const char* str, Font font);

#endif // OLED_SSD1306_H
