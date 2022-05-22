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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include "hisignalling.h"

#define MSG(args...) printf(args)
pthread_t hisignallingMsgHandleID;
#define HISIGNALLING_HEAD_1 (0xaa)
#define HISIGNALLING_HEAD_2 (0x55)

/**
    @berf GPIO enable of key
    @param pin: gpio port
*/

static int GpioExport(int pin)
{
    char buffer[64] = {0};
    int len = -1;
    int fd = -1;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        MSG("Failed to open export for writing!\n");
        close(fd);
        return -1;
    }

    len = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%d", pin);
    if (len < 0) {
        MSG("printf msg failed\r\n");
    }
    if (write(fd, buffer, len) < 0) {
        MSG("Failed to export gpio!");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
/**
    @berf GPIO disable of key
    @param pin: gpio port
*/
static int GpioUnexport(int pin)
{
    char buffer[64] = {0};
    int len = -1;
    int fd = -1;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) {
        MSG("Failed to open unexport for writing!\n");
        close(fd);
        return -1;
    }

    len = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%d", pin);
    if (len < 0) {
        MSG("printf msg failed\r\n");
    }
    if (write(fd, buffer, len) < 0) {
        MSG("Failed to unexport gpio!");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// dir: 0-->IN, 1-->OUT
static int GpioDirection(int pin, int dir)
{
    static const char dirStr[] = "in\0out";
    char path[64] = {0};
    int fd = -1;

    int len = snprintf_s(path, sizeof(path), sizeof(path) - 1, "/sys/class/gpio/gpio%d/direction", pin);
    if (len < 0) {
        MSG("printf msg failed\r\n");
    }
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        MSG("Failed to open gpio direction for writing!\n");
        close(fd);
        return -1;
    }

    if (write(fd, &dirStr[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0) { /* 3, 2, 3 gpio register */
        MSG("Failed to set direction!\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// value: 0-->LOW, 1-->HIGH
static int GpioWrite(int pin, int value)
{
    static const char valuesStr[] = "01";
    char path[64] = {0};
    int fd = -1;

    int len = snprintf_s(path, sizeof(path), sizeof(path) - 1, "/sys/class/gpio/gpio%d/value", pin);
    if (len < 0) {
        MSG("printf Msg failed\r\n");
    }
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        MSG("Failed to open gpio value for writing!\n");
        close(fd);
        return -1;
    }

    if (write(fd, &valuesStr[value == 0 ? 0 : 1], 1) < 0) { /* 1, 1 gpio register */
        MSG("Failed to write value!\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
/**
    @berf Read GPIO port level change
    @param int pin: gpio port
*/
static int GpioRead(int pin)
{
    char path[64] = {0};
    char value_str[3] = {0};
    int fd = -1;

    int len = snprintf_s(path, sizeof(path), sizeof(path) - 1, "/sys/class/gpio/gpio%d/value", pin);
    if (len < 0) {
        MSG("printf msg failed\r\n");
    }
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        MSG("Failed to open gpio value for reading!\n");
        close(fd);
        return -1;
    }

    if (read(fd, value_str, 3) < 0) { /* 3: gpio register */
        MSG("Failed to read value!\n");
        close(fd);
        return -1;
    }

    close(fd);
    return (atoi(value_str));
}

// none表示引脚为输入，不是中断引脚
// rising表示引脚为中断输入，上升沿触发
// falling表示引脚为中断输入，下降沿触发
// both表示引脚为中断输入，边沿触发
// 0-->none, 1-->rising, 2-->falling, 3-->both
static int GpioEdge(int pin, int edge)
{
    const char dirStr[] = "none\0rising\0falling\0both";
    char ptr = 0;
    char path[64] = {0};
    int fd = -1;

    switch (edge) {
        case EAGE_0:
            ptr = 0;
            break;
        case EAGE_1:
            ptr = 5; /* 5: gpio register */
            break;
        case EAGE_2:
            ptr = 12; /* 12: gpio register */
            break;
        case EAGE_3:
            ptr = 20; /* 20: gpio register */
            break;
        default:
            ptr = 0;
    }
    int len = snprintf_s(path, sizeof(path), sizeof(path) - 1, "/sys/class/gpio/gpio%d/edge", pin);
    if (len < 0) {
        MSG("printf Msg failed\r\n");
    }

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        MSG("Failed to open gpio edge for writing!\n");
        close(fd);
        return -1;
    }

    if (write(fd, &dirStr[ptr], strlen(&dirStr[ptr])) < 0) {
        MSG("Failed to set edge!\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// 初始化按键1
void InitGpio1(void)
{
    MSG("\n =============== InitGpio1 start ========== \n");
    GpioUnexport(1); /* 1: gpio pin */
    GpioExport(1); /* 1: gpio pin */
    GpioDirection(1, 0); /* 1, 0: gpio pin */
    GpioEdge(1, 2); /* 1, 2: gpio pin */
    MSG("\n =============== InitGpio1 end ========== \n");
}

// 初始化按键2
void InitGpio2(void)
{
    MSG("\n =============== InitGpio2 start ========== \n");
    GpioUnexport(2); /* 2: gpio pin */
    GpioExport(2); /* 2: gpio pin */
    GpioDirection(2, 0); /* 2, 0: gpio pin */
    GpioEdge(2, 2); /* 2, 2: gpio pin */
    MSG("\n =============== InitGpio2 end ========== \n");
}

/* 串口设置 */
int Uart1Config(int fd)
{
    struct termios newtio = {0}, oldtio = {0};
    /* 获取原有串口配置 */
    if (tcgetattr(fd, &oldtio) != 0) {
        perror("SetupSerial 1");
        return -1;
    }
    (void)memset_s(&newtio, sizeof(newtio), 0, sizeof(newtio));
    /* CREAD 开启串行数据接收，CLOCAL并打开本地连接模式 */
    newtio.c_cflag  |=  CLOCAL | CREAD;

    /* 设置数据位8 */
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8;
    /* 设置奇偶校验位 */
    newtio.c_cflag &= ~PARENB; // 无奇偶校验
    /* 设置波特率 115200 */
    cfsetispeed(&newtio, B115200);
    cfsetospeed(&newtio, B115200);

    /* 设置停止位 */
    newtio.c_cflag &=  ~CSTOPB; /* 默认为一位停止位 */
    /* 设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时 */
    newtio.c_cc[VTIME]  = 0; /* 非规范模式读取时的超时时间 */
    newtio.c_cc[VMIN] = 0; /* 非规范模式读取时的最小字符数 */
    /* tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */
    tcflush(fd, TCIFLUSH);
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
        perror("com set error");
        return -1;
    }
    return 0;
}

/**
    @berf uart send
    @param int fd: uart file descriptor
    @param void *buf:send data buf
    @param int len:data buf len
*/
int UartSend(int fd, char *buf, int len)
{
    int ret = 0;
    int count = 0;
    char *sendBuf = buf;
    int sendLen = len;

    tcflush(fd, TCIFLUSH);

    while (sendLen > 0) {
        ret = write(fd, (char*)sendBuf + count, sendLen);
        if (ret < 1) {
            printf("write data below 1 byte % d\r\n", ret);
            break;
        }
        count += ret;
        sendLen = sendLen - ret;
    }

    return count;
}
/**
    @berf uart read
    @param int uart_fd: uart file descriptor
    @param void *buf:read data buf
    @param int len:data buf len
    @param int timeoutMs: read data time
*/
int UartRead(int uartFd, char *buf, int len, int timeoutMs)
{
    int ret = 0;
    size_t  rsum = 0;
    ret = 0;
    fd_set rset;
    struct timeval time;
    int timeout = timeoutMs;
    char *readBuf = buf;
    int readLen = len;

    while (rsum < readLen) {
        time.tv_sec = timeout / 1000; /* 1000:转换成秒 */
        time.tv_usec = (timeout - time.tv_sec * 1000) * 1000; /* 1000, 1000:转换为微秒 */
        FD_ZERO(&rset);
        FD_SET(uartFd, &rset);
        ret = select(uartFd + 1, &rset, NULL, NULL, &time);
        if (ret <= 0) {
            if (ret == 0) {
                printf("time over!\r\n");
                return -1;
            }
            if (ret == -1) {
                printf("select error!\r\n");
                // 信号中断
                continue;
            }
            return -1;
        } else {
            ret = read(uartFd, (char *)readBuf + rsum, readLen - rsum);
            if (ret < 0) {
                printf("read data failed\r\n");
                return ret;
            } else {
                rsum += ret;
            }
        }
    }
    return rsum;
}

/*
* crc32 Verification implementation
*/
static const unsigned int crc32table[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
    0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
    0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
    0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
    0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
    0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
    0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
    0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
    0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
    0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
    0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
    0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
    0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
    0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
    0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
    0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
    0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
    0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
    0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
    0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
    0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
    0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
    0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

/**
    @berf CRC check
    @param const unsigned char *buf: Data to be verified buff
    @param unsigned int size: Data to be verified length
*/
static unsigned int crc32(const unsigned char *buf, unsigned int size)
{
    unsigned int  i, crc = 0xFFFFFFFF;

    for (i = 0; i < size; i++) {
        crc = crc32table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8); /* 8: 右移8bit */
    }
    return crc ^ 0xFFFFFFFF;
}

#define RIGHT_MOVE_8_BIT (8)
#define RIGHT_MOVE_16_BIT (16)
#define RIGHT_MOVE_24_BIT (24)
/* hisignal Hi3516 message send */
static unsigned int HisignallingDataPackage(HisignallingProtocalType *buf,
    unsigned int len, unsigned char *hisignallingDataBuf)
{
    unsigned int crcCheckSend = 0;
    unsigned int packageLen = 0;
    unsigned int DataPackLen = len;

    (void)memcpy_s(hisignallingDataBuf, HISGNALLING_MSG_FRAME_HEADER_LEN,
        buf->frameHeader, HISGNALLING_MSG_FRAME_HEADER_LEN);
    (void)memcpy_s(&hisignallingDataBuf[HISGNALLING_MSG_FRAME_HEADER_LEN],
        DataPackLen, buf->hisignallingMsgBuf, DataPackLen);
    (void)memcpy_s(&hisignallingDataBuf[HISGNALLING_MSG_FRAME_HEADER_LEN + DataPackLen],
        HISIGNALLING_MSG_HEADER_LEN, &(buf->endOfFrame), HISIGNALLING_MSG_HEADER_LEN);
    crcCheckSend = crc32(hisignallingDataBuf, (DataPackLen + HISIGNALLING_MSG_HEADER_TAIL_LEN));
    hisignallingDataBuf[DataPackLen + HISIGNALLING_MSG_HEADER_TAIL_LEN] =
        (unsigned char)((crcCheckSend & 0xff000000) >> RIGHT_MOVE_24_BIT);
    hisignallingDataBuf[DataPackLen + HISIGNALLING_MSG_HEADER_TAIL_LEN + 1] = /* 1: addr offset */
        (unsigned char)((crcCheckSend & 0x00ff0000) >> RIGHT_MOVE_16_BIT);
    hisignallingDataBuf[DataPackLen + HISIGNALLING_MSG_HEADER_TAIL_LEN + 2] = /* 2: addr offset */
        (unsigned char)((crcCheckSend & 0x0000ff00) >> RIGHT_MOVE_8_BIT);
    hisignallingDataBuf[DataPackLen + HISIGNALLING_MSG_HEADER_TAIL_LEN + 3] = /* 3: addr offset */
        (unsigned char)crcCheckSend;
    packageLen = DataPackLen + HISIGNALLING_MSG_HEADER_TAIL_LEN + 4; /* 4: crc check lenght */
    return packageLen;
}

/* hisignalling Hi3561 message recevice */
static HisignallingErrorType HisignallingMsgReceive(int fd, unsigned char *buf, unsigned int len)
{
    unsigned int crcCheckReceived = 0;
    int i = 0, readLen = 0;
    unsigned int RecvLen = len;

    /* Hi3516dv300 uart read */
    readLen = UartRead(fd, buf, RecvLen, 1000); /* 1000 :time out */
    if (readLen <= 0) {
        printf("uart_read data failed\r\n");
        return HISIGNALLING_RET_VAL_ERROR;
    }
    printf("read_len=%d\r\n", readLen);
    /* 校验crc */
    if (RecvLen > 4) { /* 4:crc check lenght */
        crcCheckReceived = crc32(buf, RecvLen - 4); /* 4:crc check lenght */
        if (((unsigned char)((crcCheckReceived & 0xff000000) >> RIGHT_MOVE_24_BIT) !=
            buf[RecvLen - 4]) && /* 4: addr offset */
            ((unsigned char)((crcCheckReceived & 0x00ff0000) >> RIGHT_MOVE_16_BIT) !=
            buf[RecvLen - 3]) && /* 3: addr offset */
            ((unsigned char)((crcCheckReceived & 0x0000ff00) >> RIGHT_MOVE_8_BIT)  !=
            buf[RecvLen - 2]) && /* 2: addr offset */
            ((unsigned char)crcCheckReceived != buf[RecvLen - 1])) { /* 1: addr offset */
            printf("<Hi3516dv300>: crc32 Verification failed!\r\n");
            printf("<Hi3516dv300>: crc_4=0x%x, crc_3=0x%x, crc_2=0x%x, crc_1=0x%x\r\n",
                buf[RecvLen - 4], buf[RecvLen - 3], buf[RecvLen - 2], buf[RecvLen - 1]); /* 4, 3, 2, 1: addr offset */
            return HISIGNALLING_RET_VAL_ERROR;
        }
    }
    /* 输出收到的数据 */
    for (i = 0; i < RecvLen; i++) {
        printf("0x%x ", buf[i]);
    }
    printf("\r\n");

    return HISIGNALLING_RET_VAL_CORRECT;
}

/**
    @berf hisignalling protocol send msg
    @param void *buf: send data buff
    @param unsigned int data_len: send data length
*/
static unsigned int HisignallingMsgSend(int fd, char *buf, unsigned int dataLen)
{
    unsigned int ret = 0;
    HisignallingProtocalType hisignallingMsg = {0};
    unsigned char hisignallingSendBuf[HISIGNALLING_MSG_BUFF_LEN] = {0};
    unsigned int hisignallingPackageLen = 0;
    unsigned int writeDataLen = 0;

    hisignallingMsg.frameHeader[0]= 0xAA; /* Protocol head data 1 */
    hisignallingMsg.frameHeader[1]= 0x55; /* Protocol head data 2 */
    (void)memcpy_s(hisignallingMsg.hisignallingMsgBuf, dataLen, buf, dataLen);
    hisignallingMsg.endOfFrame = 0xff; /* Protocol tail data */

    hisignallingPackageLen = HisignallingDataPackage(&hisignallingMsg, dataLen, hisignallingSendBuf);
    if (!hisignallingPackageLen) {
        printf("hisignalling_data_package failed\r\n");
        return -1;
    }
    if (*hisignallingSendBuf == 0) {
        printf("hisignalling send buf is null!\r\n");
        return -1;
    }

    ret = UartSend(fd, hisignallingSendBuf, hisignallingPackageLen);
    if (ret < 0) {
        printf("write data failed\r\n");
        return -1;
    }

    for (int i = 0; i < hisignallingPackageLen; i++) {
        printf("send data = 0x%x \r\n", hisignallingSendBuf[i]);
    }

    return 0;
}

/* hisignalling message handle */
void *HisignallingMsgHandle(char *param)
{
    unsigned int err = 0;

    while (1) {
        printf("hisignalling_msg_handle\r\n");
        usleep(HISGNALLING_FREE_TASK_TIME);
    }
    err = pthread_join(hisignallingMsgHandleID, NULL);
    if (err != 0) {
        printf("Failed to delete hisignalling msg task\r\n");
    }
}

#define HISIGNALLING_TASK_STACK_SIZE (2048)
/* hisignalling message task */
unsigned int HisignallingMsgTask(void)
{
    unsigned int  ret = 0;
    int err = 0, stacksize = HISIGNALLING_TASK_STACK_SIZE;

    pthread_attr_t hisignallingAttr = {0};
    err = pthread_attr_init(&hisignallingAttr);
    err = pthread_attr_setstacksize(&hisignallingAttr, stacksize);
    ret = pthread_create(&hisignallingMsgHandleID, &hisignallingAttr, HisignallingMsgHandle, NULL);
    if (ret != 0) {
        printf("Failed to create hisignalling msg task\r\n");
        return -1;
    }
    return 0;
}

static void UartProcess(int uartFd, int Gpio1Fd, int Gpio2Fd, struct pollfd fdS1, struct pollfd fdS2)
{
    int ret = 0;
    char buff[10] = {0};
    unsigned char writeBuffer[4]  = {0, 2, 0, 3};
    unsigned char writeBuffer2[4] = {0, 2, 0, 4};
    unsigned char writeBuffer3[4] = {0, 2, 0, 5};
    unsigned char readBuff[16] = {0};

    Uart1Config(uartFd);
    while (1) {
        /* 按键操作 */
        ret = read(Gpio1Fd, buff, 10); /* 10:read data lenght */
        if (ret == -1) {
            MSG("gpio1 read error\n");
        }
        ret = poll(&fdS1, 1, 0); /* 1: 监视一个文件描述符 */
        if (ret == -1) {
            MSG("gpio1 poll error\n");
        }
        if (fdS1.revents & POLLPRI) {
            ret = lseek(Gpio1Fd, 0, SEEK_SET);
            if (ret == -1) {
                MSG("gpio1 lseek error\n");
            }
            MSG("sw2 Pressed \n");
            /* 按键触发发送 */
            HisignallingMsgSend(uartFd, writeBuffer2, sizeof(writeBuffer2) / sizeof(writeBuffer2[0]));
        }
        ret = read(Gpio2Fd, buff, 10); /* 10:read data lenght */
        if (ret == -1) {
            MSG("gpio2 read error\n");
        }
        ret = poll(&fdS2, 1, 0); /* 1: 监视一个文件描述符 */
        if (ret == -1) {
            MSG("gpio2 poll error\n");
        }
        if (fdS2.revents & POLLPRI) {
            ret = lseek(Gpio2Fd, 0, SEEK_SET);
            if (ret == -1) {
                MSG("gpio1 lseek error\n");
            }
            MSG("sw1 Pressed \n");
            /* 按键触发发送 */
#ifdef  EXPANSION_BOARD
            HisignallingMsgSend(uartFd, writeBuffer3, sizeof(writeBuffer3) / sizeof(writeBuffer3[0]));
#elif defined (ROBOT_BOARD)
            HisignallingMsgSend(uartFd, writeBuffer, sizeof(writeBuffer) / sizeof(writeBuffer[0]));
#endif
        }
        /* 串口读写操作 */
        HisignallingMsgReceive(uartFd, readBuff, HISIGNALLING_MSG_MOTOR_ENGINE_LEN);
        usleep(HISGNALLING_FREE_TASK_TIME);
    }
}

unsigned int UartOpenInit(void)
{
    int fd;
    char *uart1 = "/dev/ttyAMA1";

    if ((fd = open(uart1, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
        printf("open %s is failed", uart1);
        return -1;
    } else {
        Uart1Config(fd);
    }
    return fd;
}

void UartSendRead(int fd, refuseClassification refuseType)
{
    /* test buffer */
    unsigned char writeBuffer2[4] = {0, 2, 0, 1};
    unsigned char writeBuffer3[4] = {0, 2, 0, 2};
    unsigned char writeBuffer4[4] = {0, 2, 0, 3};
    unsigned char writeBuffer5[4] = {0, 2, 0, 4};
    unsigned char writeBuffer6[4] = {0, 2, 0, 5};
    unsigned char writeBuffer7[4] = {0, 2, 0, 6};
    unsigned char writeBuffer8[4] = {0, 2, 0, 7};
    unsigned char writeBuffer9[4] = {0, 2, 0, 8};
    unsigned char readBuff[16] = {0};

#ifdef  EXPANSION_BOARD
    switch (refuseType) {
        case FistGesture:
            HisignallingMsgSend(fd, writeBuffer2, sizeof(writeBuffer2)/sizeof(writeBuffer2[0]));
            printf("send gesture status:FistGesture\r\n");
            break;
        case ForefingerGesture:
            HisignallingMsgSend(fd, writeBuffer3, sizeof(writeBuffer3)/sizeof(writeBuffer3[0]));
            printf("send gesture status:ForefingerGesture\r\n");
            break;
        case OkGesture:
            HisignallingMsgSend(fd, writeBuffer4, sizeof(writeBuffer4)/sizeof(writeBuffer4[0]));
            printf("send gesture status:OkGesture\r\n");
            break;
        case PalmGesture:
            HisignallingMsgSend(fd, writeBuffer5, sizeof(writeBuffer5)/sizeof(writeBuffer5[0]));
            printf("send gesture status:PalmGesture\r\n");
            break;
        case YesGesture:
            HisignallingMsgSend(fd, writeBuffer6, sizeof(writeBuffer6)/sizeof(writeBuffer6[0]));
            printf("send gesture status:YesGesture\r\n");
            break;
        case ForefingerAndThumbGesture:
            HisignallingMsgSend(fd, writeBuffer7, sizeof(writeBuffer7)/sizeof(writeBuffer7[0]));
            printf("send gesture status:ForefingerAndThumbGesture\r\n");
            break;
        case LittleFingerAndThumbGesture:
            HisignallingMsgSend(fd, writeBuffer8, sizeof(writeBuffer8)/sizeof(writeBuffer8[0]));
            printf("send gesture status:LittleFingerAndThumbGesture\r\n");
            break;
        case InvalidGesture:
            HisignallingMsgSend(fd, writeBuffer9, sizeof(writeBuffer9)/sizeof(writeBuffer9[0]));
            printf("send gesture status:InvalidGesture\r\n");
            break;
    }
#endif
    /* 串口读操作 */
    if (readBuff[0] == HISIGNALLING_HEAD_1 && readBuff[1] == HISIGNALLING_HEAD_2) {
        HisignallingMsgReceive(fd, readBuff, HISIGNALLING_MSG_MOTOR_ENGINE_LEN);
    }
}

int AiUartTransmit(void)
{
    /* 按键初始化定义 */
    int gpio1Fd = -1;
    int gpio2Fd = -1;
    int ret1     = -1;
    int ret2     = -1;
    struct pollfd fds1;
    struct pollfd fds2;
    /* 串口初始化定义 */
    int fd = 0;
    char *uart1 = "/dev/ttyAMA1";

    printf("hisignal task start\r\n");
    /* 按键操作 */
    InitGpio1();
    InitGpio2();
    gpio1Fd = open("/sys/class/gpio/gpio1/value", O_RDONLY);
    if (gpio1Fd < 0) {
        MSG("Failed to open gpio1 !\n");
        return -1;
    }
    fds1.fd     = gpio1Fd;
    fds1.events = POLLPRI;

    gpio2Fd = open("/sys/class/gpio/gpio2/value", O_RDONLY);
    if (gpio2Fd < 0) {
        MSG("Failed to open gpio1 !\n");
        return -1;
    }
    fds2.fd     = gpio2Fd;
    fds2.events = POLLPRI;
    /* 串口读写 */
    if ((fd = open(uart1, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
        printf("open %s is failed", uart1);
        return -1;
    } else {
        UartProcess(fd, gpio1Fd, gpio2Fd, fds1, fds2);
    }
    return 0;
}
