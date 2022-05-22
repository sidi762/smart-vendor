# 运行第一个程序hello world<a name="ZH-CN_TOPIC_0000001130176841"></a>
## 硬件环境搭建
-    硬件要求：Hi3861V100核心板、扩展板、OLED板；硬件搭建如下图所示。
-    [Hi3861V100核心板参考：HiSpark_WiFi_IoT智能开发套件_原理图硬件资料\原理图\HiSpark_WiFi-IoT_Hi3861_CH340G_VER.B.pdf](http://gitee.com/hihope_iot/embedded-race-hisilicon-track-2022/blob/master/%E7%A1%AC%E4%BB%B6%E8%B5%84%E6%96%99/HiSpark_WiFi_IoT%E6%99%BA%E8%83%BD%E5%AE%B6%E5%B1%85%E5%BC%80%E5%8F%91%E5%A5%97%E4%BB%B6_%E5%8E%9F%E7%90%86%E5%9B%BE.rar)
-    [扩展板参考：HiSpark_WiFi_IoT智能开发套件_原理图硬件资料\原理图\HiSpark_WiFi-IoT_Hi3861_CH340G_VER.B.pdf](http://gitee.com/hihope_iot/embedded-race-hisilicon-track-2022/blob/master/%E7%A1%AC%E4%BB%B6%E8%B5%84%E6%96%99/HiSpark_WiFi_IoT%E6%99%BA%E8%83%BD%E5%AE%B6%E5%B1%85%E5%BC%80%E5%8F%91%E5%A5%97%E4%BB%B6_%E5%8E%9F%E7%90%86%E5%9B%BE.rar)
-    [OLED板参考：HiSpark_WiFi_IoT智能开发套件_原理图硬件资料\原理图\HiSpark_WiFi_IoT_OLED_VER.A.pdf](http://gitee.com/hihope_iot/embedded-race-hisilicon-track-2022/blob/master/%E7%A1%AC%E4%BB%B6%E8%B5%84%E6%96%99/HiSpark_WiFi_IoT%E6%99%BA%E8%83%BD%E5%AE%B6%E5%B1%85%E5%BC%80%E5%8F%91%E5%A5%97%E4%BB%B6_%E5%8E%9F%E7%90%86%E5%9B%BE.rar)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%87107.png)

## 软件介绍
-   1.代码目录结构及相应接口功能介绍,如下图所示：
```
vendor_hisilicon/hispark_pegasus/demo/hello_world_demo
├── BUILD.gn            # BUILD.gn文件由三部分内容（目标、源文件、头文件路径）构成,开发者根据需要填写,
|                       # sources中指定静态库.a所依赖的.c文件及其路径，若路径中包含"//"则表示绝对路径（此处为代码根路径），若不包含"//"则表示相对路
|                       # 径。include_dirs中指定source所需要依赖的.h文件路径。
├── hal_iot_gpio_ex.c   # 
├── iot_gpio_ex.h       # 
├── led_example.c       # LedExampleEntry():gpio9控制led灯闪烁;SYS_RUN(LedExampleEntry):启动恢复业务模块接口,定义ohos_init.h文件中。
├── oled_demo.c         # OledmentTask():启动OLED任务;OledDemo():实现点亮led屏幕;APP_FEATURE_INIT():。
├── oled_fonts.h        # g_f6X8[][6]:定义6*8矩阵;g_f8X16[]:定义8*16矩阵。
├── oled_ssd1306.c      # IotI2cData:定义接收,发送数据以及长度;OledInit():OLED初始化。
└── oled_ssd1306.h      # 
```
-  2.工程编译
    -    将源码./vendor_hisilicon/hispark_pegasus/demo目录下的hello_world_demo整个文件夹及内容复制到源码./applications/sample/wifi-iot/app/下。
    ```
    .
    └── applications
        └── sample
            └── wifi-iot
                └── app
                    └──hello_world_demo
                       └── 代码   
    ```
   
    -    修改源码./applications/sample/wifi-iot/app/BUILD.gn文件，在features字段中增加索引，使目标模块参与编译。features字段指定业务模块的路径和目标，以helloWorld举例，features字段配置如下。
    ```
    import("//build/lite/config/component/lite_component.gni")
    
    lite_component("app") {
        features = [
            "hello_world_demo:helloWorld",
        ]
    }
    ```
    -    修改.device/soc/hisilicon/hi3861v100/sdk_liteos/build/config/usr_config.mk文件。在这个配置文件中打开I2C,PWM驱动宏。搜索字段CONFIG_I2C_SUPPORT ，并打开I2C,PWM。配置如下：
    ```
    # CONFIG_I2C_SUPPORT is not set
    CONFIG_I2C_SUPPORT=y
    # CONFIG_PWM_SUPPORT is not set
    CONFIG_PWM_SUPPORT=y
    ```        
    -    工程相关配置完成后,然后编译。
-   3.烧录
    -    编译成功后，点击DevEco Home->配置工程->hi3861->upload_port->选择对应串口端口号->选择upload_protocol->选择burn-serial->点击save保存，在保存成功后点击upload进行烧录，出现connecting, please reset device..字样复位开发板等待烧录完成。
    -    烧录成功后，再次点击Hi3861核心板上的“RST”复位键，此时开发板的系统会运行起来。运行结果：在OLED屏正中央(第3行，5列开始)，显示一个“Hello World”，主板上的LED灯间隔1s交替闪烁。

    ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%87106.png)
