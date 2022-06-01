# 腾讯云的微信小程序开发及Hi3861板端互联
## 硬件环境搭建
-    硬件要求：Hi3861V100核心板、扩展板；硬件搭建如下图所示。
-    [Hi3861V100核心板参考：HiSpark_WiFi_IoT智能开发套件_原理图硬件资料\原理图\HiSpark_WiFi-IoT_Hi3861_CH340G_VER.B.pdf](http://gitee.com/hihope_iot/embedded-race-hisilicon-track-2022/blob/master/%E7%A1%AC%E4%BB%B6%E8%B5%84%E6%96%99/HiSpark_WiFi_IoT%E6%99%BA%E8%83%BD%E5%AE%B6%E5%B1%85%E5%BC%80%E5%8F%91%E5%A5%97%E4%BB%B6_%E5%8E%9F%E7%90%86%E5%9B%BE.rar)
-    [扩展板参考：HiSpark_WiFi_IoT智能开发套件_原理图硬件资料\原理图\HiSpark_WiFi-IoT_Hi3861_CH340G_VER.B.pdf](http://gitee.com/hihope_iot/embedded-race-hisilicon-track-2022/blob/master/%E7%A1%AC%E4%BB%B6%E8%B5%84%E6%96%99/HiSpark_WiFi_IoT%E6%99%BA%E8%83%BD%E5%AE%B6%E5%B1%85%E5%BC%80%E5%8F%91%E5%A5%97%E4%BB%B6_%E5%8E%9F%E7%90%86%E5%9B%BE.rar)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/260.jpg)

## 软件介绍
-   1.代码目录结构及相应接口功能介绍
```
vendor_hisilicon/hispark_pegasus/demo/iottencent_demo
├── app_demo_iot.c      #IoSetPull();IoSetFunc();TaskMsleep()。
├── BUILD.gn            # BUILD.gn文件由三部分内容（目标、源文件、头文件路径）构成,开发者根据需要填写,static_library中指定业务模块的编译结果，为静态库文件led_example，开发者根据实际情况完成填写。
|                        sources中指定静态库.a所依赖的.c文件及其路径，若路径中包含"//"则表示绝对路径（此处为代码根路径），若不包含"//"则表示相对路径。include_dirs中指定source所需要依赖的.h文件路径。
├── cjson_init.c        #
├── iot_config.h        # 
├── iot_hmac.c          # 
├── iot_hmac.h          # 
├── iot_log.c           # 
├── iot_log.h           # 
├── iot_main.c          # 
├── iot_main.h          # 
├── iot_profile.c       # 
├── iot_sta.c           # 
└── iot_profile.h       # 
```
-   2.Mqtt协议第三方软件与腾讯云之间的通信

2.1 浏览器搜索腾讯云，在腾讯云界面创建产品和设备，登录腾讯云后搜索“物联网通信”，进入页面后，物联网通信选择立即使用。进入页面后，选择产品列表栏->创建新产品->产品名称：xxx,认证方式：密钥认证，点击确定。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8717.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8720.png)

2.2 在产品列表栏进入刚刚创建的新产品->设备列表->添加设备->创建新设备：设备名称，设备备注（选填），使用物联网通信提供密钥->保存->复制保存生成的密钥->开始管理设备（可以看到设备相应信息）。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8721.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8722.png)

2.3 添加设备，同时查看设备相应信息。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F/800.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F/801.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F/802.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F/803.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F/804.png)

2.4 返回设备列表可以看到设备并未激活，接下来激活设备；下载[MQTT.fx-1.7.1 ](https://www.jensd.de/wordpress/?p=2746)，下载完成后打开工具，点击设备图标，进入到设备界面配置ClientID，username,password 等参数。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%BE%AE%E4%BF%A1%E5%B0%8F%E7%A8%8B%E5%BA%8F/806.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8730.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8731.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8732.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8733.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8734.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8735.png)

2.5 完成2.3步骤后，可以在腾讯云设备列表里面看到设备状态显示已激活，开始腾讯云与MQTT工具之间的通信。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8736.png)

2.6 MQTT.fx-1.7.1工具订阅腾讯云的topic，腾讯云下发指令。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8737.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8738.png)

2.7 在腾讯云网页端点击“在线调试”。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8739.png)

2.8 可以在MQTT.fx-1.7.1工具中看到腾讯云网页端发过来的消息。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8740.png)

2.9 MQTT.fx-1.7.1工具发布腾讯云到的topic，在设备影子中查看发布到腾讯云的消息。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8741.png)

```
{
	"type":"update",
        "state":{
        "reported":{
	"light":1,
        "motor":1,
        "temperature":25,
        "humidity":60,
        "light_intensity":50
    }
  },
"version":0,
"clientToken":"clientToken"
}
```

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8742.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8743.png)

-   3.微信小程序开发环境搭建及与腾讯云之间通信

3.1 注册申请小程序账号：https://mp.weixin.qq.com/wxopen/waregister?action=step1。

3.2 登录小程序后（选择IOT字样的账号），在“开发”->“开发设置”页面可以看到小程序的AppID，如下图所示，详情请查看微信官方文档：
https://developers.weixin.qq.com/miniprogram/dev/framework/quickstart/getstart.html#%E7%94%B3%E8%AF%B7%E5%B8%90%E5%8F%B7。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8744.png)

3.3 开发者工具下载与安装：https://developers.weixin.qq.com/miniprogram/dev/devtools/download.html。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8745.png)

3.4 下载安装完成后，打开微信开发工具，导入提供的微信小程序软件包[获取微信小程序代码](https://gitee.com/leo593362220/Hi3861OpenHarmony)。并填入自己的APPID,后端服务选择微信云开发。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%87102.jpg)

3.5 使用微信开发者工具打开工程后需要修改project.config.json文件中appid,projectname修改成自己的。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8747.png)

3.6 获取secretId和secretKey，在腾讯云->产品列表->搜索栏搜索访问密钥->访问密钥->继续使用->可以看到secretId和secretKey，复制在xxxx.txt文件，后面需要。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8749.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8750.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8751.png)

3.7 环境ID配置并获取环境ID,复制在xxx.txt文件，后面需要。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8752.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8753.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8754.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8755.png)![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8747.png)

3.8 需要修改app.js文件中productId（腾讯云上设备信息上产品ID）,deviceName（腾讯云上设备信息上产品名称）,secretId和secretKey(3.6步骤获取到的secretId和secretKey)，env（3.7步骤获取到的环境ID）。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8748.png)

3.9 配置当前云环境

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8756.png)

注意：第一次部署环境，图片里面的上传并部署会变成新建并部署。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8757.png)

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/228.jpg)

等待大概1分钟左右，直到出现上传云函数成功：（注意：上传并部署由于网络问题可能会失败，看到上传云函数等字样代表上传部署成功，这里以iothub-publish为例）。

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8758.png)

3.10 环境部署完成后，点击编译，然后真机调试会出现如下界面，上面两个都要出现

![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8761.png)

-   4.Hi3861V100与微信小程序之间通信
    -    将源码./vendor/hisilicon/hispark_pegasus/demo目录下的iottencent_demo整个文件夹及内容复制到源码./applications/sample/wifi-iot/app/下。
    ```
    .
    └── applications
        └── sample
            └── wifi-iot
                └── app
                    └──iottencent_demo
                       └── 代码   
    ```

    -    配置源码./applications/sample/wifi-iot/app/iottencent_demo/iot_config.h中CONFIG_AP_SSID，CONFIG_AP_PWD为WiFi名称和WiFi密码。CONFIG_USER_ID为腾讯云设备信息上MQTT Username，CONFIG_USER_PWD为腾讯云上设备的MQTT Password,CN_CLIENTID为腾讯云上clientID。
    ```
    #define CONFIG_AP_SSID  "XXXXXXXX" // WIFI SSID
    #define CONFIG_AP_PWD   "XXXXX" // WIFI PWD
    /* Tencent iot Cloud user ID , password */
    #define CONFIG_USER_ID    "XXXXXXXXXXXXX"
    #define CONFIG_USER_PWD   "XXXXXXXXXXXX"
    #define CN_CLIENTID     "xxxxxxxx" // Tencent cloud ClientID format: Product ID + device name
    ```

    -    更改源码./applications/sample/wifi-iot/app/iottencent_demo/iot_main.c文件中搜索g_defaultSubscribeTopic字段，将腾讯云上的Topic列表中自定义Topic添加到字段里。
    ```
    static const char *g_defaultSubscribeTopic[] = {
        "76VQ4ZASTL/mqtt/data", 
        "76VQ4ZASTL/mqtt/event",
        "76VQ4ZASTL/mqtt/control",
    };
    ```
    -    更改源码./applications/sample/wifi-iot/app/iottencent_demo/iot_profile.c文件中搜索CN_PROFILE_TOPICFMT_TOPIC字段，将腾讯云上的产品名，设备名称添加到字段里。
    ```
    #define CN_PROFILE_TOPICFMT_TOPIC            "$shadow/operation/xxxxx/XXXX"  //xxxx为产品名和设备名称，如：76VQ4ZASTL
    ```

    -    修改源码./applications/sample/wifi-iot/app/BUILD.gn文件，在features字段中增加索引，使目标模块参与编译。features字段指定业务模块的路径和目标,features字段配置如下。
    ```
    import("//build/lite/config/component/lite_component.gni")

    lite_component("app") {
        features = [
            "iottencent_demo:appDemoIot",
        ]
    }
    ```

    -    工程相关配置完成后,然后编译。
-   5.烧录
    -    编译成功后，点击DevEco Home->配置工程->hi3861->upload_port->选择对应串口端口号->选择upload_protocol->选择hiburn-serial->点击save保存，在保存成功后点击upload进行烧录，出现connecting, please reset device..字样复位开发板等待烧录完成。烧录成功后，再次点击Hi3861核心板上的“RST”复位键，此时开发板的系统会运行起来。
    -    注意:一定要断开MQTT工具的连接。
    -    微信小程序上选择已连接Wifi,跳过配网->云端互联->看到设备互联界面->点击刷新按钮，运行结果出现如下图代表成功，点击照明，主板灯会亮。

    ![输入图片说明](https://gitee.com/asd1122/tupian/raw/master/%E5%9B%BE%E7%89%87/%E5%9B%BE%E7%89%8762.png)