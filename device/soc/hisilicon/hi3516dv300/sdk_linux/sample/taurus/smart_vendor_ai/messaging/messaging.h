#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MESSAGING_H
#define MESSAGING_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct SlotInfo
{
    int slot_num;
    char* product_name;
    double product_price;
    char* product_price_string;
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

int printCJSONVersion(void);
void create_objects(void);
char* slotSelectionToJson(SlotSelection selectedSlot);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MEDIA_AI_H__ */
