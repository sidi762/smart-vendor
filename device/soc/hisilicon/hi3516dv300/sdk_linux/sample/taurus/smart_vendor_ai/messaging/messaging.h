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

/*----------------------------------------------------------------
@brief Send data to 3618 via UART
@param int fd: UART file descriptor
@param unsigned char *payload: The data to send
----------------------------------------------------------------*/
void messageUARTSendData(int fd, unsigned char *payload);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MEDIA_AI_H__ */
