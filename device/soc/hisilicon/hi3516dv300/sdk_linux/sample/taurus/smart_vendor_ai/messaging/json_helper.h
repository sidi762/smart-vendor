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
