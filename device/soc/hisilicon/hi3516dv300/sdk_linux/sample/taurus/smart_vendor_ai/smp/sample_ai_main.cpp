/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
//#include "sample_media_opencv.h"

using namespace std;

/* function : show usage */
static void SmartVendorUsage(char* pchPrgName)
{
    printf("Usage : %s <index> \n", pchPrgName);
    printf("index:\n");
    printf("\t 0) cnn trash_classify(resnet18).\n");
    printf("\t 1) hand classify(yolov2+resnet18).\n");
    printf("\t 2) tennis detect(opencv).\n");
}

/*
 * function    : main()
 * Description : main
 */
int main(int argc, char *argv[])
{
    HI_S32 s32Ret = HI_FAILURE;
    sdk_init();
    /* MIPI is GPIO55, Turn on the backlight of the LCD screen */
    system("cd /sys/class/gpio/;echo 55 > export;echo out > gpio55/direction;echo 1 > gpio55/value");

    // switch (*argv[1]) {
    //     case '0':
    //         SAMPLE_MEDIA_CNN_TRASH_CLASSIFY();
    //         break;
    //     case '1':
    //         SAMPLE_MEDIA_HAND_CLASSIFY();
    //         break;
    //     case '2':
    //         mediaOpencv.SAMPLE_MEDIA_TENNIS_DETECT();
    //         break;
    //     default:
    //         SmartVendorUsage(argv[0]);
    //         break;
    // }
    printCJSONVersion();
    create_objects();
    if(true){ //Stage 2
        SMART_VENDOR_HAND_CLASSIFY();
    }



    sdk_exit();
    SAMPLE_PRT("\nsdk exit success\n");
    return s32Ret;
}
