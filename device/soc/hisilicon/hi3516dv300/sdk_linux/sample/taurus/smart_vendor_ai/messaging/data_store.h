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
#include "json_helper.h"

#ifndef DATA_STORE_H
#define DATA_STORE_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
void fileInit();


/*----------------------------------------------------------------
@brief Save vendor data to a json file
@param SlotInfo items[]: the array of SlotInfo object to be saved
@param int len: the length of the array
----------------------------------------------------------------*/
void saveVendorDataToFile(SlotInfo vendorData[], int len);

int remainingNumUpdate(int slotNum);

char* memVendorDataToShadow();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MEDIA_AI_H__ */
