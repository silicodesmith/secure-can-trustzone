/*
 * can_driver_s.h
 *
 *  Created on: Apr 20, 2025
 *      Author: prasa
 */

#ifndef INC_CAN_DRIVER_S_H_
#define INC_CAN_DRIVER_S_H_

#include <stdint.h>
#include <stddef.h>
#include "stm32h5xx_hal.h"


// Function declarations

/**
 * @brief Initializes the CAN driver.
 */
void can_driver_init(void);

/**
 * @brief Transmits a CAN message.
 * @param ptr Pointer to the data to be transmitted.
 * @param size Size of the data to be transmitted.
 * @return 0 on success, -1 on failure.
 */
int can_tx_msg(uint8_t *ptr, size_t size);


#endif /* INC_CAN_DRIVER_S_H_ */
