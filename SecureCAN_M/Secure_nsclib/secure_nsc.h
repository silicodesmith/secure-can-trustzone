/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure_nsclib/secure_nsc.h
  * @author  MCD Application Team
  * @brief   Header for secure non-secure callable APIs list
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

/* USER CODE BEGIN Non_Secure_CallLib_h */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  non-secure callback ID enumeration definition
  */
typedef enum
{
  SECURE_FAULT_CB_ID     = 0x00U, /*!< System secure fault callback ID */
  GTZC_ERROR_CB_ID       = 0x01U,  /*!< GTZC secure error callback ID */
  CAN_DATA_READY_ID		 = 0x02U ,
  CAN_DATA_SEND_ENABLE_ID = 0x03U
} SECURE_CallbackIDTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
typedef void (*ns_callback_t)(void);

/* Exported functions ------------------------------------------------------- */
void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func);
void ns_can_tx_message(uint8_t *ptr, size_t size);
void SECURE_RegisterNSBuffer(uint8_t *ptr, size_t size);
#endif /* SECURE_NSC_H */
/* USER CODE END Non_Secure_CallLib_h */

