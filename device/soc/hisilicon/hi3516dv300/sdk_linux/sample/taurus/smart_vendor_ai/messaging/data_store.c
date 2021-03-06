/*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cJSON.h"
#include "messaging.h"
#include "json_helper.h"
#include "data_store.h"

#define NUMBER_OF_SLOTS 4
#define CONFIG_FILE_PATH "/userdata/vendor_data/vendor.json"
#define CONFIG_FILE_DIR "/userdata/vendor_data"

SlotInfo vendorData[NUMBER_OF_SLOTS] = {0};

/*
SlotInfo testVendorData;
testVendorData.slot_num = 4;
strcpy(testVendorData.product_name, "N95 Mask");
strcpy(testVendorData.product_price_string, "10.50 Yuan");
testVendorData.product_price = 10.5;
testVendorData.remaining_num = 3;
*/

SlotInfo loadVendorDataFromFile()
{

}

void saveVendorDataToFile(SlotInfo vendorData[], int len)
{
    char* jsonString = vendorDataToJson(vendorData, len);
    FILE *fp = NULL;
    fp = fopen(CONFIG_FILE_PATH, "w+");
    fputs(jsonString, fp);
    fclose(fp);
    return;
}

void updateMemVendorData(SlotInfo newVendorData[]){
    for(int i = 0; i < NUMBER_OF_SLOTS; i += 1){
        vendorData[i] = newVendorData[i];
    }
}

int remainingNumUpdate(int slotNum){
    if(vendorData[slotNum - 1].remaining_num - 1 >= 0){
        vendorData[slotNum - 1].remaining_num -= 1;
        return 1;
    }else{
        return -1;
    }
}

char* memVendorDataToShadow(){
    return vendorDataUpdateShadow(vendorData, (sizeof(vendorData)/sizeof(vendorData[0])), 1);
}

void fileInit()
{
    struct stat st = {0};

    if (stat(CONFIG_FILE_DIR, &st) == -1) {
        mkdir(CONFIG_FILE_DIR, 0777);

        for(int i = 0; i < NUMBER_OF_SLOTS; i += 1){ // Initialize data
            printf("i = %d\n", i);
            vendorData[i].slot_num = i + 1;
            strcpy(vendorData[i].product_name, "none");
            strcpy(vendorData[i].product_price_string, "0.00");
            vendorData[i].product_price = 0;
            vendorData[i].remaining_num = 3;
        }
        saveVendorDataToFile(vendorData, (sizeof(vendorData)/sizeof(vendorData[0])));
    }
    printf("\n%s\n", vendorDataUpdateShadow(vendorData, (sizeof(vendorData)/sizeof(vendorData[0])), 1));

}
