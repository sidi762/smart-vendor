/*
 * Copyright (c) 2022 Sidi Liang.
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

#include <iostream>
#include "unistd.h"
#include "sdk.h"
#include "smart_vendor_classification.h"
#include "messaging.h"
#include "data_store.h"

/* Display help info */
static void SmartVendorHelp()
{
    printf("\nSmart vending machine 3516-vision client \n");
    printf("Version 1.0\n");
    printf("Developed by:\n");
    printf("\t Sidi Liang\n");
    printf("Part of the Smart Vendor Project:\n");
    printf("\t Sidi Liang, Yi Liu, Yicong Liu\n");
}

/*
 * Main Entry
 */
int main(int argc, char *argv[])
{
    HI_S32 s32Ret = HI_FAILURE;
    sdk_init();
    /* MIPI is GPIO55, Turn on the backlight of the LCD screen */
    system("cd /sys/class/gpio/;echo 55 > export;echo out > gpio55/direction;echo 1 > gpio55/value");
    SmartVendorHelp();

    fileInit();

    while(true){ //Stage 2
        SMART_VENDOR_HAND_CLASSIFY();
        SAMPLE_PRT("\nHand classification ended, press enter to restart!\n");
        (void)getchar();
    }



    sdk_exit();
    SAMPLE_PRT("\n Program exited. Bye.\n");
    return s32Ret;
}
