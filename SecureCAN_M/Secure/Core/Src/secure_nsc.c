/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    Secure/Src/secure_nsc.c
 * @author  MCD Application Team
 * @brief   This file contains the non-secure callable APIs (secure world)
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
#include "can_driver_s.h"

/** @addtogroup STM32H5xx_HAL_Examples

 * @{
 */

/** @addtogroup Templates
 * @{
 */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
//void *pcandataCallback = NULL;
ns_callback_t pcandataCallback;
ns_callback_t pcandata_send_enable_Callback;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Secure registration of non-secure callback.
 * @param  CallbackId  callback identifier
 * @param  func        pointer to non-secure function
 * @retval None
 */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
	if(func != NULL)
	{
		switch(CallbackId)
		{
		case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
			pSecureFaultCallback = func;
			break;
		case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
			pSecureErrorCallback = func;
			break;
		case CAN_DATA_READY_ID:
			pcandataCallback  = func;
			break;
		case CAN_DATA_SEND_ENABLE_ID:
			pcandata_send_enable_Callback = func;
			break;
		default:
			/* unknown */
			break;
		}
	}
}

/**
 * @}
 */

uint8_t *ns_can_rx_buffer = NULL;
size_t ns_can_rx_size = 0;

CMSE_NS_ENTRY void SECURE_RegisterNSBuffer(uint8_t *ptr, size_t size)
{
	// Validate NS pointer range
	//if (cmse_check_address_range((void *)ptr, size, CMSE_NONSECURE) != NULL) {
	ns_can_rx_buffer = ptr;
	ns_can_rx_size = size;
	//}
}


CMSE_NS_ENTRY void ns_can_tx_message(uint8_t *ptr, size_t size)
{
	can_tx_msg(ptr,size);
}

/**
 * @}
 */
/* USER CODE END Non_Secure_CallLib */

