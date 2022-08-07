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

#ifndef JSON_HELPER_H
#define JSON_HELPER_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct SlotInfo
{
    int slot_num;
    char product_name[32];
    double product_price;
    char product_price_string[10];
    int remaining_num;
    char imgPath[128];

} SlotInfo;

typedef struct UIControl
{
    int ui_page_num;

} UIControl;


typedef struct SlotSelection
{
    int slot_num;

} SlotSelection;

/*----------------------------------------------------------------
@brief print the version of CJSON library
@return 0 on success
----------------------------------------------------------------*/
int printCJSONVersion(void);

/*----------------------------------------------------------------
@brief create several CJSON objects for testing
----------------------------------------------------------------*/
void create_objects(void);

/*----------------------------------------------------------------
@brief Convert SlotSelection object (see json_helper.h) to JSON string
@param SlotSelection selectedSlot: the SlotSelection object to be converted
@return The string containing JSON representation of selectedSlot
----------------------------------------------------------------*/
char* SlotSelectionToJson(SlotSelection selectedSlot);

/*----------------------------------------------------------------
@brief Convert UIControl object (see json_helper.h) to JSON string
@param UIControl UIController: the SlotSelection object to be converted
@return The string containing JSON representation of UIController
----------------------------------------------------------------*/
char* UIControlToJson(UIControl UIController);

/*----------------------------------------------------------------
@brief Convert a array of SlotInfo object (see json_helper.h) to JSON string
@param SlotInfo items[]: the array of SlotInfo object to be converted
@param int len: the length of the array
@return The string containing JSON representation of items[]
----------------------------------------------------------------*/
char* vendorDataToJson(SlotInfo items[], int len);

/*----------------------------------------------------------------
@brief Convert a JSON string and save the result into vendorDataPtr[]
@param char* jsonString: the JSON string to be converted
@param SlotInfo vendorDataPtr[]: the result SlotInfo (see json_helper.h)
@return 0 if successful, 1 if unsuccessful
----------------------------------------------------------------*/
int jsonToVendorData(char* jsonString, SlotInfo vendorDataPtr[]);


/*----------------------------------------------------------------
@brief Convert a array of SlotInfo object (see json_helper.h) to JSON string for updating device shadow
@note clientID is hard coded, should be improved in the future
@param SlotInfo items[]: the array of SlotInfo object to be converted
@param int len: the length of the array
@return The string containing the message for updating device shadow
----------------------------------------------------------------*/
char* vendorDataUpdateShadow(SlotInfo items[], int len, int version);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MEDIA_AI_H__ */
