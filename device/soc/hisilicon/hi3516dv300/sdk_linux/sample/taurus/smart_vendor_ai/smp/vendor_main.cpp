/*
 *       _____ __  __          _____ _______  __      ________ _   _ _____   ____  _____
 *      / ____|  \/  |   /\   |  __ \__   __| \ \    / /  ____| \ | |  __ \ / __ \|  __ \
 *     | (___ | \  / |  /  \  | |__) | | |     \ \  / /| |__  |  \| | |  | | |  | | |__) |
 *      \___ \| |\/| | / /\ \ |  _  /  | |      \ \/ / |  __| | . ` | |  | | |  | |  _  /
 *      ____) | |  | |/ ____ \| | \ \  | |       \  /  | |____| |\  | |__| | |__| | | \ \
 *     |_____/|_|  |_/_/    \_\_|  \_\ |_|        \/   |______|_| \_|_____/ \____/|_|  \_\
 *
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

#include <iostream>
#include "unistd.h"
#include "sdk.h"
#include "smart_vendor_classification.h"
#include "messaging.h"
#include "data_store.h"

/* Display help info */
static void SmartVendorHelp()
{
    printf("\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
    puts("\n"
        " ________  _____ ______   ________  ________  _________        ___      ___ _______   ________   ________  ________  ________     \n"
        "|\\   ____\\|\\   _ \\  _   \\|\\   __  \\|\\   __  \\|\\___   ___\\     |\\  \\    /  /|\\  ___ \\ |\\   ___  \\|\\   ___ \\|\\   __  \\|\\   __  \\    \n"
        "\\ \\  \\___|\\ \\  \\\\\\__\\ \\  \\ \\  \\|\\  \\ \\  \\|\\  \\|___ \\  \\_|     \\ \\  \\  /  / | \\   __/|\\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \\  \\|\\  \\ \\  \\|\\  \\   \n"
        " \\ \\_____  \\ \\  \\\\|__| \\  \\ \\   __  \\ \\   _  _\\   \\ \\  \\       \\ \\  \\/  / / \\ \\  \\_|/_\\ \\  \\\\ \\  \\ \\  \\ \\\\ \\ \\  \\\\\\  \\ \\   _  _\\  \n"
        "  \\|____|\\  \\ \\  \\    \\ \\  \\ \\  \\ \\  \\ \\  \\\\  \\|   \\ \\  \\       \\ \\    / /   \\ \\  \\_|\\ \\ \\  \\\\ \\  \\ \\  \\_\\\\ \\ \\  \\\\\\  \\ \\  \\\\  \\| \n"
        "    ____\\_\\  \\ \\__\\    \\ \\__\\ \\__\\ \\__\\ \\__\\\\ _\\    \\ \\__\\       \\ \\__/ /     \\ \\_______\\ \\__\\\\ \\__\\ \\_______\\ \\_______\\ \\__\\\\ _\\ \n"
        "   |\\_________\\|__|     \\|__|\\|__|\\|__|\\|__|\\|__|    \\|__|        \\|__|/       \\|_______|\\|__| \\|__|\\|_______|\\|_______|\\|__|\\|__|\n"
        "   \\|_________|                                                                                                                   \n"
    );
    printf("\n SMART VENDING MACHINE 3516-VISION CLIENT \n");
    printf(" Version 1.0\n");
    printf(" Developed by: Sidi Liang\n");
    printf(" Copyright (c) 2022 Sidi Liang\n");
    printf(" Licensed under the Apache License, Version 2.0\n");
    printf(" Part of the Smart Vendor Project:\n");
    printf("\t Sidi Liang, Yi Liu, Yicong Liu\n");
    printf("\n*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n");
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


    /* Main Loop */
    while(true){
        //Stage 1
        waitForStartSignal();
        //Stage 2
        SMART_VENDOR_HAND_CLASSIFY();
        SAMPLE_PRT("\nHand classification ended, restarting!\n");
        //(void)getchar();
    }



    sdk_exit();
    SAMPLE_PRT("\n Program exited. Bye.\n");
    return s32Ret;
}
