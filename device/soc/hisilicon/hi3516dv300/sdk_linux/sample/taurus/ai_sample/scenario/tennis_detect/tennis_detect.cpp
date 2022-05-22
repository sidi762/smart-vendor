/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "tennis_detect.h"

#include "sample_comm_nnie.h"
#include "sample_comm_ive.h"
#include "sample_media_ai.h"
#include "vgs_img.h"
#include "misc_util.h"

using namespace std;
using namespace cv;

static IVE_SRC_IMAGE_S pstSrc;
static IVE_DST_IMAGE_S pstDst;
static IVE_CSC_CTRL_S stCscCtrl;

static HI_VOID IveImageParamCfg(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst,
    VIDEO_FRAME_INFO_S *srcFrame)
{
    pstSrc->enType = IVE_IMAGE_TYPE_YUV420SP;
    pstSrc->au64VirAddr[0] = srcFrame->stVFrame.u64VirAddr[0];
    pstSrc->au64VirAddr[1] = srcFrame->stVFrame.u64VirAddr[1];
    pstSrc->au64VirAddr[2] = srcFrame->stVFrame.u64VirAddr[2]; // 2: Image data virtual address

    pstSrc->au64PhyAddr[0] = srcFrame->stVFrame.u64PhyAddr[0];
    pstSrc->au64PhyAddr[1] = srcFrame->stVFrame.u64PhyAddr[1];
    pstSrc->au64PhyAddr[2] = srcFrame->stVFrame.u64PhyAddr[2]; // 2: Image data physical address

    pstSrc->au32Stride[0] = srcFrame->stVFrame.u32Stride[0];
    pstSrc->au32Stride[1] = srcFrame->stVFrame.u32Stride[1];
    pstSrc->au32Stride[2] = srcFrame->stVFrame.u32Stride[2]; // 2: Image data span

    pstSrc->u32Width = srcFrame->stVFrame.u32Width;
    pstSrc->u32Height = srcFrame->stVFrame.u32Height;

    pstDst->enType = IVE_IMAGE_TYPE_U8C3_PACKAGE;
    pstDst->u32Width = pstSrc->u32Width;
    pstDst->u32Height = pstSrc->u32Height;
    pstDst->au32Stride[0] = pstSrc->au32Stride[0];
    pstDst->au32Stride[1] = 0;
    pstDst->au32Stride[2] = 0; // 2: Image data span
}

static HI_S32 yuvFrame2rgb(VIDEO_FRAME_INFO_S *srcFrame, IPC_IMAGE *dstImage)
{
    IVE_HANDLE hIveHandle;
    HI_S32 s32Ret = 0;
    stCscCtrl.enMode = IVE_CSC_MODE_PIC_BT709_YUV2RGB; // IVE_CSC_MODE_VIDEO_BT601_YUV2RGB
    IveImageParamCfg(&pstSrc, &pstDst, srcFrame);

    s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstDst.au64PhyAddr[0], (void **)&pstDst.au64VirAddr[0],
        "User", HI_NULL, pstDst.u32Height*pstDst.au32Stride[0] * 3); // 3: multiple
    if (HI_SUCCESS != s32Ret) {
        HI_MPI_SYS_MmzFree(pstDst.au64PhyAddr[0], (void *)pstDst.au64VirAddr[0]);
        SAMPLE_PRT("HI_MPI_SYS_MmzFree err\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_SYS_MmzFlushCache(pstDst.au64PhyAddr[0], (void *)pstDst.au64VirAddr[0],
        pstDst.u32Height*pstDst.au32Stride[0] * 3); // 3: multiple
    if (HI_SUCCESS != s32Ret) {
        HI_MPI_SYS_MmzFree(pstDst.au64PhyAddr[0], (void *)pstDst.au64VirAddr[0]);
        return s32Ret;
    }
    // 3: multiple
    memset_s((void *)pstDst.au64VirAddr[0], pstDst.u32Height*pstDst.au32Stride[0] * 3,
        0, pstDst.u32Height*pstDst.au32Stride[0] * 3); // 3: multiple
    HI_BOOL bInstant = HI_TRUE;

    s32Ret = HI_MPI_IVE_CSC(&hIveHandle, &pstSrc, &pstDst, &stCscCtrl, bInstant);
    if (HI_SUCCESS != s32Ret) {
        HI_MPI_SYS_MmzFree(pstDst.au64PhyAddr[0], (void *)pstDst.au64VirAddr[0]);
        return s32Ret;
    }

    if (HI_TRUE == bInstant) {
        HI_BOOL bFinish = HI_TRUE;
        HI_BOOL bBlock = HI_TRUE;
        s32Ret = HI_MPI_IVE_Query(hIveHandle, &bFinish, bBlock);
        while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret) {
            usleep(100); // 100: usleep time
            s32Ret = HI_MPI_IVE_Query(hIveHandle, &bFinish, bBlock);
        }
    }
    dstImage->u64PhyAddr = pstDst.au64PhyAddr[0];
    dstImage->u64VirAddr = pstDst.au64VirAddr[0];
    dstImage->u32Width = pstDst.u32Width;
    dstImage->u32Height = pstDst.u32Height;

    return HI_SUCCESS;
}

static HI_S32 frame2Mat(VIDEO_FRAME_INFO_S *srcFrame, Mat &dstMat)
{
    HI_U32 w = srcFrame->stVFrame.u32Width;
    HI_U32 h = srcFrame->stVFrame.u32Height;
    int bufLen = w * h * 3;
    HI_U8 *srcRGB = NULL;
    IPC_IMAGE dstImage;
    if (yuvFrame2rgb(srcFrame, &dstImage) != HI_SUCCESS) {
        SAMPLE_PRT("yuvFrame2rgb err\n");
        return HI_FAILURE;
    }
    srcRGB = (HI_U8 *)dstImage.u64VirAddr;
    dstMat.create(h, w, CV_8UC3);
    memcpy_s(dstMat.data, bufLen * sizeof(HI_U8), srcRGB, bufLen * sizeof(HI_U8));
    HI_MPI_SYS_MmzFree(dstImage.u64PhyAddr, (void *)&(dstImage.u64VirAddr));
    return HI_SUCCESS;
}

HI_S32 tennis_detect::TennisDetectLoad(uintptr_t* model)
{
    HI_S32 ret = 1;
    *model = 1;
    SAMPLE_PRT("TennisDetectLoad success\n");

    return ret;
}

HI_S32 tennis_detect::TennisDetectUnload(uintptr_t model)
{
    model = 0;

    return HI_SUCCESS;
}

HI_S32 tennis_detect::TennisDetectCal(uintptr_t model, VIDEO_FRAME_INFO_S *srcFrm, VIDEO_FRAME_INFO_S *dstFrm)
{
    (void)model;
    int ret = 0;
    RectBox boxs[32] = {0}; // 32: TENNIS_OBJ_MAX
    int j = 0;

    Mat image;
    frame2Mat(srcFrm, image);
    if (image.size == 0) {
        SAMPLE_PRT("image is null\n");
        return HI_FAILURE;
    }

    Mat src = image;
    Mat src1 = src.clone();
    Mat dst, edge, gray, hsv;

    dst.create(src1.size(), src1.type()); // Create a matrix of the same type and size as src (dst)

    // The cvtColor operator is used to convert an image from one color space to another color space
    cvtColor(src1, hsv, COLOR_BGR2HSV); // Convert original image to HSV image

    // Binarize the hsv image, here is to binarize the green background,
    // this parameter can be adjusted according to requirements
    inRange(hsv, Scalar(31, 82, 68), Scalar(65, 248, 255), gray); // 31: B, 82: G, 68:R / 65: B, 248:G, 255:R

    // Use canny operator for edge detection
    // 3: threshold1, 9: threshold2, 3: apertureSize
    Canny(gray, gray, 3, 9, 3);
    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    SAMPLE_PRT("contours.size():%d\n", contours.size());

    for (int i = 0; i < (int)contours.size(); i++) {
        if (contours.size() > 40) { // 40: contours.size() extremes
            continue;
        }

        Rect ret1 = boundingRect(Mat(contours[i]));
        ret1.x -= 5; // 5: x coordinate translation
        ret1.y -= 5; // 5: y coordinate translation
        ret1.width += 10; // 10: Rectangle width plus 10
        ret1.height += 10; // 10: Rectangle height plus 10

        // 20: Rectangle width and height pixel extremes
        if ((ret1.width > 20) && (ret1.height > 20)) {
            boxs[j].xmin = ret1.x * 3; // 3: optimized value
            boxs[j].ymin = (int)(ret1.y * 2.25); // 2.25: optimized value
            boxs[j].xmax = boxs[j].xmin + ret1.width * 3; // 3: optimized value
            boxs[j].ymax = boxs[j].ymin + (int)ret1.height * 2.25; // 2.25: optimized value
            j++;
        }
    }
    // 25: detect boxesNum
    if (j > 0 && j <= 25) {
        SAMPLE_PRT("box num:%d\n", j);
        MppFrmDrawRects(dstFrm, boxs, j, RGB888_RED, 2); // 2: DRAW_RETC_THICK
    }

    return ret;
}