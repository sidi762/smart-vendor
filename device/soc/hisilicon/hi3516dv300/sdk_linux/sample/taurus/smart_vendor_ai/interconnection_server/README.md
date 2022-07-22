# Pegasus与Taurus串口互联通信<a name="ZH-CN_TOPIC_0000001130176841"></a>
-注意：在使用串口互联前，请确认手势检测+手势识别可以正常运行。

## 硬件环境搭建
-    硬件要求：Taurus开发板；硬件搭建如下图所示。注意这里需要跟Pegasus同时使用，详情可以参考[串口互联client端](http://gitee.com/openharmony/vendor_hisilicon/blob/master/hispark_pegasus/demo/interconnection_client_demo/README.md),由于Pegasus有两种方式，下面介绍两种硬件搭建方式。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/10.jpg)

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
-    这里以手势识别为例：1.hand_classify.c文件中在初始化uart
     ```
     uartFd = UartOpenInit();
     if (uartFd < 0) {
       printf("uart1 open failed\r\n");
     } else {
       printf("uart1 open successed\r\n");
     }
     return ret;
     ```
-    2.在HandDetectFlag调用UartSendRead()函数实现数据的发送，Pegasus接收到数据后实现对应外设的响应。
-    3.当Taurus的摄像头检测到特定的手势之后，Taurus会将对应的检测结果通过串口发送给Pegasus端，此时Pegasus主板上的灯会亮起，具体实验结果及打印信息如下图所示。

     ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/6.png)

     ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/7.png)

     ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E4%BA%92%E8%81%94/8.jpg)