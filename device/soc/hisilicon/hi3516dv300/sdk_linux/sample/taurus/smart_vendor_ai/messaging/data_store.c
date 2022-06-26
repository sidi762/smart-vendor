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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cJSON.h"
#include "messaging.h"
#include "json_helper.h"
#include "data_store.h"

#define NUMBER_OF_SLOTS 4

SlotInfo vendorData[NUMBER_OF_SLOTS] = {0};

void fileInit()
{
    struct stat st = {0};

    if (stat("/userdata/vendor_data", &st) == -1) {
        mkdir("/userdata/vendor_data", 0777);

        for(int i = 0; i < NUMBER_OF_SLOTS; i += 1){ // Initialize data
            printf("i = %d\n", i);
            vendorData[i].slot_num = i + 1;
            strcpy(vendorData[i].product_name, "none");
            strcpy(vendorData[i].product_price_string, "0.00");
            vendorData[i].product_price = 0;
            vendorData[i].remaining_num = 3;
        }
        char* jsonString = vendorDataToJson(vendorData, (sizeof(vendorData)/sizeof(vendorData[0])));
        /*
        int lengthCount = 0;
        while(*(jsonString+lengthCount) != '\0'){
            printf("%c", *(jsonString+lengthCount));
            lengthCount += 1;
        }
        */
        FILE *fp = NULL;
        fp = fopen("/userdata/vendor_data/vendor.json", "w+");
        fputs(jsonString, fp);

        fclose(fp);
    }

}