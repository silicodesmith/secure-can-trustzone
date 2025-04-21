/*
 * can_common.h
 *
 *  Created on: Apr 20, 2025
 *      Author: Naveen S
 */
#ifndef INC_CAN_COMMON_H_
#define INC_CAN_COMMON_H_

#pragma pack(push, 1) // Ensure no padding between struct members

// Car Telemetry Data (40 bytes)
typedef struct {
	uint16_t speed;                // Vehicle speed (km/h) - 2 bytes
	uint8_t  gear;                 // Current gear (0-8) - 1 byte
	float    engine_temp;          // Engine temperature (°C) - 4 bytes
	float    battery_temp;         // Battery temperature (°C) - 4 bytes
	uint16_t tire_pressure[4];     // Tire pressures (psi) - 8 bytes (FL, FR, RL, RR)
	uint16_t rpm;                  // Engine RPM - 2 bytes
	uint8_t  fuel_level;           // Fuel level (0-100%) - 1 byte
	uint8_t  brake_status;         // Brake status (bitmask: ABS, EBD, etc.) - 1 byte
	uint16_t error_codes;          // Error codes (bitmask) - 2 bytes
	uint32_t timestamp;            // Timestamp (ms since startup) - 4 bytes
	uint8_t  reserved[11];         // Reserved for future use - 11 bytes
} CarData;

#pragma pack(pop) // Restore default struct padding

#endif /* INC_CAN_COMMON_H_ */
