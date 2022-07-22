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
