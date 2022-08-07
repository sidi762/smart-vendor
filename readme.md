# 基于手势识别的非接触式智能商品售卖机
### Smart Vendor: A Smart, Non-Contact Vending Machine based on Static Gesture Recognition
#### 梁思地 刘奕 刘亦骢  
### 本项目为2022嵌入式芯片与系统设计竞赛应用赛道参赛项目，在全国总决赛中获国家二等奖。

```
  _____ __  __          _____ _______  __      ________ _   _ _____   ____  _____  
 / ____|  \/  |   /\   |  __ \__   __| \ \    / /  ____| \ | |  __ \ / __ \|  __ \  
| (___ | \  / |  /  \  | |__) | | |     \ \  / /| |__  |  \| | |  | | |  | | |__) |  
 \___ \| |\/| | / /\ \ |  _  /  | |      \ \/ / |  __| | . ` | |  | | |  | |  _  /  
 ____) | |  | |/ ____ \| | \ \  | |       \  /  | |____| |\  | |__| | |__| | | \ \  
|_____/|_|  |_/_/    \_\_|  \_\ |_|        \/   |______|_| \_|_____/ \____/|_|  \_\  
```
[![license](https://img.shields.io/github/license/sidi762/smart-vendor?logo=gitee&style=for-the-badge)](./LICENSE)  
![linux](https://img.shields.io/badge/GNU-Linux-green?style=flat&logo=GNU)
![OHOS](https://img.shields.io/badge/OpenAtom-OpenHarmony-green?style=flat&logo=Huawei)
![miniprogram](https://img.shields.io/badge/WeChat-Mini%20Program-green?style=flat&logo=WeChat)  
[![JavaScript Style
Guide](https://cdn.rawgit.com/feross/standard/master/badge.svg)](https://github.com/feross/standard)  

  Copyright (c) 2022 Sidi Liang, Yi Liu, Yicong Liu.  
  Licensed under the Apache License, Version 2.0 (the "License");  
  you may not use this file except in compliance with the License.  
  You may obtain a copy of the License at  

      http://www.apache.org/licenses/LICENSE-2.0  

  Unless required by applicable law or agreed to in writing, software  
  distributed under the License is distributed on an "AS IS" BASIS,  
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
  See the License for the specific language governing permissions and  
  limitations under the License.  

  ## __目录__

  * [__文件组成说明__](#文件组成说明)
  * [__安装运行__](#安装运行)
  * [__设计概述__](#设计概述)
    * [应用领域](#应用领域)
    * [主要技术特点](#主要技术特点)
    * [关键性能指标](#关键性能指标)
  * [__系统组成及功能说明__](#系统组成及功能说明)
    * [各模块介绍](#各模块介绍)
  * [__完成情况及性能参数__](#完成情况及性能参数)
    * [机器视觉手势识别模块](#1.-机器视觉手势识别模块)
    * [传感器、电机驱动及物联网通信模块](#2.-传感器、电机驱动及物联网通信模块)
    * [可视化界面模块、后台管理模块、云端物联网平台模块及作品硬件设计](#3.-可视化界面模块、后台管理模块、云端物联网平台模块及作品硬件设计)
  * [__参考文献__](#参考文献)

## __文件组成说明__  
* `./device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/smart_vendor_ai`：机器视觉手势识别模块（Hi3516DV300开发板）软件代码
* `./applications/sample/wifi-iot`：传感器、电机驱动及物联网通信模块（Hi3861V100开发板）软件代码
* `./miniprogram_ui`：可视化界面模块（微信小程序）
* `./dl`：静态手势识别模型文件以及部分训练相关代码

## __安装运行__
### 机器视觉手势识别模块
#### 环境要求
* `Hi3516DV300 Taurus AI`开发套件  
* `OpenHarmony v3.1 小型系统 (small system)`编译环境及源码, 可参考[OpenHarmony官方文档](https://docs.openharmony.cn/pages/v3.1/zh-cn/device-dev/device-dev-guide.md/)进行配置

#### 编译
首先将`device`文件夹内所有内容合并至OpenHarmony中`device`路径下。  
可使用华为 DevEco Device Tool IDE方式或命令行方式进行编译。以下是命令行方式编译说明。  

首先进入OpenHarmony目录下：
```bash
cd OpenHarmony
```
使用hb工具进行编译：
```bash
hb set .
ipcamera_hispark_taurus_linux/ipcamera_hispark_taurus
hb build -f
```
编译结果位于 `./out/hispark_taurus/ipcamera_hispark_taurus/bin/smart_vendor_main`

#### 烧录
详见[OpenHarmony官方文档](https://docs.openharmony.cn/pages/v3.1/zh-cn/device-dev/device-dev-guide.md/)。

#### 安装
1. 将`dl`目录下`gesture_classification_v6.wk`模型文件拷贝至Hi3516DV300开发板中`/userdata/models/smart_vendor_ai`目录下。
2. 将`dl`目录下`hand_detect.wk`模型文件拷贝至Hi3516DV300开发板中`/userdata/models/hand_classify/`目录下。
3. 将编译得到的二进制文件`smart_vendor_main`拷贝至Hi3516DV300开发板中。

#### 运行
在Hi3516DV300开发板上，首先启用`hi_mipi_tx`驱动：
```bash
insmod /ko/hi_mipi_tx.ko
```
运行程序:
```bash
./smart_vendor_main
```

### 传感器、电机驱动及物联网通信模块
#### 环境要求
* `Hi3861V100 Wi-Fi IoT`开发套件
* `OpenHarmony v3.1 轻量系统 (mini system)`编译环境及源码, 可参考[OpenHarmony官方文档](https://docs.openharmony.cn/pages/v3.1/zh-cn/device-dev/device-dev-guide.md/)进行配置

___

## __设计概述__

本项目为2022嵌入式芯片与系统设计竞赛参赛项目，在全国总决赛中获国家二等奖。  
自动售货机自发明以来得到了广泛流行，给人类生活带来了极大方便。然而，在当今新冠肺炎疫情的背景下，自动售货机按钮、 触屏的交互方式要求用户直接接触设备，存在一定疫情传播风险，因此需要设计一种无接触的交互方式以降低疫情传播风险、保护用户个人健康[1]。本项目利用 Hi3516DV300 计算机视觉与 Hi3861V100 WiFi-IoT 开发套件设计了一种利用手势进行交互的智能商品售卖机，可通过手势识别实现商品选择等功能，选择商品、支付成功后自动出货，达到了无接触交互的目的，安全卫生、交互方便。本项目同时采用了物联网技术与设计思想，实现了数据上传、数据分析、远程数据更新等功能。

### 应用领域
自动售货机本身具有可售商品范围广泛、部署便捷、低成本的特点[2]，可应用于各类公共场所，具有极广泛的应用场景，并有助于提升生产总值、促进经济发展[3]。本项目由于采用了无接触的交互方式，尤其适用于医院、机场、火车站等人流量大、对公共卫生要求高的特殊公共场所，可在方便顾客使用、促进零售业发展的同时确保用户的个人健康安全，降低疫情传播风险。

### 主要技术特点
本项目主要使用深度学习技术，选用自行训练的Resnet18深度神经网络对静态手势进行识别以实现无接触交互的目的。手势识别基于Hi3516DV300开发板的SVP智能视觉平台实现，拍摄视频获取画面与深度神经网络推理均在板端完成，无需上传视频流至服务器，在保护用户隐私安全的同时可节省带宽资源，获得低延迟的使用体验。  

本项目系统架构基于云平台（腾讯云物联网通信平台）进行设计，云端与各模块间使用MQTT物联网通信协议进行数据传输，并利用*设备影子*实现数据同步，实现了轻量、稳定、快速的数据传输。

* 创新性地为自动售货机设计了基于静态手势识别的无接触交互方式，满足了新冠肺炎疫情下自动售货机对于无接触交互方式的需求，同时保证了使用的简易性、便捷性。我们经过调查研究发现，此前未有自动售货机采用类似的交互方式。  

* 在手势识别方案上，我们使用了Yolo v2与Resnet18深度神经网络串联的方案。我们自制了包含约30000张图片的静态手势识别数据集，按照约6:2:2的比率随机划分训练集、验证集与测试集，在训练中应用了随机裁切等数据增强手段、降低学习率、随机梯度下降（Stochastic gradient descent， SGD）优化器等技术，提高了模型准确率，并在训练过程中应用了GradCam++可视化技术对模型训练结果进行可视化，借以评估模型训练效果，对数据集进行有针对性的改进。在板端识别时，针对单次识别效果欠佳的问题，我们采取连续多次采样，取用连续相同识别结果为最终结果的策略，有效提高了系统识别稳定性与准确率。  

* 设计了基于云平台与物联网技术的系统架构，使用MQTT物联网通信协议进行云端与各模块间的远程通信，初步实现了自动售货机的智能化并具备良好的灵活性与可拓展性，可适用于不同的部署规模，同时具有轻量化、低成本的优点。  

* 自主进行了设备外观与结构设计与制造，在实现预期目标功能的同时兼顾了美观性与实用性。  

### 关键性能指标
* 此售货机使用的深度学习手势识别模型的准确率达92%，在绝大多数的情况下可以快速和准确的识别使用者所展示的手势，具有良好的鲁棒性，受识别场景背景的影响较小。  

* 在网络通畅的情况下，本项目已展示出了良好的稳定性。  

## __系统组成及功能说明__

![img1](https://gitee.com/sidi762/smart-vendor/raw/master/img/img1.png "图 1")  
本作品主要分为如下五个模块，如图 1 所示：基于Hi3516DV300 Taurus AI计算机视觉平台的机器视觉手势识别模块、基于Hi3861V100 Pegasus WiFi-IoT套件的传感器、电机驱动及物联网通信模块、基于微信小程序开发的管理后台模块与可视化用户界面模块，和云端物联网平台模块。

机器视觉手势识别模块可获取实时视频流，进行板端AI推理，实现手势识别功能。传感器、电机驱动及物联网通信模块连接并控制红外传感器、直流电机驱动板等外设，通过UART串口与机器视觉手势识别模块进行通信，向机器视觉手势识别模块发送启动信号、接收手势识别结果。同时该模块使用MQTT物联网通信协议与腾讯云物联网通信平台进行通信，并可根据物联网通信平台下发的控制指令控制直流电机工作。可视化界面显示供选择的货道信息，通过MQTT协议与物联网通信平台进行通信，传输完成交易流程所需要的信息。管理后台小程序中包括剩余货量查询、货道信息修改等功能，通过MQTT协议与物联网通信平台进行通信。

图 2 展示了系统的完整工作流程。  
![img2](https://gitee.com/sidi762/smart-vendor/raw/master/img/img2.png "图 2")  

### 各模块介绍
#### 1. 机器视觉手势识别模块

机器视觉手势识别模块基于Hi3516DV300 Taurus AI计算机视觉平台进行开发，如图 3所示。该模块使用自身摄像头获取实时视频流，利用NNIE神经网络引擎对获取的画面帧进行板端AI推理，实现手势识别功能。同时，售货机相关数据（如产品名称、价格等）以JS对象简谱（JavaScript Object Notation，JSON）格式存储在该模块的SD卡中。该模块与传感器、电机驱动及物联网通信模块通过UART串口进行实时通信，以JSON数据格式将识别结果发送至传感器、电机驱动及物联网通信模块。同时，在交易完成后，更新后的售货机相关数据也通过UART串口发送至传感器、电机驱动及物联网通信模块，使其在系统各模块间同步。  


![img3](https://gitee.com/sidi762/smart-vendor/raw/master/img/img3.png "图 3: 机器视觉手势识别模块实拍图")  


##### 使用深度神经网络进行静态手势识别
![img4](https://gitee.com/sidi762/smart-vendor/raw/master/img/img4.png "图 4: 静态手势识别实现方案")  

我们使用了Yolo v2与Resnet18双神经网络串联作为静态手势识别方案，如图 4所示。获取到画面后，首先使用Yolo v2深度神经网络进行手部检测、裁剪，再将裁切后的图片送至Resnet18深度神经网络进行静态手势分类。经实验验证，这个方案可有效排除背景等无效信息对识别结果的干扰，极大提高了静态手势识别准确率。我们发现现有成熟Yolo v2手部检测模型具有识别准确率高、稳定性强的优点，因此我们在手部检测及裁切环节中直接使用了该模型。我们自行拍摄制作了静态数字手势数据集，数据集共包含9种数字手势、5种其他手势共14种手势的图片与一定数量的背景集，共约3万张图片。我们按照约6:2:2的比率随机划分出了训练集、验证集与测试集。我们将该数据集用于Resnet18 静态手势识别网络的训练。在训练过程中我们应用了图像归一化等预处理方法，应用了随机裁切等数据增强手段、并应用了降低学习率、随机梯度下降（Stochastic gradient descent， SGD）优化器等训练微调技术。结果证明，我们采取的这些策略有效提高了模型准确率。  

![img5](https://gitee.com/sidi762/smart-vendor/raw/master/img/img5.png "图 5: GradCam++ 可视化结果")   

此外，我们在训练过程中应用了GradCam++可视化技术对模型训练结果进行了可视化，针对目标类别绘制了热图，如图 5所示。我们以此为依据评估模型训练效果，在后续训练中对数据集进行了有针对性的改进，并确保了我们的模型具有良好的可解释性。  

![img6](https://gitee.com/sidi762/smart-vendor/raw/master/img/img6.png "图 6: 静态手势识别模型处理流程")   

图 6 为我们静态手势识别模型的处理、适配、部署流程。我们使用NNIE RuyiStudio工具对训练好的模型进行了量化处理，降低了性能需求，使其适用于嵌入式设备。在板端识别时，针对单次识别效果欠佳的问题，我们采取连续多次采样，取用连续相同识别结果为最终结果的策略，有效提高了系统识别稳定性与准确率。  

#### 2. 传感器、电机驱动及物联网通信模块
传感器、电机驱动及物联网通信模块以Hi3861V100 Pegasus平台为核心来实现对4个货道的电机的控制以及与其他各模块之间的通讯和数据传输，如图 7所示。  

![img7](https://gitee.com/sidi762/smart-vendor/raw/master/img/img7.png "图 7: 传感器、电机驱动及物联网通信模块实拍图")  

![img8](https://gitee.com/sidi762/smart-vendor/raw/master/img/img8.png "图 8: 传感器、电机驱动及物联网通信模块结构框图")  

##### 各货道出货电机控制  
考虑到智能售货机中各个货道商品的重量以及整个系统的供电能力，团队选用N20减速电机来驱动货道中的弹簧，以达到低电压高扭力的效果。本设计通过控制电机转动的时间来使货物掉落。由于Pegasus外设板5V供电电压的限制，在设计中我们采用了L298N双H桥直流电机驱动模块。每个驱动模块都有一个电池盒作为外部供电电源，这样也可以保护Pegasus核心板。每个驱动模块最多可以连接两个电机。通过将驱动模块的输入端口与Pegasus外设上的输出GPIO端口相连以实现程序对电机的控制。  

##### Hi3861V100 与 Hi3516DV300板间通讯
为了获取到手势识别的结果，Hi3861V100 WiFi-IOT与Hi3516DV300 之间需要进行数据的传输。考虑到传输效率，结构复杂程度等因素，我们团队采用自行设计的通信协议，通过UART串口进行传输。在确保传输效率与可靠性的同时，使系统硬件构成更加简洁。  

##### 人体红外传感器
智能售货机利用人体红外传感器实现感知顾客靠近的功能，当用户靠近后再启动识别流程，以此降低系统运行能耗。传感器利用热释电原理检测人体活动，一旦人进入探测区域内，即发出高电平信号，如图 10所示。


![img10](https://gitee.com/sidi762/smart-vendor/raw/master/img/img10.png "图 10: 人体红外传感器")  


##### 基于MQTT物联网通讯协议的云端互联
![img11](https://gitee.com/sidi762/smart-vendor/raw/master/img/img11.png "图 11: Hi3861V100云端互联示意图")  

1. Wi-Fi连接  
团队充分利用Hi3861V100上的Wi-Fi模块，通过调用自有API实现与场所Wi-Fi的连接。团队通过代码实现了Pegasus平台对网络的自我诊断，当Wi-Fi信号出现波动，连接不稳定时，系统会进行自我修复，重新连接网络确保正常工作。本作品配有网络连接信号灯，便于工作人员日常管理。  

2. 云端互联  
本作品采用腾讯云的物联网平台进行后台管理和数据的暂时储存。Pegasus平台作为整个系统中的中枢，具有将3516模块中储存的货道信息转移到云上的能力。本作品采用MQTT数据传输协议来实现快速稳定的数据传输。在接受云端数据方面，团队为系统单独创建了一个线程用来监听云端，每当有云端有数据下发，Pegasus都能够快速判断并作出正确反应。多线程架构的建立使得系统处理任务更加迅速，同时避免了不同任务之间的干扰，使得系统更加高效可靠。  

![img12](https://gitee.com/sidi762/smart-vendor/raw/master/img/img12.png "图 12: Pegasus平台系统程序架构")  


#### 3. 可视化界面模块  
可视化界面使用微信小程序实现，由一台可运行微信小程序的终端显示。界面设计上，我们将清晰展示产品信息作为首要目标，因此我们设计将页面四等分，每一个区域显示对应商品的名称与价格，并将价格使用红色字体标出，达到突出显示的效果。可视化界面模块与腾讯云平台通过MQTT协议进行数据传输。当顾客付款成功后，终端会向云端发送success信号，并通过云转发至Pegasus平台驱动电机出货。每当交易完成，终端就会从云端拉取各个货道信息并更新界面，保持信息的同步。  

![img13](https://gitee.com/sidi762/smart-vendor/raw/master/img/img13.png "图 13: 用户可视化界面")    

#### 4. 后台管理模块  
在售货机的后台管理小程序中，我们使用了与用户界面相似的设计布局，并将可更改的部分使用蓝色进行标注，方便后台维护人员更新售货机产品数据。除此之外，我们还自制了一份手势温馨提示，内容为每个货道对应的标准手势图示，降低人们在初次使用此类售货机时的学习成本。后台管理模块也是通过MQTT协议与云端通讯以保持数据的同步。每当货道信息发生变化，后台就会拉取新的数据进行更新。当维护人员进行补货时，可以通过界面操作来更改货道信息。更改后的信息也会及时传到云端进行数据同步。图14为后台管理界面。  

![img14](https://gitee.com/sidi762/smart-vendor/raw/master/img/img14.png "图 14: 后台管理小程序界面")  

#### 5. 云端物联网平台模块

![img15](https://gitee.com/sidi762/smart-vendor/raw/master/img/img15.png "图 15: MQTT通信示意图")  

本作品采用腾讯云物联网通信平台作为云端管理系统进行数据的传输与短时储存。我们选用MQTT物联网通讯协议实现各模块间与云端的通信任务。不同模块通过订阅不同的Topic来实现与云端的数据传输与权限管理，同时在腾讯云物联网通信平台内部我们使用其提供的规则引擎来实现不同Topic之间数据的转发，避免相互干扰，确保数据传输的可靠性。我们的云端物联网平台的通讯结构如图15所示。  

本作品采用腾讯云的设备影子作为中介来支持设备与用户应用程序查看和更新设备状态。设备、用户应用程序、设备影子三者之间通过两个特殊的 Topic 来实现通信：  
`$shadow/operation/${productId}/${deviceName}`：用于发布（上行）消息，可实现对设备影子数据的 get/update 操作。  
`$shadow/operation/result/${productId}/${deviceName}`：用于订阅（下行）消息，影子服务端通过此 Topic 发送应答和推送消息。  

![img16](https://gitee.com/sidi762/smart-vendor/raw/master/img/img16.png "图 16: 设备影子数据传输")  

#### 6. 作品硬件设计
在售货机的初期设计阶段，我们使用了CAD设计软件对硬件进行了建模设计并决定使用透明亚克力板作为搭建材料。由于其良好的透光性，使用亚克力板可方便展示售货机内部结构，如图 17所示。然而，我们发现，亚克力板会导致售货机内部较为严重的光反射现象和背景干扰。因此，我们在第二版本中选择使用白色不透明亚克力板和透明亚克力板相结合搭建，这样不仅可以观察到售货机的工作状态，也可以极大的改善装置内部反光情况。  

![img17](https://gitee.com/sidi762/smart-vendor/raw/master/img/img17.png "图 17: 初版作品实拍图")

## __完成情况及性能参数__
我们已经基本完成了各模块的开发以及系统联调，已基本实现全部设计功能，并且相关性能均达到设计指标。以下将分模块介绍具体完成情况。  

### 1. 机器视觉手势识别模块  
目前该模块已完成相关开发，实现了全部设计功能，识别准确率、系统延迟等技术指标基本达到了设计目标。  

由测试结果可得，轻量化后的手势识别模型准确率在92%左右。虽然略低于Pytorch框架下模型准确率，但考虑到轻量化过程中精度的损失，92%准确率仍然在我们预期范围内，可以接受。  

![img19](https://gitee.com/sidi762/smart-vendor/raw/master/img/img19.png "图 19: 模型训练过程中损失率（loss）的变化")  
![img20](https://gitee.com/sidi762/smart-vendor/raw/master/img/img20.png "图 20: 训练过程中模型识别准确率（accuracy）的变化")  

### 2. 传感器、电机驱动及物联网通信模块

#### 软件  
Pegasus平台软件已完成基本开发，可在Hi3861V100开发板中运行。程序能够实现人体红外的识别感应，与腾讯云平台和Hi3516DV300开发板之间的数据传输以及对电机的控制。

#### 硬件  
已成功完成电路搭建与连接，系统能够正常工作，具体实物如图21所示：  

![img21](https://gitee.com/sidi762/smart-vendor/raw/master/img/img21.png "图 21 1: 硬件实物图")  
![img22](https://gitee.com/sidi762/smart-vendor/raw/master/img/img21_1.png "图 21 2: 硬件实物图")

### 3. 可视化界面模块、后台管理模块、云端物联网平台模块及作品硬件设计
我们已经完成了全部的售货机搭建工作，并且已经完成各模块安装与功能验证。与此同时，我们已经完成了可视化界面、管理后台模块与其余模块通过腾讯云物联网平台进行的互联功能验证。我们完成并验证了管理后台模块小程序更改商品信息，并将其自动上传至腾讯云物联网平台的设备影子，同步至售货机的界面的全流程。  

## 参考文献
```
[1] Sang M. Lee and DonHee Lee, "Opportunities and challenges for contactless healthcare services in the post-COVID-19 Era", Technological Forecasting and Social Change, vol. 167, pp. 120712, 2021
[2] 梁永馨, 王凤越. 自动售货机的发展现状及趋势[J]. 当代经济, 2019(2):4.
[3] 余世明，晁岳磊，缪仁将. 自动售货机研究现状及展望[J]. 中国工程科学, 2008, 10(7):6.
```
