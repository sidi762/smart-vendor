# Pegasus与Taurus串口互联通信<a name="ZH-CN_TOPIC_0000001130176841"></a>

## 硬件环境搭建
-    硬件要求：Hi3861V100核心板、底板、外设扩展板或者机器人板；硬件搭建如下图所示。注意这里需要跟Taurus同时使用，详情可以参考[串口互联server](http://gitee.com/openharmony/device_soc_hisilicon/blob/master/hi3516dv300/sdk_linux/sample/taurus/ai_sample/interconnection_server/README.md)端。
-    外设扩展板使用的是Hi3861的GPIO0和GPIO1作为串口1复用功能，请在程序初始化时注意管脚复用关系。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/10.jpg)

-    注意：Robot板使用的串口1复用端口是Hi3861的GPIO5和GPIO6，其中GPIO6要在程序烧录启动后才能使用，如果用户在还没烧录和启动之前就将串口线与硬件连接上，这时Hi3861将无法烧录和重启。GPIO6（TX）引脚影响Hi3861烧录和启动，用户在使用Robot板时，先拔掉Robot上4pin串口连接线，在程序烧录启动后，再将4pin串口连接线插回Robot板，此时串口可以正常通信。如果用户在使用Robot板的过程中觉得频繁插拔串口线麻烦，用户可在串口线上做一个开关，当Hi3861烧录或复位启动前关闭开关，单板启动后打开开关。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/11.jpg)

## 串口通信控制协议HiSignalling介绍
-    为了便于Taurus与Pegasus开发套件之间进行通信和控制，定义了一套简易的HiSignalling通信控制协议，数据帧格式如下表所示，并提供相关参考代码，大家也可以根据自己的需要使用其他协议。

| 帧头（2Byte）  | Payload Len (2Byte)  | payload  | 帧尾（1Byte）  | CRC32(4Byte)  |
|---|---|---|---|---|
| 0xAA,0x55  |   |   | 0xFF  | CRC32 |
			
例如一组数据帧为：AA5500020003FF8ED2BEDF (十六进制不区分大小写)
-    0AA55:       帧头
-    0002：       Payload Len
-    0003:        Payload
-    FF:          帧尾
-    8ED2BEDF:    CRC32校验码

## 软件介绍
-   注意这里需要跟Taurus同时使用，Taurus软件介绍详情可以参考串口互联server端。
-   1.代码目录结构及相应接口功能介绍
- UART API

| API                                                          | 接口说明                 |
| ------------------------------------------------------------ | ------------------------ |
| unsigned int UartInit(WifiIotUartIdx id, const WifiIotUartAttribute *param, const WifiIotUartExtraAttr *extraAttr); | 初始化，配置一个UART设备 |
| int UartRead(WifiIotUartIdx id, unsigned char *data, unsigned int dataLen) | 从UART设备中读取数据     |
| int UartWrite(WifiIotUartIdx id, const unsigned char *data, unsigned int dataLen) | 将数据写入UART设备       |

-   2.工程编译
    -   将源码./vendor/hisilicon/hispark_pegasus/demo目录下的interconnection_client_demo整个文件夹及内容复制到源码./applications/sample/wifi-iot/app/下，如图。
    ```
    .
    └── applications
        └── sample
            └── wifi-iot
                └── app
                    └──interconnection_client_demo
                       └── 代码
    ```

    -   在hisignalling.h文件中，如果是想使用硬件扩展板，请将BOARD_SELECT_IS_EXPANSION_BOARD这个宏打开，如果是想使用Robot板，请将BOARD_SELECT_IS_ROBOT_BOARD 这个宏打开。
    ```
    /**
    * @brief Adapter plate selection
    * 使用时选择打开宏，使用外设扩展板打开#define BOARD_SELECT_IS_EXPANSION_BOARD这个宏
    * 使用Robot板打开#define BOARD_SELECT_IS_ROBOT_BOARD这个宏
    **/
    //#define BOARD_SELECT_IS_ROBOT_BOARD
    #define BOARD_SELECT_IS_EXPANSION_BOARD
    #ifdef BOARD_SELECT_IS_EXPANSION_BOARD
    #define EXPANSION_BOARD
    #else
    #define ROBOT_BOARD
    #endif
    ```

    -   修改源码./applications/sample/wifi-iot/app/BUILD.gn文件，在features字段中增加索引，使目标模块参与编译。features字段指定业务模块的路径和目标,features字段配置如下。
    ```
    import("//build/lite/config/component/lite_component.gni")
    
    lite_component("app") {
        features = [
            "interconnection_client_demo:interconnectionClientDemo",
        ]
    }
    ```

    -   工程相关配置完成后,然后编译。
-   3.烧录
    -   编译成功后，点击DevEco Home->配置工程->hi3861->upload_port->选择对应串口端口号->选择upload_protocol->选择hiburn-serial->点击save保存，在保存成功后点击upload进行烧录，出现connecting, please reset device..字样复位开发板等待烧录完成。
    -   烧录成功后，再次点击Hi3861核心板上的“RST”复位键，此时开发板的系统会运行起来。运行结果:打开串口工具，可以看到如下打印,同时3861主板灯闪亮一下。

    ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/6.png)

    ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/7.png)

    ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/8.jpg)