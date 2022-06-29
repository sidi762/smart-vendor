/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

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
#include "json_helper.h"
#include "messaging.h"

#define NUMBER_OF_SLOTS 4

/* Create a bunch of objects as demonstration. */
int print_preallocated(cJSON *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("cJSON_Print result:\n%s\n", out);
            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    /* success */
    printf("%s\n", buf);

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("cJSON_Print result:\n%s\n", out);
        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}

/* Create a bunch of objects as demonstration. */
void create_objects(void)
{
    /* declare a few. */
    cJSON *root = NULL;
    cJSON *fld = NULL;
    int i = 0;

    /* Our array of "records": */
    SlotInfo testSlot;
    testSlot.slot_num = 0;
    //testSlot.product_name = "test";
    testSlot.product_price = 10;
    //testSlot.product_price_string = "10.00";
    testSlot.remaining_num = 10;

    /* Here we construct some JSON standards, from the JSON site. */



    /* Our array of "records": */
    root = cJSON_CreateArray();
    cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
    cJSON_AddNumberToObject(fld, "slot_num", testSlot.slot_num);
    cJSON_AddNumberToObject(fld, "product_price", testSlot.product_price);
    cJSON_AddNumberToObject(fld, "remaining_num", testSlot.remaining_num);
    //cJSON_AddStringToObject(fld, "product_name", testSlot.product_name);
    //cJSON_AddStringToObject(fld, "product_price_string", testSlot.product_price_string);

    /* cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root, 1), "City", cJSON_CreateIntArray(ids, 4)); */

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }

}


int printCJSONVersion(void)
{
    /* print the version */
    printf("Version: %s\n", cJSON_Version());
    return 0;
}

char* SlotSelectionToJson(SlotSelection selectedSlot)
{
    cJSON *root = cJSON_CreateObject();
    char* ret;
    cJSON_AddNumberToObject(root, "slot_num", selectedSlot.slot_num);
    ret = cJSON_Print(root);
    cJSON_Delete(root);
    return ret;
}

char* UIControlToJson(UIControl UIController)
{
    cJSON *root = cJSON_CreateObject();
    char* ret;
    cJSON_AddNumberToObject(root, "ui_page_num", UIController.ui_page_num);
    ret = cJSON_Print(root);
    cJSON_Delete(root);
    return ret;
}

char* vendorDataToJson(SlotInfo items[], int len)
{
    cJSON *root = NULL;
    cJSON *fld = NULL;
    char *ret = NULL;
    cJSON *productsArray = NULL;

    root = cJSON_CreateObject();
    productsArray = cJSON_AddArrayToObject(root, "products");
    for(int i = 0; i < len; i += 1){
        cJSON_AddItemToArray(productsArray, fld = cJSON_CreateObject());
        cJSON_AddNumberToObject(fld, "slot_num", items[i].slot_num);
        cJSON_AddNumberToObject(fld, "product_price", items[i].product_price);
        cJSON_AddNumberToObject(fld, "remaining_num", items[i].remaining_num);
        cJSON_AddStringToObject(fld, "product_name", items[i].product_name);
        cJSON_AddStringToObject(fld, "product_price_string", items[i].product_price_string);
    }
    ret = cJSON_Print(root);
    cJSON_Delete(root);
    return ret;
}

int jsonToVendorData(char* jsonString, SlotInfo vendorDataPtr[])
{
    const cJSON *item = NULL;
    const cJSON *items = NULL;
    const cJSON *slotNumJson = NULL;
    const cJSON *productPriceJson = NULL;
    const cJSON *remainingNumJson = NULL;
    const cJSON *productNameJson = NULL;
    const cJSON *productPriceStringJson = NULL;

    SlotInfo ret[NUMBER_OF_SLOTS] = {0};

    cJSON *vendorDataJson = cJSON_Parse(jsonString);
    if (vendorDataJson == NULL){
       const char *error_ptr = cJSON_GetErrorPtr();
       if (error_ptr != NULL){
           fprintf(stderr, "Error before: %s\n", error_ptr);
           return 1;
       }
    }

    items = cJSON_GetObjectItemCaseSensitive(vendorDataJson, "products");
    int count = 0;
    cJSON_ArrayForEach(item, items){
        if(count < NUMBER_OF_SLOTS){
            slotNumJson = cJSON_GetObjectItemCaseSensitive(item, "slot_num");
            if (cJSON_IsNumber(slotNumJson)){
                ret[count].slot_num = slotNumJson->valueint;
            }
            productPriceJson = cJSON_GetObjectItemCaseSensitive(item, "product_price");
            if (cJSON_IsNumber(productPriceJson)){
                ret[count].product_price = productPriceJson->valueint;
            }
            remainingNumJson = cJSON_GetObjectItemCaseSensitive(item, "remaining_num");
            if (cJSON_IsNumber(remainingNumJson)){
                ret[count].product_price = remainingNumJson->valueint;
            }
            productNameJson = cJSON_GetObjectItemCaseSensitive(item, "product_name");
            if (cJSON_IsString(productNameJson) && (productNameJson->valuestring != NULL)){
                strcpy(ret[count].product_name, productNameJson->valuestring);
            }
            productPriceStringJson = cJSON_GetObjectItemCaseSensitive(item, "product_price_string");
            if (cJSON_IsString(productPriceStringJson) && (productPriceStringJson->valuestring != NULL)){
                strcpy(ret[count].product_price_string, productPriceStringJson->valuestring);
            }
            count += 1;
        }else{
            printf("Error: items in json is more than number of slots\n");
            return 1;
        }
    }

    vendorDataPtr = ret;
    cJSON_Delete(vendorDataJson);
    return 0;
}

char* vendorDataUpdateShadow(SlotInfo items[], int len, int version)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *fld = NULL;
    cJSON *arr = NULL;
    cJSON *state = NULL;
    cJSON *reported = NULL;
    char* ret = NULL;
    cJSON_AddStringToObject(root, "type", "update");
    cJSON_AddObjectToObject(root, state = cJSON_CreateObject());
    cJSON_AddObjectToObject(state, reported = cJSON_CreateObject());
    cJSON_AddArrayToObject(reported, arr = cJSON_CreateArray());
    for(int i = 0; i < len; i += 1){
        cJSON_AddItemToArray(arr, fld = cJSON_CreateObject());
        cJSON_AddNumberToObject(fld, "slot_num", items[i].slot_num);
        cJSON_AddNumberToObject(fld, "product_price", items[i].product_price);
        cJSON_AddNumberToObject(fld, "remaining_num", items[i].remaining_num);
        cJSON_AddStringToObject(fld, "product_name", items[i].product_name);
        cJSON_AddStringToObject(fld, "product_price_string", items[i].product_price_string);
    }
    cJSON_AddNumberToObject(root, "version", version);
    cJSON_AddStringToObject(root, "clientToken", "");
    ret = cJSON_Print(root);
    cJSON_Delete(root);
    return ret;
}
