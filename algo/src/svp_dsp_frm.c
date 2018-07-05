#include "hi_dsp.h"
#include "svp_dsp_frm.h"
#include "svp_dsp_def.h"
#include "svp_dsp_tile.h"
#include "svp_dsp_tm.h"
#include "svp_dsp_performance.h"
#include "svp_dsp_trace.h"
#include <string.h>
#include <stdio.h>
//tvl1algo
#include "tvl1flow_lib.c"
#include <time.h>
#include <math.h>
#define PAR_DEFAULT_OUTFLOW "flow.flo"
#define PAR_DEFAULT_NPROC   0
#define PAR_DEFAULT_TAU     0.25
#define PAR_DEFAULT_LAMBDA  0.15
#define PAR_DEFAULT_THETA   0.3
#define PAR_DEFAULT_NSCALES 100
#define PAR_DEFAULT_ZFACTOR 0.5
#define PAR_DEFAULT_NWARPS  5
#define PAR_DEFAULT_EPSILON 0.01
#define PAR_DEFAULT_VERBOSE 0
#define SVP_DSP_STAT_PERF 1

/*statistic performace*/
#if SVP_DSP_STAT_PERF

#define SVP_DSP_STAT_PERF_DECLARE() HI_U32 u32CycleStart, u32CycleStop;

#define  SVP_DSP_STAT_PERF_FILL(taskId,tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth)\
{\
    s_astStatPerf[(taskId)].u32TileWidth = (tileWidth);\
    s_astStatPerf[(taskId)].u32TileHeight = (tileHeight);\
    s_astStatPerf[(taskId)].u32ImageWidth = (width);\
    s_astStatPerf[(taskId)].u32ImageHeight = (height);\
    s_astStatPerf[(taskId)].u32TileCount = (tileCount);\
    s_astStatPerf[(taskId)].u32TileTotalCycle = (tileTotalCycle);\
    s_astStatPerf[(taskId)].u32InTotalBandWidth = (InTotalBandWidth);\
    s_astStatPerf[(taskId)].u32OutTotalBandWidth = (OutTotalBandWidth);\
}
#define SVP_DSP_STAT_PERF_INC_TILE_COUNT(taskId)\
{\
    s_astStatPerf[(taskId)].u32TileCount++;\
    SVP_DSP_TIME_STAMP(u32CycleStart);\
}
#define SVP_DSP_STAT_PERF_CALC_CYCLE(taskId)\
{\
    SVP_DSP_TIME_STAMP(u32CycleStop);\
    s_astStatPerf[(taskId)].u32TileTotalCycle += (u32CycleStop - u32CycleStart);\
}
#define SVP_DSP_STAT_IN_BANDWIDTH_CALC(taskId, TileWidth, TileHeight, ElementSize)\
{\
    s_astStatPerf[(taskId)].u32InTotalBandWidth += (TileWidth)*(TileHeight)*(ElementSize);\
}

#define SVP_DSP_STAT_OUT_BANDWIDTH_CALC(taskId, TileWidth, TileHeight, ElementSize)\
{\
    s_astStatPerf[(taskId)].u32OutTotalBandWidth += (TileWidth)*(TileHeight)*(ElementSize);\
}

/*Erode*/
#define SVP_DSP_STAT_PERF_INIT_ERODE(tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth) SVP_DSP_STAT_PERF_FILL(SVP_DSP_TASK_ERODE,tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth)
#define SVP_DSP_STAT_PERF_BEGIN_ERODE() SVP_DSP_STAT_PERF_INC_TILE_COUNT(SVP_DSP_TASK_ERODE)
#define SVP_DSP_STAT_PERF_END_ERODE() SVP_DSP_STAT_PERF_CALC_CYCLE(SVP_DSP_TASK_ERODE)

/*Dilate*/
#define SVP_DSP_STAT_PERF_INIT_DILATE(tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth) SVP_DSP_STAT_PERF_FILL(SVP_DSP_TASK_DILATE,tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth)
#define SVP_DSP_STAT_PERF_BEGIN_DILATE() SVP_DSP_STAT_PERF_INC_TILE_COUNT(SVP_DSP_TASK_DILATE)
#define SVP_DSP_STAT_PERF_END_DILATE() SVP_DSP_STAT_PERF_CALC_CYCLE(SVP_DSP_TASK_DILATE)

static SVP_DSP_PERF_S s_astStatPerf[SVP_DSP_TASK_BUTT];
#else
#define SVP_DSP_STAT_PERF_DECLARE()
/*Erode*/
#define SVP_DSP_STAT_PERF_INIT_ERODE(tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth)
#define SVP_DSP_STAT_PERF_BEGIN_ERODE()
#define SVP_DSP_STAT_PERF_END_ERODE()

/*Dilate*/
#define SVP_DSP_STAT_PERF_INIT_DILATE(tileWidth,tileHeight,width,height,tileCount,tileTotalCycle,InTotalBandWidth, OutTotalBandWidth)
#define SVP_DSP_STAT_PERF_BEGIN_DILATE()
#define SVP_DSP_STAT_PERF_END_DILATE()

#endif

/*****************************************************************************
*   Prototype    : SVP_DSP_FRM_Init
*   Description  : DSP frame and tile manager init.
*   Parameters   : HI_VOID
*
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*
*   History:
*
*       1.  Date         : 2017-03-31
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_FRM_Init(HI_VOID)
{
    return SVP_DSP_TM_Init();
}

/*****************************************************************************
*   Prototype    : SVP_DSP_FRM_Exit
*   Description  : DSP  frame and tile manager exit.
*   Parameters   : HI_VOID
*
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*
*   History:
*
*       1.  Date         : 2017-03-31
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_FRM_Exit(HI_VOID)
{
    return SVP_DSP_TM_Exit();
}

/*****************************************************************************
*   Prototype    : SVP_DSP_Erode_3x3_U8_U8_Frm
*   Description  : Erodes a frame taking maximum of pixel neighborhood with 3x3 filter.
*   Parameters   : SVP_DSP_SRC_FRAME_S *pstSrc          Input source data. Only the U8C1 input format is supported.
*                  SVP_DSP_DST_FRAME_S *pstDst          Output result.

*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :

*
*
*   History:
*
*       1.  Date         : 2017-03-31
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_Erode_3x3_U8_U8_Frm(SVP_DSP_SRC_FRAME_S* pstSrc, SVP_DSP_DST_FRAME_S* pstDst)
{
    SVP_DSP_STAT_PERF_DECLARE()
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32PingPongFlag = 0;
    HI_S32 s32InIndX = 0, s32InIndY = 0;
    HI_S32 s32OutIndX = 0, s32OutIndY = 0;
    HI_S32 s32TmpWidth, s32TmpHeight;
    HI_S32 s32Width;
    HI_S32 s32Height;
    HI_U32 i = 0, j = 0;
    HI_U32 u32TileWidth = SVP_DSP_ERODE_TILE_WIDTH;
    HI_U32 u32TileHeight = SVP_DSP_ERODE_TILE_HEIGHT;
    HI_U32 u32EdgeExt = 1;

    SVP_DSP_FRAME_S* apstFrm[SVP_DSP_ERODE_FRAME_NUM];
    // Source and destination tiles. Will be working in ping pong mode.
    SVP_DSP_TILE_S* apstInTile[SVP_DSP_ERODE_IN_TILE_NUM ], *apstOutTile[SVP_DSP_ERODE_OUT_TILE_NUM];
    // Data buffer pointers for source and destination tiles
    HI_VOID* apvInTileBuff[SVP_DSP_ERODE_IN_TILE_BUFF_NUM ];
    HI_VOID* apvOutTileBuff[SVP_DSP_ERODE_OUT_TILE_BUFF_NUM];
    //输入图像的大小
    s32Width = pstSrc->s32FrameWidth;
    s32Height = pstSrc->s32FrameHeight;
    //输入和输出图像
    apstFrm[0] = pstSrc;
    apstFrm[1] = pstDst;
    //u32TileWidth取SVP_DSP_ERODE_TILE_WIDTH，s32Width（输入图像）中小的那个。
    u32TileWidth  = SVP_DSP_CLIP(s32Width, SVP_DSP_ERODE_TILE_WIDTH, s32Width);
    u32TileHeight = SVP_DSP_CLIP(s32Height, SVP_DSP_ERODE_TILE_HEIGHT, s32Height);
    //allocate buffer
    s32Ret = SVP_DSP_AllocateBuffers(apvInTileBuff, SVP_DSP_ERODE_IN_TILE_BUFF_NUM , (u32TileWidth + 2 * u32EdgeExt) * (u32TileHeight + 2 * u32EdgeExt), XV_MEM_BANK_COLOR_0, 64);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_1, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
    s32Ret = SVP_DSP_AllocateBuffers(apvOutTileBuff, SVP_DSP_ERODE_OUT_TILE_BUFF_NUM , u32TileWidth * u32TileHeight, XV_MEM_BANK_COLOR_1, 64);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_2, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

    s32Ret = SVP_DSP_AllocateTiles(apstInTile, SVP_DSP_ERODE_IN_TILE_NUM);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_3, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
    s32Ret = SVP_DSP_AllocateTiles(apstOutTile, SVP_DSP_ERODE_OUT_TILE_NUM);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_4, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

    SVP_DSP_STAT_PERF_INIT_ERODE(u32TileWidth, u32TileHeight, s32Width, s32Height, 0, 0, 0, 0)

    s32PingPongFlag = 0;
    s32InIndX = 0;
    s32InIndY = 0;

    if ((s32Height >= SVP_DSP_ERODE_TILE_HEIGHT) && (s32Width >= SVP_DSP_ERODE_TILE_WIDTH))
    {
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE_X_TO_Y(s32InIndX, s32InIndY, u32TileWidth, u32TileHeight, s32Width, s32Height);
        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of second tile into pong buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE_X_TO_Y(s32InIndX, s32InIndY, u32TileWidth, u32TileHeight, s32Width, s32Height);
        s32OutIndX = 0;
        s32OutIndY = 0;
        s32TmpWidth = s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        s32TmpHeight = s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT;

        for (i = 0; i < s32TmpHeight; i += u32TileHeight)
        {
            for (j = 0; j < s32TmpWidth; j += u32TileWidth)
            {
                SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);
                SVP_DSP_STAT_PERF_BEGIN_ERODE()
                s32Ret =  SVP_DSP_Erode_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Erode_3x3_Const process failed!\n", s32Ret);

                SVP_DSP_STAT_PERF_END_ERODE()

                SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
                SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
                s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                if ((0 != s32InIndX) || (0 != s32InIndY))
                {
                    // Initiate transfer for next input tile
                    SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
                    SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);
                    s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
                    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                    SVP_DSP_MOVE_X_TO_Y(s32InIndX, s32InIndY, u32TileWidth, u32TileHeight, s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH, s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT);
                }

                SVP_DSP_MOVE_X_TO_Y(s32OutIndX, s32OutIndY, u32TileWidth, u32TileHeight, s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH, s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT);

                // flip the ping pong flag
                s32PingPongFlag = (s32PingPongFlag ^ 0x1);
            }

            SVP_DSP_WaitForTile(apstOutTile[(s32PingPongFlag ^ 0x1)]);
        }
    }

    //y direction
    if ((s32Height % SVP_DSP_ERODE_TILE_HEIGHT != 0) && (s32Width >= SVP_DSP_ERODE_TILE_WIDTH))
    {
        u32TileHeight = s32Height % SVP_DSP_ERODE_TILE_HEIGHT;

        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], u32TileWidth, u32TileHeight,  SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        //s32PingPongFlag = 0;
        s32InIndX = 0;
        s32InIndY = s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT;

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE(s32InIndX, u32TileWidth);

        if ((s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH) != s32InIndX)
        {
            SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
            SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

            // Initiate data transfer of second tile into pong buffer
            s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
            SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
            SVP_DSP_MOVE(s32InIndX, u32TileWidth);
        }

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);

        s32OutIndX = 0;
        s32OutIndY = s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT;
        s32TmpHeight = s32OutIndY;
        s32TmpWidth = s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH;

        for (i = s32TmpHeight; i < s32Height; i += u32TileHeight)
        {
            for (j = 0; j < s32TmpWidth; j += u32TileWidth)
            {
                SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);

                SVP_DSP_STAT_PERF_BEGIN_ERODE()

                s32Ret =  SVP_DSP_Erode_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Erode_3x3 process failed!\n", s32Ret);

                SVP_DSP_STAT_PERF_END_ERODE()

                SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
                SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
                s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                if ((s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH) != s32InIndX/* || (0 != s32InIndY)*/)
                {
                    // Initiate transfer for next input tile
                    SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
                    SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);
                    s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
                    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                    SVP_DSP_MOVE(s32InIndX, u32TileWidth);
                }

                SVP_DSP_MOVE(s32OutIndX, u32TileWidth);
                // flip the ping pong flag
                s32PingPongFlag = (s32PingPongFlag ^ 0x1);
            }
        }
        //SVP_DSP_WaitForTile(apstOutTile[(s32PingPongFlag ^ 0x1)]);
    }

    if ((s32Width % SVP_DSP_ERODE_TILE_WIDTH != 0) && (s32Height >= SVP_DSP_ERODE_TILE_HEIGHT))
    {
        u32TileWidth = s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        u32TileHeight =  SVP_DSP_CLIP(s32Height, SVP_DSP_ERODE_TILE_HEIGHT, s32Height);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        //s32PingPongFlag = 0;
        s32InIndX = s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        s32InIndY = 0;

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE(s32InIndY, u32TileHeight);

        if ((s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT) != s32InIndY)
        {
            SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
            SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

            // Initiate data transfer of second tile into pong buffer
            s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
            SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
            SVP_DSP_MOVE(s32InIndY, u32TileHeight);
        }

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        //SVP_MOVE_X_TO_Y_XEDGE(s32InIndX,s32InIndY,u32TileWidth,u32TileHeight,s32Width,s32Height);
        s32OutIndX = s32Width - s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        s32OutIndY = 0;

        s32TmpHeight = s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT;
        s32TmpWidth = s32OutIndX;

        for (i = 0; i < s32TmpHeight; i += u32TileHeight)
        {
            for (j = s32TmpWidth; j < s32Width; j += u32TileWidth)
            {
                SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);
                SVP_DSP_STAT_PERF_BEGIN_ERODE()

                s32Ret =  SVP_DSP_Erode_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#):Erode_3x3 process failed!\n", s32Ret);
                SVP_DSP_STAT_PERF_END_ERODE()

                SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
                SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
                s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                //SVP_MOVE_X_TO_Y_XEDGE(s32InIndX,s32InIndY,u32TileWidth,u32TileHeight,s32Width,s32Height);
                if ((s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT) != s32InIndY)
                {
                    // Initiate transfer for next input tile
                    SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
                    SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);
                    s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
                    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
                    SVP_DSP_MOVE(s32InIndY, u32TileHeight);
                }

                SVP_DSP_MOVE(s32OutIndY, u32TileHeight);
                // flip the ping pong flag
                s32PingPongFlag = (s32PingPongFlag ^ 0x1);
            }
        }
        //SVP_DSP_WaitForTile(apstOutTile[(s32PingPongFlag ^ 0x1)]);
    }

    //last one tile
    if ((s32Height % SVP_DSP_ERODE_TILE_HEIGHT != 0) && (s32Width % SVP_DSP_ERODE_TILE_WIDTH != 0))
    {
        u32TileWidth  = s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        u32TileHeight = s32Height % SVP_DSP_ERODE_TILE_HEIGHT;

        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0], u32TileWidth, u32TileHeight,  SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1], u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        //s32PingPongFlag = 0;
        s32InIndX = s32Width  - s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        s32InIndY = s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT;

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32OutIndX = s32Width  - s32Width % SVP_DSP_ERODE_TILE_WIDTH;
        s32OutIndY = s32Height - s32Height % SVP_DSP_ERODE_TILE_HEIGHT;

        SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);
        SVP_DSP_STAT_PERF_BEGIN_ERODE()

        s32Ret =  SVP_DSP_Erode_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Erode_3x3 process failed!\n", s32Ret);
        SVP_DSP_STAT_PERF_END_ERODE()

        SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
        s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        SVP_DSP_WaitForTile(apstOutTile[s32PingPongFlag]);
    }

    //free out tiles
FAIL_5:
    (HI_VOID)SVP_DSP_FreeTiles(apstOutTile, SVP_DSP_ERODE_OUT_TILE_NUM);
FAIL_4:
    (HI_VOID)SVP_DSP_FreeTiles(apstInTile, SVP_DSP_ERODE_IN_TILE_NUM);
FAIL_3:
    (HI_VOID)SVP_DSP_FreeBuffers(apvOutTileBuff, SVP_DSP_ERODE_OUT_TILE_BUFF_NUM);
FAIL_2:
    (HI_VOID)SVP_DSP_FreeBuffers(apvInTileBuff, SVP_DSP_ERODE_IN_TILE_BUFF_NUM);
    //free frame
FAIL_1:
#if 0
    (HI_VOID)SVP_DSP_FreeFrames(apstFrm, SVP_DSP_ERODE_FRAME_NUM);
FAIL_0:
#endif
    return s32Ret;
}

/*****************************************************************************
*   Prototype    : SVP_DSP_Dilate_3x3_U8_U8_Frm
*   Description  : Dilates a frame taking maximum of pixel neighborhood with 3x3 filter.
*   Parameters   : SVP_DSP_SRC_FRAME_S *pstSrc          Input source data. Only the U8C1 input format is supported.
*                  SVP_DSP_DST_FRAME_S *pstDst          Output result.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*
*   History:
*
*       1.  Date         : 2017-03-31
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_Dilate_3x3_U8_U8_Frm(SVP_DSP_SRC_FRAME_S* pstSrc1, SVP_DSP_SRC_FRAME_S* pstSrc2, SVP_DSP_DST_FRAME_S* pstDst)
{
    SVP_DSP_STAT_PERF_DECLARE()
    
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32PingPongFlag = 0;
    HI_S32 s32InIndX = 0, s32InIndY = 0;
    HI_S32 s32OutIndX = 0, s32OutIndY = 0;
    HI_U32 s32Width;
    HI_U32 s32Height;
    HI_S32 s32TmpWidth, s32TmpHeight;
    HI_U32 i = 0, j = 0;
    HI_U32 u32TileWidth = SVP_DSP_DILATE_TILE_WIDTH;
    HI_U32 u32TileHeight = SVP_DSP_DILATE_TILE_HEIGHT;
    HI_U32 u32EdgeExt = 1;
    
    SVP_DSP_FRAME_S* apstFrm[SVP_DSP_DILATE_FRAME_NUM];
    // Source and destination tiles. Will be working in ping pong mode.
    SVP_DSP_TILE_S* apstInTile[SVP_DSP_DILATE_IN_TILE_NUM ], *apstOutTile[SVP_DSP_DILATE_OUT_TILE_NUM];
    // Data buffer pointers for source and destination tiles
    HI_VOID* apvInTileBuff[SVP_DSP_DILATE_IN_TILE_BUFF_NUM];
    HI_VOID* apvOutTileBuff[SVP_DSP_DILATE_OUT_TILE_BUFF_NUM];

    s32Width = pstSrc->s32FrameWidth;
    s32Height = pstSrc->s32FrameHeight;

    apstFrm[0] = pstSrc;
    apstFrm[1] = pstDst;

    u32TileWidth  = SVP_DSP_CLIP( s32Width, SVP_DSP_DILATE_TILE_WIDTH, s32Width);
    u32TileHeight = SVP_DSP_CLIP(s32Height, SVP_DSP_DILATE_TILE_HEIGHT, s32Height);

    //allocate buffer
    s32Ret = SVP_DSP_AllocateBuffers(apvInTileBuff, SVP_DSP_DILATE_IN_TILE_BUFF_NUM ,\
        (u32TileWidth + 2 * u32EdgeExt) * (u32TileHeight + 2 * u32EdgeExt), XV_MEM_BANK_COLOR_0, 64);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_1, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
    s32Ret = SVP_DSP_AllocateBuffers(apvOutTileBuff, SVP_DSP_DILATE_OUT_TILE_BUFF_NUM ,\
        u32TileWidth * u32TileHeight, XV_MEM_BANK_COLOR_1, 64);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_2, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

    s32Ret = SVP_DSP_AllocateTiles(apstInTile, SVP_DSP_DILATE_IN_TILE_NUM);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_3, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
    s32Ret = SVP_DSP_AllocateTiles(apstOutTile, SVP_DSP_DILATE_OUT_TILE_NUM);
    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_4, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

    SVP_DSP_STAT_PERF_INIT_DILATE(u32TileWidth, u32TileHeight, s32Width, s32Height, 0, 0, 0, 0)

    s32PingPongFlag = 0;
    s32InIndX = 0;
    s32InIndY = 0;

    if (s32Height >= SVP_DSP_DILATE_TILE_HEIGHT && s32Width >= SVP_DSP_DILATE_TILE_WIDTH )
    {
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE_X_TO_Y(s32InIndX, s32InIndY, u32TileWidth, u32TileHeight, s32Width, s32Height);
        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of second tile into pong buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE_X_TO_Y(s32InIndX, s32InIndY, u32TileWidth, u32TileHeight, s32Width, s32Height);
        s32OutIndX = 0;
        s32OutIndY = 0;
        s32TmpWidth = s32Width - s32Width % SVP_DSP_DILATE_TILE_WIDTH;
        s32TmpHeight = s32Height - s32Height % SVP_DSP_DILATE_TILE_HEIGHT;

        for (i = 0; i < s32TmpHeight; i += u32TileHeight)
        {
            for (j = 0; j < s32TmpWidth; j += u32TileWidth)
            {
                SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);

                SVP_DSP_STAT_PERF_BEGIN_DILATE()
                s32Ret =  SVP_DSP_Dilate_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Dilate_3x3 process failed!\n", s32Ret);

                SVP_DSP_STAT_PERF_END_DILATE()

                SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
                SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
                s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                if ((0 != s32InIndX) || (0 != s32InIndY))
                {
                    // Initiate transfer for next input tile
                    SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
                    SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);
                    s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
                    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                    SVP_DSP_MOVE_X_TO_Y(s32InIndX, s32InIndY, u32TileWidth, u32TileHeight,\
                        s32Width - s32Width % SVP_DSP_DILATE_TILE_WIDTH, s32Height - s32Height % SVP_DSP_DILATE_TILE_HEIGHT);
                }

                SVP_DSP_MOVE_X_TO_Y(s32OutIndX, s32OutIndY, u32TileWidth, u32TileHeight,\
                    s32Width - s32Width % SVP_DSP_DILATE_TILE_WIDTH, s32Height - s32Height % SVP_DSP_DILATE_TILE_HEIGHT);
                // flip the ping pong flag
                s32PingPongFlag = (s32PingPongFlag ^ 0x1);
            }

            SVP_DSP_WaitForTile(apstOutTile[(s32PingPongFlag ^ 0x1)]);
        }
    }


    //y direction
    if ((s32Height % SVP_DSP_DILATE_TILE_HEIGHT != 0) &&  (s32Width >= SVP_DSP_DILATE_TILE_WIDTH) )
    {
        u32TileHeight = s32Height % SVP_DSP_DILATE_TILE_HEIGHT;

        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        //s32PingPongFlag = 0;
        s32InIndX = 0;
        s32InIndY = s32Height - s32Height % SVP_DSP_DILATE_TILE_HEIGHT;

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE(s32InIndX, u32TileWidth);

        if ((s32Width - (s32Width % SVP_DSP_DILATE_TILE_WIDTH)) != s32InIndX)
        {
            SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
            SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

            // Initiate data transfer of second tile into pong buffer
            s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
            SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

            SVP_DSP_MOVE(s32InIndX, u32TileWidth);
        }

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        //SVP_MOVE_X_TO_Y_YEDGE(s32InIndX,s32InIndY,u32TileWidth,u32TileHeight,s32Width,s32Height);
        s32OutIndX = 0;
        s32OutIndY = s32Height - (s32Height % SVP_DSP_DILATE_TILE_HEIGHT);
        s32TmpHeight = s32OutIndY;
        s32TmpWidth = s32Width - (s32Width % SVP_DSP_DILATE_TILE_WIDTH);

        for (i = s32TmpHeight; i < s32Height; i += u32TileHeight)
        {
            for (j = 0; j < s32TmpWidth; j += u32TileWidth)
            {
                SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);
                SVP_DSP_STAT_PERF_BEGIN_DILATE()
                s32Ret =  SVP_DSP_Dilate_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Dilate_3x3 process failed!\n", s32Ret);
                SVP_DSP_STAT_PERF_END_DILATE()

                SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
                SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
                s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                if ((s32Width - (s32Width % SVP_DSP_DILATE_TILE_WIDTH)) != s32InIndX/* || (0 != s32InIndY)*/)
                {
                    // Initiate transfer for next input tile
                    SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
                    SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);
                    s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
                    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
                    SVP_DSP_MOVE(s32InIndX, u32TileWidth);
                }

                SVP_DSP_MOVE(s32OutIndX, u32TileWidth);
                // flip the ping pong flag
                s32PingPongFlag = (s32PingPongFlag ^ 0x1);
            }
        }

        //SVP_DSP_WaitForTile(apstOutTile[(s32PingPongFlag ^ 0x1)]);
    }

    if ((s32Width % SVP_DSP_DILATE_TILE_WIDTH) != 0 && (s32Height >= SVP_DSP_DILATE_TILE_HEIGHT))
    {
        u32TileWidth = s32Width % SVP_DSP_DILATE_TILE_WIDTH;
        u32TileHeight = SVP_DSP_CLIP(s32Height, SVP_DSP_DILATE_TILE_HEIGHT , s32Height);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);

        //s32PingPongFlag = 0;
        s32InIndX = s32Width - (s32Width % SVP_DSP_DILATE_TILE_WIDTH);
        s32InIndY = 0;

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        SVP_DSP_MOVE(s32InIndY, u32TileHeight);

        if ((s32Height - (s32Height % SVP_DSP_DILATE_TILE_HEIGHT)) != s32InIndY)
        {
            SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
            SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

            // Initiate data transfer of second tile into pong buffer
            s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
            SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
            SVP_DSP_MOVE(s32InIndY, u32TileHeight);
        }

        s32PingPongFlag = (s32PingPongFlag ^ 0x1);
        //SVP_MOVE_X_TO_Y_XEDGE(s32InIndX,s32InIndY,u32TileWidth,u32TileHeight,s32Width,s32Height);
        s32OutIndX = s32Width - (s32Width % SVP_DSP_DILATE_TILE_WIDTH);
        s32OutIndY = 0;

        s32TmpHeight = s32Height - (s32Height % SVP_DSP_DILATE_TILE_HEIGHT);
        s32TmpWidth = s32OutIndX;

        for (i = 0; i < s32TmpHeight; i += u32TileHeight)
        {
            for (j = s32TmpWidth; j < s32Width; j += u32TileWidth)
            {
                SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);
                SVP_DSP_STAT_PERF_BEGIN_DILATE()
                s32Ret =  SVP_DSP_Dilate_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Dilate_3x3 process failed!\n", s32Ret);
                SVP_DSP_STAT_PERF_END_DILATE()
                SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
                SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
                s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
                SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

                //SVP_MOVE_X_TO_Y_XEDGE(s32InIndX,s32InIndY,u32TileWidth,u32TileHeight,s32Width,s32Height);
                if ((s32Height - (s32Height % SVP_DSP_DILATE_TILE_HEIGHT)) != s32InIndY)
                {
                    // Initiate transfer for next input tile
                    SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
                    SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);
                    s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
                    SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());
                    SVP_DSP_MOVE(s32InIndY, u32TileHeight);
                }

                SVP_DSP_MOVE(s32OutIndY, u32TileHeight);
                // flip the ping pong flag
                s32PingPongFlag = (s32PingPongFlag ^ 0x1);
            }
        }

        //SVP_DSP_WaitForTile(apstOutTile[(s32PingPongFlag ^ 0x1)]);
    }

    //last one tile
    if ((s32Height % SVP_DSP_DILATE_TILE_HEIGHT) != 0 && (s32Width % SVP_DSP_DILATE_TILE_WIDTH) != 0)
    {
        u32TileWidth  = s32Width % SVP_DSP_DILATE_TILE_WIDTH;
        u32TileHeight = s32Height % SVP_DSP_DILATE_TILE_HEIGHT;

        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[0], apvInTileBuff[0], apstFrm[0],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstInTile[1], apvInTileBuff[1], apstFrm[0], \
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, u32EdgeExt, u32EdgeExt, 0, 0);

        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[0], apvOutTileBuff[0], apstFrm[1],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
        SVP_DSP_SETUP_TILE_BY_TYPE(apstOutTile[1], apvOutTileBuff[1], apstFrm[1],\
            u32TileWidth, u32TileHeight, SVP_DSP_TILE_U8, 0, 0, 0, 0);
SVP_DSP_DILATE_FRAME_NUM
SVP_DSP_DILATE_FRAME_NUM
SVP_DSP_DILATE_FRAME_NUMidth % SVP_DSP_DILATE_TILE_WIDTH;
SVP_DSP_DILATE_FRAME_NUMeight % SVP_DSP_DILATE_TILE_HEIGHT;

        SVP_DSP_TILE_SET_X_COORD(apstInTile[s32PingPongFlag], s32InIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstInTile[s32PingPongFlag], s32InIndY);

        // Initiate data transfer of first tile into ping buffer
        s32Ret = SVP_DSP_ReqTileTransferIn(apstInTile[s32PingPongFlag], NULL, SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        s32OutIndX = s32Width  - s32Width % SVP_DSP_DILATE_TILE_WIDTH;
        s32OutIndY = s32Height - s32Height % SVP_DSP_DILATE_TILE_HEIGHT;

        SVP_DSP_WaitForTile(apstInTile[s32PingPongFlag]);
        SVP_DSP_STAT_PERF_BEGIN_DILATE()
        s32Ret =  SVP_DSP_Dilate_3x3_U8_U8_Const(apstInTile[s32PingPongFlag], apstOutTile[s32PingPongFlag]);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error(%#x):Dilate_3x3 process failed!\n", s32Ret);
        SVP_DSP_STAT_PERF_END_DILATE()
        SVP_DSP_TILE_SET_X_COORD(apstOutTile[s32PingPongFlag], s32OutIndX);
        SVP_DSP_TILE_SET_Y_COORD(apstOutTile[s32PingPongFlag], s32OutIndY);
        s32Ret = SVP_DSP_ReqTileTransferOut(apstOutTile[s32PingPongFlag], SVP_DSP_INT_ON_COMPLETION);
        SVP_DSP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_5, HI_DBG_ERR, "Error:%s\n", SVP_DSP_GetErrorInfo());

        SVP_DSP_WaitForTile(apstOutTile[s32PingPongFlag]);
    }

    //free out tiles
FAIL_5:
    (HI_VOID)SVP_DSP_FreeTiles(apstOutTile, SVP_DSP_DILATE_OUT_TILE_NUM);
FAIL_4:
    (HI_VOID)SVP_DSP_FreeTiles(apstInTile, SVP_DSP_DILATE_IN_TILE_NUM);
FAIL_3:
    (HI_VOID)SVP_DSP_FreeBuffers(apvOutTileBuff, SVP_DSP_DILATE_OUT_TILE_BUFF_NUM);
FAIL_2:
    (HI_VOID)SVP_DSP_FreeBuffers(apvInTileBuff, SVP_DSP_DILATE_IN_TILE_BUFF_NUM);
    //free frame
FAIL_1:

#if 0
    (HI_VOID)SVP_DSP_FreeFrames(apstFrm, SVP_DSP_DILATE_FRAME_NUM);
FAIL_0:
#endif
    return s32Ret;
}

/*****************************************************************************
*   Prototype    : SVP_DSP_GetPerf
*   Description  : Get DSP performance.
*   Parameters   : SVP_DSP_TASK_TYPE_E  enType        Input task id. The algorithm need to get performance
*				     SVP_DSP_PERF_S      *pstPerf       Output performance structure. To store performance data
*
*   Return Value :
*   Spec         :
*
*
*   History:
*
*       1.  Date         : 2017-03-31
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_VOID SVP_DSP_GetPerf(SVP_DSP_TASK_TYPE_E enType, SVP_DSP_PERF_S* pstPerf)
{
#if SVP_DSP_STAT_PERF

    if ((enType < SVP_DSP_TASK_BUTT) && (enType >= SVP_DSP_TASK_ERODE))
    {
        memcpy(pstPerf, &s_astStatPerf[enType], sizeof(*pstPerf));
    }

#endif
}

/*****************************************************************************
*	Prototype	 : SVP_DSP_CopyData
*	Description  : Transfer data between external and local memories.
*	Parameters	 : HI_VOID *pvSrc		   Input source data address.
*				   HI_VOID *pvDst		   Output destination address.
*				   HI_S32 s32Size		  Data size
*
*	Return Value : HI_SUCCESS: Success;Error codes: .
*	Spec		 :
*	History:
*
*		1.	Date		 : 2017-03-31
*			Author		 :
*			Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_CopyData(HI_VOID* pvDst, HI_VOID* pvSrc, HI_S32 s32Size)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SVP_DSP_CopyDataByIdma(pvDst, pvSrc, s32Size, SVP_DSP_INT_ON_COMPLETION);

    return s32Ret;
}


/*****************************************************************************
*   Prototype    : SVP_DSP_Tvl1Flow_Frm
*   Description  : Dilates a frame taking maximum of pixel neighborhood with 3x3 filter.
*   Parameters   : SVP_DSP_SRC_FRAME_S *pstSrc          Input source data. Only the U8C1 input format is supported.
*                  SVP_DSP_DST_FRAME_S *pstDst          Output result.
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*
*   History:
*
*       1.  Date         : 2018-07-03
*           Author       : WANG
*           Modification : Created function
*
*****************************************************************************/

HI_S32 SVP_DSP_Tvl1Flow_Frm(SVP_DSP_SRC_FRAME_S* pstSrc1,SVP_DSP_SRC_FRAME_S* pstSrc2, SVP_DSP_DST_FRAME_S* pstDst)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SVP_DSP_FRAME_S* apstFrm[SVP_DSP_TVL1_FRAME_NUM];
    apstFrm[0] = pstSrc1;
    apstFrm[1] = pstSrc2;
    apstFrm[2] = pstDst;
    HI_S32 nx = apstFrm[0]->s32FrameWidth;
    HI_S32 ny = apstFrm[0]->s32FrameHeight;
    HI_S32 nx2 = apstFrm[1]->s32FrameWidth;
    HI_S32 ny2 = apstFrm[1]->s32FrameHeight;
    HI_FLOAT *I0 = apstFrm[0]->pvFrameBuff;
    HI_FLOAT *I1 = apstFrm[1]->pvFrameBuff;
	if (nx == nx2 && ny == ny2)
	{
		//Set the number of scales according to the size of the
		//images.  The value N is computed to assure that the smaller
		//images of the pyramid don't have a size smaller than 16x16
		const float N = 1 + log(hypot(nx, ny)/16.0) / log(1/zfactor);
		if (N < nscales)
			nscales = N;

		if (verbose)
			fprintf(stderr,
				"nproc=%d tau=%f lambda=%f theta=%f nscales=%d "
				"zfactor=%f nwarps=%d epsilon=%g\n",
				nproc, tau, lambda, theta, nscales,
				zfactor, nwarps, epsilon);

		//allocate memory for the flow
		float *u = xmalloc(2 * nx * ny * sizeof*u);
		float *v = u + nx*ny;;

		//compute the optical flow
		Dual_TVL1_optic_flow_multiscale(
				I0, I1, u, v, nx, ny, tau, lambda, theta,
				nscales, zfactor, nwarps, epsilon, verbose
		);

		//save the optical flow
        
        float *rdata = malloc(nx*ny*2*sizeof*rdata);
        for(int l = 0;l<2;l++){
            for(int i = 0;i<nx*ny;i++){
                rdata[2*i+l]=u[nx*ny*l+i;]
            }
        }
        
        apstFrm[2]->pvFrameBuff = rdata;
        apstFrm[2]->s32FrameHeight = ny;
        apstFrm[2]->s32FrameWidth = nx;
		//delete allocated memory
        free(rdata);
		free(I0);
		free(I1);
		free(u);
	} else {
		fprintf(stderr, "ERROR: input images size mismatch "
				"%dx%d != %dx%d\n", nx, ny, nx2, ny2);
		return EXIT_FAILURE;
	}

}
