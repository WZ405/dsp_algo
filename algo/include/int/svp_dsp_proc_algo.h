/******************************************************************************
 
  Copyright (C), 2001-2017, Hisilicon Tech. Co., Ltd.
 
 ******************************************************************************
  File Name     : svp_dsp_proc_algo.h
  Version       : Initial Draft
  Author        : 
  Created       : 2017/10/31
  Description   : VDSP process algorithm header file
  History       : 
  1.Date        : 2017/10/31
    Authr       : 
    Modification: Created file 
 ******************************************************************************/

#ifndef __SVP_DSP_PROC_ALGO_H__
#define __SVP_DSP_PROC_ALGO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*****************************************************************************
*   Prototype    : SVP_DSP_ProcessDilate3x3
*   Description  : Dilate 3x3 process
*   Input        : HI_U64     u64IdmaOffset   IDMA Address offset
*                  HI_U64     u64Body         Body
*                  HI_U32     u32BodyLen      Body Len
*
*   Output       : 
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : 
*   Calls        : 
*   Called By    : 
*   History:
* 
*       1.  Date         : 2017-11-01
*           Author       : 
*           Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_ProcessDilate3x3(HI_U64 u64IdmaOffset,HI_U64 u64Body,HI_U32 u32BodyLen);

/*****************************************************************************
*   Prototype    : SVP_DSP_ProcessErode3x3
*   Description  : Erode 3x3 process
*   Input        : HI_U64     u64IdmaOffset   IDMA Address offset
*                  HI_U64     u64Body         Body
*                  HI_U32     u32BodyLen      Body Len
*
*   Output       : 
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         : 
*   Calls        : 
*   Called By    : 
*   History:
* 
*       1.  Date         : 2017-11-01
*           Author       : 
*           Modification : Created function
*
*****************************************************************************/
HI_S32 SVP_DSP_ProcessErode3x3(HI_U64 u64IdmaOffset,HI_U64 u64Body,HI_U32 u32BodyLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SVP_DSP_PROC_ALGO_H__ */
