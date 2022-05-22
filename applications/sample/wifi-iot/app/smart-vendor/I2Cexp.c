#include <stddef.h>
#include <stdio.h>
#include "ohos_init.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_gpio_ex.h"
#include "oled_fonts.h"
#include "oled_ssd1306.h"
#include "hi_gpio.h"
#include "hi_io.h"
#include "hi_task.h"
#include "hi_types_base.h"
#include <unistd.h>
#include "cmsis_os2.h"


#define OLED_I2C_IDX 0

#define OLED_WIDTH    (128)
#define OLED_I2C_ADDR 0x78 // 默认地址为 0x78
#define OLED_I2C_CMD 0x00 // 0000 0000       写命令
#define OLED_I2C_DATA 0x40 // 0100 0000(0x40) 写数据
#define OLED_I2C_BAUDRATE (400 * 1000) // 400k

#define DELAY_100_MS (100 * 1000)
#define AHT20_BAUDRATE (400 * 1000)
#define AHT20_I2C_IDX 0

static void OledDemoTask(const char *arg)
{
    (void)arg;
    OledInit();
    OledFillScreen(0);
    IoTI2cInit(AHT20_I2C_IDX, AHT20_BAUDRATE);

    OledShowString(20, 3, "Hello smart vendor", 1); /* 屏幕第20列3行显示1行 */
}


static void OLEDDemo(void)
{
    osThreadAttr_t attr;
    attr.name = "UartDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096; /* 任务大小4096 */
    attr.priority = osPriorityNormal;
    if (osThreadNew(OledDemoTask, NULL, &attr) == NULL) {
        printf("[OledDemo] Falied to create OLEDDemoTask!\n");
    }
}
SYS_RUN(OLEDDemo);