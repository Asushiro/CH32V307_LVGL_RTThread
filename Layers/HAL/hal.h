/********************************** (C) COPYRIGHT *******************************
 * File Name          : hal.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2023/10/9
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v30x_conf.h"

/*********************************************************************
 * CONSTANTS
 */



/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/**
 * @brief   内存数据复制
 */
void my_memcpy(void *pdst, const void *psrc, uint32_t len);

/**
 * @brief   内存数据值设置
 */
void my_memset(void *pdst, int8_t value, uint32_t len);

/**
 * @brief   内存数据值比较
 */
int8_t my_memcmp(const void *psrc1, const void *psrc2, uint32_t len);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
