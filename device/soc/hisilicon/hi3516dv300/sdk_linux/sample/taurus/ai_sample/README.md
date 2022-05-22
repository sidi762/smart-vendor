# 5.综合实验案例

## 1. 概述

综合案例章节中，我们将在Hi3516DV300 SDK的基础之上进行开发，分别为手部检测+手势识别实验，垃圾分类实验。手部检测+手势识别实验以及垃圾分类实验，主要基于训练好的wk模型在板端进行部署，并充分发挥海思IVE、NNIE硬件加速能力，完成AI推理和业务处理。

## 2. 目录

* ai_sample在Hi3516DV300 SDK基础上进行开发，在利用媒体通路的基础上，通过捕获VPSS帧进行预处理操作，并送至NNIE进行推理，结合AI CPU算子最终得到AI Flag并进行相应业务处理，该AI sample集成了垃圾分类、手势检测识别两个基础场景，运用到媒体理论、多线程、IPC通信、IVE、NNIE等思想，实现了一个轻量级sample，方便开发者了解taurus Hi3516DV300的AI能力，ai_sample目录结构如下：

```shell
//device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/ai_sample
│  BUILD.gn                    # 编译ohos ai_sample需要的gn文件
├─ai_infer_process             # AI前处理、推理、后处理相关接口
│  ├─ai_infer_process.c
│  └─ai_infer_process.h
├─dependency                  # ai sample依赖的一些功能，如语音播报
│  ├─audio_test.c
│  └─audio_test.h
├─ext_util					  # 常用的基础接口、可移植操作系统接口posix等
│  ├─base_interface.c
│  ├─base_interface.h
│  ├─misc_util.c
│  ├─misc_util.h
│  ├─posix_help.c
│  └─posix_help.h
├─mpp_help        		     # 封装的媒体相关接口
│  ├─include
│  │  ├─ive_img.h
│  │  └─vgs_img.h
│  └─src
│    ├─ive_img.c
│    └─vgs_img.c
├─scenario
│  ├─cnn_trash_classify        # 垃圾分类sample
│  │   ├─cnn_trash_classify.c
│  │   └─cnn_trash_classify.h
│  └─hand_classify             # 手部检测+手势识别sample
│      ├─hand_classify.c
│      ├─hand_classify.h
│      ├─yolov2_hand_detect.c
│      └─yolov2_hand_detect.h
└─smp					   # ai sample主入口及媒体处理文件
  ├─sample_ai_main.cpp
  ├─sample_media_ai.c
  └─sample_media_ai.h
```

## 3.拷贝第三方库

* 步骤1：在源码的根目录下，分步执行下面的命令，拷贝第三方库至ai sample中

```
mkdir device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/ai_sample/third_party/src/ -p

cp third_party/iniparser device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/ai_sample/third_party/src/ -rf
```

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/046%E6%8B%B7%E8%B4%9D%E7%AC%AC%E4%B8%89%E6%96%B9%E5%BA%93%E8%87%B3ai%20sample.png)

* 步骤2：使用IDE打开device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/ai_sample/third_party/src/iniparser/src/iniparser.c文件，把第15行的1024改成8192。

```c
#define ASCIILINESZ         ((8192))  // (1024)
```

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/047%E4%BF%AE%E6%94%B9iniparser.png)

## 4. 编译

在编译ai_sample之前，需确保OpenHarmony 小型系统的主干代码已经整编通过，**且已经按照《[修改源码及配置文件适配Taurus开发板](../doc/2.2.1.%E4%BF%AE%E6%94%B9%E6%BA%90%E7%A0%81%E5%8F%8A%E9%85%8D%E7%BD%AE%E6%96%87%E4%BB%B6%E9%80%82%E9%85%8DTaurus%E5%BC%80%E5%8F%91%E6%9D%BF.md)》的内容进行修改**。在单编ai_sample之前，需修改目录下的一处依赖，进入//device/soc/hisilicon/hi3516dv300/sdk_linux目录下，通过修改BUILD.gn，在deps下面新增target，``"sample/taurus/ai_sample:hi3516dv300_ai_sample"``，如下图所示：

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/033%E4%BF%AE%E6%94%B9BUILDgn.png)

* 点击Deveco Device Tool工具的Build按键进行编译，具体的编译过程这里不再赘述，编译成功后，如下图所示：

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_helloworld_sample/0002-build%20success.png)

* 编译成功后，即可在out/hispark_taurus/ipcamera_hispark_taurus_linux/rootfs/bin目录下，生成 ohos_camera_ai_demo可执行文件，如下图所示：

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/044%E5%BE%97%E5%88%B0ai%E5%8F%AF%E6%89%A7%E8%A1%8C%E6%96%87%E4%BB%B6.png)

## 5. 拷贝可执行程序和依赖文件至开发板的mnt目录下

**方式一：使用SD卡进行资料文件的拷贝**

* 首先需要自己准备一张SD卡

* 步骤1：将编译后生成的可执行文件拷贝到SD卡中。

* 步骤2：将device\soc\hisilicon\hi3516dv300\sdk_linux\out\lib\目录下的**libvb_server.so和 libmpp_vbs.so**拷贝至SD卡中
* 步骤3：将device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/目录下的models文件夹和aac_file文件夹拷贝至SD卡中。（前提是按照《[获取sample依赖的资源文件到本地](../doc/6.2.%E8%8E%B7%E5%8F%96sample%E4%BE%9D%E8%B5%96%E7%9A%84%E8%B5%84%E6%BA%90%E6%96%87%E4%BB%B6%E5%88%B0%E6%9C%AC%E5%9C%B0.md)》文档获取了资源文件）

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/035%E5%B0%86%E5%8F%AF%E6%89%A7%E8%A1%8C%E6%96%87%E4%BB%B6%E5%92%8C%E4%BE%9D%E8%B5%96%E6%96%87%E4%BB%B6%E6%8B%B7%E8%B4%9D%E8%87%B3SD%E5%8D%A1.png)

* 步骤4：可执行文件拷贝成功后，将内存卡插入开发板的SD卡槽中，可通过挂载的方式挂载到板端，可选择SD卡 mount指令进行挂载。

```shell
mount -t vfat /dev/mmcblk1p1 /mnt
# 其中/dev/mmcblk1p1需要根据实际块设备号修改
```

* 挂载成功后，如下图所示：

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/036%E6%8C%82%E8%BD%BDSD%E5%8D%A1%E8%87%B3%E5%BC%80%E5%8F%91%E6%9D%BF.png)

**方式二：使用NFS挂载的方式进行资料文件的拷贝**

* 首先需要自己准备一根网线

* 步骤1：参考[博客链接](https://blog.csdn.net/Wu_GuiMing/article/details/115872995?spm=1001.2014.3001.5501)中的内容，进行nfs的环境搭建

* 步骤2：将编译后生成的可执行文件拷贝到Windows的nfs共享路径下

* 步骤3：将device\soc\hisilicon\hi3516dv300\sdk_linux\out\lib\目录下的**libvb_server.so和 libmpp_vbs.so**拷贝至Windows的nfs共享路径下

* 步骤4：将device/soc/hisilicon/hi3516dv300/sdk_linux/sample/taurus/目录下的**models文件夹**和**aac_file文件夹**拷贝至Windows的nfs共享路径下（前提是按照《[获取sample依赖的资源文件到本地](../doc/6.2.%E8%8E%B7%E5%8F%96sample%E4%BE%9D%E8%B5%96%E7%9A%84%E8%B5%84%E6%BA%90%E6%96%87%E4%BB%B6%E5%88%B0%E6%9C%AC%E5%9C%B0.md)》文档获取了资源文件）


![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/048%E5%B0%86%E5%8F%AF%E6%89%A7%E8%A1%8C%E6%96%87%E4%BB%B6%E5%92%8C%E4%BE%9D%E8%B5%96%E6%96%87%E4%BB%B6%E6%8B%B7%E8%B4%9D%E8%87%B3nfs.png)

* 步骤5：依赖文件拷贝至Windows的nfs共享路径下后，执行下面的命令，将Windows的nfs共享路径挂载至开发板的mnt目录下

```
mount -o nolock,addr=192.168.200.1 -t nfs 192.168.200.1:/d/nfs /mnt
```

## 6.拷贝mnt目录下的文件至正确的目录下

* 执行下面的命令，拷贝mnt目录下面的ohos_camera_ai_demo至userdata目录，拷贝mnt目录下面的libvb_server.so和 libmpp_vbs.so至/usr/lib/目录下，再将models和aac_file文件夹拷贝至userdata目录下

```
cp /mnt/ohos_camera_ai_demo  /userdata/
cp /mnt/*.so /usr/lib/
cp /mnt/models  /userdata/ -rf
cp /mnt/aac_file  /userdata/ -rf
```

![](https://gitee.com/wgm2022/mypic/raw/master/hispark_taurus_ai_sample/037%E6%8B%B7%E8%B4%9DSD%E5%8D%A1%E4%B8%AD%E7%9A%84%E6%96%87%E4%BB%B6%E8%87%B3%E5%AF%B9%E5%BA%94%E8%B7%AF%E5%BE%84%E4%B8%8B.png)

* 执行下面的命令，给ohos_camera_ai_demo文件可执行权限

```
chmod 777 /userdata/ohos_camera_ai_demo
```





