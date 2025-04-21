/*
 * can_driver_s_i.h
 *
 *  Created on: Apr 20, 2025
 *      Author: Naveen
 */

#ifndef INC_CAN_DRIVER_S_I_H_
#define INC_CAN_DRIVER_S_I_H_

#include "can_common.h"

#define TIME_PROFILE_PRINT

#pragma pack(push, 1) // Ensure no padding between struct members

typedef struct {
	uint32_t message_id;   // CAN message identifier (e.g., 0x123)
	uint8_t  dlc;          // Data Length Code (payload size = 64 bytes)
	uint8_t  flags;        // CAN-FD flags (e.g., FDF, BRS, ESI)
	uint16_t counter;      // Sequence counter for packet ordering
} CANHeader;

typedef struct {
	CarData   data;     // 40 bytes
	uint8_t   tag[16];  // Poly1305 authentication tag - 16 bytes
}can_msg_chipher_t;

typedef struct {
	CANHeader msg_header;   // 8 bytes
	can_msg_chipher_t   chipher_data;     // 40 bytes
}can_msg_t;

#pragma pack(pop) // Restore default struct padding


#define CAN_CAR_DATA_ID         (0x111)   /* TX CAN message identifier    */
#define CAN_NONCE_UPDATE_ID		(0x222)
#define COUNTOF(BUFFER) 		(sizeof((BUFFER)) / sizeof(*(BUFFER)))
#define NONCE_UPDATE_INTERVAL 	300U

static int update_nonce();

#endif /* INC_CAN_DRIVER_S_I_H_ */
