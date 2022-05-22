# I2C实验笔记
## 调用相关功能固定头文件和.c文件
### 调用GPIO
hal_iot_gpio_ex.c, iot_gpio_ex.h

### OLED驱动（可找到调用I2C示例）
oled_ssd1306.c, oled_ssd1306.h

### OLED 字体
oled_fonts.h

### 建立新线程执行任务
详见I2Cexp.c

`static void OLEDDemo(void)
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
SYS_RUN(OLEDDemo);`
