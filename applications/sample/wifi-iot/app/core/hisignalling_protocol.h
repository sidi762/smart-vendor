/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HISIGNALLING_PROTOCOL_H
#define HISIGNALLING_PROTOCOL_H
#include <hi_types_base.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
/**
* @brief : hisignalling Macro definition
*/
#define HISIGNALLING_MSG_HEADER_LEN         (1)
#define HISGNALLING_MSG_FRAME_HEADER_LEN    (2)
#define HISIGNALLING_MSG_HEADER_TAIL_LEN    (3)
#define HISGNALLING_MSG_CRC32_LEN           (4)
#define HISIGNALLING_MSG_PAYLOAD_INDEX      (5)
#define HISGNALLING_FREE_TASK_TIME          (20)
#define HISIGNALLING_MSG_TASK_PRIO          (28)
#define HISIGNALLING_UART_MSG_LEN           (32)
#define HISIGNALLING_MSG_BUFF_LEN           (512)
#define HISIGNALLING_MSG_TASK_STACK_SIZE    (4096)

/* hisgnalling protocol frame header and tail */
#define HISIGNALLING_MSG_FRAME_HEADER_1     ((hi_u8)0xAA)
#define HISIGNALLING_MSG_FRAME_HEADER_2     ((hi_u8)0x55)
#define HISIGNALLING_MSG_FRAME_TAIL         ((hi_u8)0xFF)
/**
* @brief Adapter plate selection
* 使用时选择打开宏，使用外设扩展板打开#define BOARD_SELECT_IS_EXPANSION_BOARD这个宏
* 使用Robot板自己定义#define BOARD_SELECT_IS_ROBOT_BOARD这个宏，注释掉#define BOARD_SELECT_IS_EXPANSION_BOARD
**/

#define BOARD_SELECT_IS_EXPANSION_BOARD
#ifdef BOARD_SELECT_IS_EXPANSION_BOARD
#define EXPANSION_BOARD
#else
#define ROBOT_BOARD
#endif
/**
 * @brief:this defines for the log module, and  HISIGNALING_LOG_TRACE/HISIGNALING_LOG_DEBUG...
 * will not participate the compile in the release version
 * */
typedef enum {
    HISIGNALLING_LEVEL_TRACE = 0,
    HISIGNALLING_LEVEL_DEBUG,
    HISIGNALLING_LEVEL_INFO,
    HISIGNALLING_LEVEL_WARN,
    HISIGNALLING_LEVEL_ERROR,
    HISIGNALLING_LEVEL_FATAL,
    HISIGNALLING_LEVEL_MAX
} HisignallingLogType;
/**
 * @brief: use this function to get the hisignalling log level name
 *
 * @parameter[in]:level, the level to get
 *
 * @return: the mapped level name
 * */
const char *HisignallingLevelNum (HisignallingLogType hisignallingLevel);
/**
 * @brief: this is a weak function ,and you could rewrite one
 *
 * @param fmt: same use as the fmt for printf
 *
 * @param unfixed: same use for printf
 *
 * @return: don't care about it
 *
 * @attention: and the components should not call this function directly, you'd better
 *
 *             call HISIGNALING_LOG groups
 *
 * */
#define HISIGNALLING_PRINT(fmt, ...) \
    do \
    { \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG(level, fmt, ...) \
    do \
    { \
        HISIGNALLING_PRINT("<%s>, <%s>, <%d> "fmt" \r\n", \
        HisignallingLevelNum((level)), __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG_TRACE(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_TRACE, fmt, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG_DEBUG(fmt, ...)   \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_DEBUG, fmt, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG_INFO(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG_WARN(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_WARN, fmt, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG_ERROR(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_ERROR, fmt, ##__VA_ARGS__); \
    } while (0)

#define HISIGNALLING_LOG_FATAL(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_FATAL, fmt, ##__VA_ARGS__); \
    } while (0)
/**
 * @brief: use this hisignalling Transmission protocol frame format
 *
 * @param frameHeader: Transmission protocol frame header
 *
 * @param hisignallingMsgBuf: Transmission protocol frame buffer
 *
 * @param hisigallingMsgLen: Transmission protocol frame buffer len
 *
 * @param endOfFrame: Transmission protocol frame tail
 *
 * @param hisignallingCrc32Check: Transmission protocol crc32 check
 *
 * */
typedef struct {
    hi_u8 frameHeader[HISGNALLING_MSG_FRAME_HEADER_LEN];
    hi_u8 hisignallingMsgBuf[HISIGNALLING_MSG_BUFF_LEN];
    hi_u32 hisigallingMsgLen;
    hi_u8 endOfFrame;
    hi_u32 hisignallingCrc32Check;
}HisignallingProtocalType;
/**
 * @brief: use this hisignalling return type
 *
 * @param HISIGNALLING_RET_VAL_CORRECT: return type is correct
 *
 * @param HISIGNALLING_RET_VAL_ERROR: return type is error
 *
 * @param HISIGNALLING_RET_VAL_ERROR: return type is  unknown type
 * */
typedef enum {
    HISIGNALLING_RET_VAL_CORRECT = 0,
    HISIGNALLING_RET_VAL_ERROR,
    HISGNALLING_RET_VAL_MAX
}HisignallingErrorType;
/**
 * @brief: use this hisignalling sample:Access peripheral enumeration type
 *
 * @param MOTOR_GO_FORWARD: peripheral motor go forward
 *
 * @param MOTOR_GO_BACK: peripheral motor go back
 *
 * @param STEERING_ENGINE_MOVEMENT/STEERING_ENGINE_AND_MOTOR_MOVEMENT/STEERING_ENGINE_AND_MOTOR_STOP
 * the same use for peripheral
 * */
typedef enum {
    MOTOR_GO_FORWARD = 0,
    MOTOR_GO_BACK,
    STEERING_ENGINE_MOVEMENT,
    STEERING_ENGINE_AND_MOTOR_MOVEMENT,
    STEERING_ENGINE_AND_MOTOR_STOP,
    TRASH_CAN_LID_OPEN,
    TRASH_CAN_LID_COLSE
}HisignallingDataType;
/**
* @brief:hisignalling protocal Function declaration
**/
hi_u32 hisignallingMsgTask(hi_void);
/**
* @brief:hisignalling uart message receive API
* @param buf: uart receive buffer
* @param len: uart receive buffer len
*/
HisignallingErrorType hisignallingMsgReceive(hi_u8 *buf, hi_u32 len);
/**
* @brief:hisignalling uart send API
* @param buf: uart message send buffer
* @param len: uart message send buffer len
*/
hi_u32 hisignallingMsgSend(hi_void *buf, hi_u32 dataLen);
int SetUartReceiveFlag(void);
#endif /* HISIGNALING_PROTOCOL_H */