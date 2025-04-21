/*
 * CAN_Driver.c
 *
 *  Created on: Apr 5, 2025
 *      Author: Naveen S
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32h5xx_hal.h"
#include "stm32h5xx_nucleo.h"
#include <can_driver_s_i.h>
#include <can_driver_s.h>
#include "msg_security.h"
#include "time_profile.h"

typedef void (*ns_callback_t)(void) __attribute__((cmse_nonsecure_call));

extern FDCAN_HandleTypeDef hfdcan1;
extern ns_callback_t pcandataCallback;
extern uint8_t *ns_can_rx_buffer;
extern void Error_Handler(void);


FDCAN_RxHeaderTypeDef rxHeader;
FDCAN_TxHeaderTypeDef txHeader;
static CarData rxData;
static can_msg_t cmsg_st_tx;
static can_msg_t cmsg_st_rx;
static can_msg_t cmsg_st_tx_replay;
int can_auth_test_failure = 0;

// Function to return the corresponding size for the given dlc_value
static int getFDCANSizeFromDLC(uint32_t dlc_value, uint8_t *size)
{
	// Check if the provided dlc_value matches a known macro
	switch(dlc_value) {
	case FDCAN_DLC_BYTES_0:   *size = 0;  break;
	case FDCAN_DLC_BYTES_1:   *size = 1;  break;
	case FDCAN_DLC_BYTES_2:   *size = 2;  break;
	case FDCAN_DLC_BYTES_3:   *size = 3;  break;
	case FDCAN_DLC_BYTES_4:   *size = 4;  break;
	case FDCAN_DLC_BYTES_5:   *size = 5;  break;
	case FDCAN_DLC_BYTES_6:   *size = 6;  break;
	case FDCAN_DLC_BYTES_7:   *size = 7;  break;
	case FDCAN_DLC_BYTES_8:   *size = 8;  break;
	case FDCAN_DLC_BYTES_12:  *size = 12; break;
	case FDCAN_DLC_BYTES_16:  *size = 16; break;
	case FDCAN_DLC_BYTES_20:  *size = 20; break;
	case FDCAN_DLC_BYTES_24:  *size = 24; break;
	case FDCAN_DLC_BYTES_32:  *size = 32; break;
	case FDCAN_DLC_BYTES_48:  *size = 48; break;
	case FDCAN_DLC_BYTES_64:  *size = 64; break;
	default:
		// If the dlc_value is not supported, return an error
		return HAL_ERROR;  // Error code for unsupported dlc_value
	}
	return HAL_OK;  // Successfully assigned the corresponding size
}

// Function to return the corresponding macro value for the given size
static int getFDCANDLCBytes(uint8_t size, uint32_t *dlc_value)
{
	// Check if the provided size is within the supported range
	switch(size) {
	case 0:  *dlc_value = FDCAN_DLC_BYTES_0;   break;
	case 1:  *dlc_value = FDCAN_DLC_BYTES_1;   break;
	case 2:  *dlc_value = FDCAN_DLC_BYTES_2;   break;
	case 3:  *dlc_value = FDCAN_DLC_BYTES_3;   break;
	case 4:  *dlc_value = FDCAN_DLC_BYTES_4;   break;
	case 5:  *dlc_value = FDCAN_DLC_BYTES_5;   break;
	case 6:  *dlc_value = FDCAN_DLC_BYTES_6;   break;
	case 7:  *dlc_value = FDCAN_DLC_BYTES_7;   break;
	case 8:  *dlc_value = FDCAN_DLC_BYTES_8;   break;
	case 12: *dlc_value = FDCAN_DLC_BYTES_12;  break;
	case 16: *dlc_value = FDCAN_DLC_BYTES_16;  break;
	case 20: *dlc_value = FDCAN_DLC_BYTES_20;  break;
	case 24: *dlc_value = FDCAN_DLC_BYTES_24;  break;
	case 32: *dlc_value = FDCAN_DLC_BYTES_32;  break;
	case 48: *dlc_value = FDCAN_DLC_BYTES_48;  break;
	case 64: *dlc_value = FDCAN_DLC_BYTES_64;  break;
	default:
		// If the size is not supported, return an error
		return HAL_ERROR;  // Error code for unsupported size
	}
	return HAL_OK;  // Successfully assigned the corresponding macro
}

void can_driver_init()
{
	/* Configure reception filter to Rx FIFO 0 */
	FDCAN_FilterTypeDef        sFilterConfig;
	FDCAN_FilterTypeDef        nonce_FilterConfig;

	sFilterConfig.IdType       = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex  = 0U;
	sFilterConfig.FilterType   = FDCAN_FILTER_RANGE;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1    = CAN_CAR_DATA_ID;
	sFilterConfig.FilterID2    = CAN_CAR_DATA_ID;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}


	nonce_FilterConfig.IdType       = FDCAN_STANDARD_ID;
	nonce_FilterConfig.FilterIndex  = 1U;
	nonce_FilterConfig.FilterType   = FDCAN_FILTER_RANGE;
	nonce_FilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	nonce_FilterConfig.FilterID1    = CAN_NONCE_UPDATE_ID;
	nonce_FilterConfig.FilterID2    = CAN_NONCE_UPDATE_ID;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &nonce_FilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/**
	 *  Configure global filter:
	 *    - Filter all remote frames with STD and EXT ID
	 *    - Reject non matching frames with STD ID and EXT ID
	 */
	if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
			FDCAN_REJECT, FDCAN_REJECT,
			FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
	{
		Error_Handler();
	}

	/* Activate Rx FIFO 0 new message notification */
	if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0U) != HAL_OK)
	{
		Error_Handler();
	}

	/* Activate Rx FIFO 1 new message notification */
	if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0U) != HAL_OK)
	{
		Error_Handler();
	}


	/**
	 * Configure and enable Tx Delay Compensation, required for BRS mode.
	 * TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
	 * TdcFilter default recommended value: 0
	 */
	if (HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1,
			(hfdcan1.Init.DataPrescaler * hfdcan1.Init.DataTimeSeg1), 0U) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_FDCAN_EnableTxDelayCompensation(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}

	/* Start FDCAN controller */
	if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}
}

#ifdef SEND_CAN_DATA_UART
static void send_struct_binary(CarData *data) {
	// Send header
	printf("CAR");  // Header identifier

	// Send binary representation of each field in the struct
	printf("%c%c", data->speed & 0xFF, (data->speed >> 8) & 0xFF);   // Speed (2 bytes)
	printf("%c", data->gear);                                       // Gear (1 byte)

	unsigned char *engine_temp_bytes = (unsigned char *)&data->engine_temp;
	for (int i = 0; i < sizeof(float); i++) {
		printf("%c", engine_temp_bytes[i]);                         // Engine Temp (4 bytes)
	}

	unsigned char *battery_temp_bytes = (unsigned char *)&data->battery_temp;
	for (int i = 0; i < sizeof(float); i++) {
		printf("%c", battery_temp_bytes[i]);                        // Battery Temp (4 bytes)
	}

	for (int i = 0; i < 4; i++) {
		printf("%c%c", data->tire_pressure[i] & 0xFF,               // Tire Pressure FL, FR, RL, RR (8 bytes total)
				(data->tire_pressure[i] >> 8) & 0xFF);
	}

	printf("%c%c", data->rpm & 0xFF, (data->rpm >> 8) & 0xFF);      // RPM (2 bytes)
	printf("%c", data->fuel_level);                                 // Fuel Level (1 byte)
	printf("%c", data->brake_status);                               // Brake Status (1 byte)
	printf("%c%c", data->error_codes & 0xFF,                        // Error Codes (2 bytes)
			(data->error_codes >> 8) & 0xFF);

	unsigned char *timestamp_bytes = (unsigned char *)&data->timestamp;
	for (int i = 0; i < sizeof(uint32_t); i++) {
		printf("%c", timestamp_bytes[i]);                           // Timestamp (4 bytes)
	}

	// Send footer
	printf("END");                                                  // Footer identifier
}
#endif

/**
 * @brief  Rx FIFO 0 callback.
 * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
 *         the configuration information for the specified FDCAN.
 * @param  RxFifo0ITs indicates which Rx FIFO 0 interrupts are signaled.
 *         This parameter can be any combination of @arg FDCAN_Rx_Fifo0_Interrupts.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	int ret;
	uint8_t can_len;
	size_t rx_sz;
#ifdef TIME_PROFILE_PRINT
	uint32_t time_us;
#endif

	BSP_LED_On(LED_YELLOW);
	if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0U) {
		/* Retrieve Rx messages from RX FIFO0 */
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rxHeader, (uint8_t *)&cmsg_st_rx) != HAL_OK)
		{
			Error_Handler();
		}


		getFDCANSizeFromDLC(rxHeader.DataLength,&can_len);

#ifdef TIME_PROFILE_PRINT
		profiler_start();
#endif
		ret = msg_decrypt((uint8_t *) &cmsg_st_rx.chipher_data, can_len-sizeof(cmsg_st_rx.msg_header), (uint8_t *) &cmsg_st_rx.msg_header,sizeof(cmsg_st_rx.msg_header), (uint8_t *) &rxData,&rx_sz);
#ifdef TIME_PROFILE_PRINT
		time_us = profiler_stop_us();
		printf("Decryption time: %lu ns\n", time_us);
#endif
		if(ret == 0) {
			memcpy(ns_can_rx_buffer,&rxData,rx_sz);
			pcandataCallback();
#ifdef SEND_CAN_DATA_UART
			send_struct_binary(&rxData);
#endif
		} else {
			printf("Decrypt Error, Meessage Rejected\n");
		}
#if 0
		if ((rxHeader.Identifier == RX_ID) &&
				(rxHeader.IdType     == FDCAN_STANDARD_ID) &&
				(rxHeader.DataLength == FDCAN_DLC_BYTES_16) &&
				(BufferCmp8b(txData, rxData, COUNTOF(rxData)) == 0U))
		{
			/* Turn LED1 on */
			BSP_LED_On(LED1);
		}
#endif
	}
	BSP_LED_Off(LED_YELLOW);
}


void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
	int ret;
	uint8_t can_len;
	size_t rx_sz;
	uint8_t tmp_IV[12];

	if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != 0U) {
		/* Retrieve Rx messages from RX FIFO0 */
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &rxHeader, (uint8_t *)&cmsg_st_rx) != HAL_OK)
		{
			Error_Handler();
		}

		getFDCANSizeFromDLC(rxHeader.DataLength,&can_len);

		ret = msg_decrypt((uint8_t *) &cmsg_st_rx.chipher_data, can_len-sizeof(cmsg_st_rx.msg_header), (uint8_t *) &cmsg_st_rx.msg_header,sizeof(cmsg_st_rx.msg_header), (uint8_t *) &tmp_IV,&rx_sz);
		if(ret == 0) {

			set_nonce(tmp_IV);
			printf("Nounce Updated\n");
		} else {
			printf("Message Decrypt Error [Nonce]\n");
		}
	}
}


int can_tx_msg(uint8_t *ptr, size_t size)
{
	int ret = 0;
	static uint32_t pkt_id = 0;
	static int nonce_pkt = 0;
	static int replay_capture = 0;
	size_t c_sz;
#ifdef TIME_PROFILE_PRINT
	uint32_t time_us;
	//uint64_t time_ns;
#endif

	BSP_LED_On(LED_GREEN);

	if(pkt_id == NONCE_UPDATE_INTERVAL ) {
		printf("TX: Nonce Update Started\n");
		pkt_id = 1;
		nonce_pkt  = 1;
		update_nonce();
		HAL_Delay(10U);
		nonce_pkt = 0;
	}

	cmsg_st_tx.msg_header.message_id = 0xA5A5;
	cmsg_st_tx.msg_header.counter = pkt_id++;
	memset((uint8_t *) &cmsg_st_tx.chipher_data, 0x0, sizeof(cmsg_st_tx.chipher_data));

#ifdef TIME_PROFILE_PRINT
	profiler_start();
#endif
	ret = msg_encrypt(ptr, size, (uint8_t *) &cmsg_st_tx.msg_header, sizeof(cmsg_st_tx.msg_header), (uint8_t *) &cmsg_st_tx.chipher_data, &c_sz);
#ifdef TIME_PROFILE_PRINT
	time_us = profiler_stop_us();
	printf("Encryption time: %lu us\n", time_us);
#endif
	if (ret != 0 ) {
		printf("Encrypt Error\n");
		return -1;
	}

	if(pkt_id == 10 && replay_capture == 0) {
		memcpy(&cmsg_st_tx_replay,&cmsg_st_tx,sizeof(cmsg_st_tx));
		replay_capture = 1;
	}

	if(can_auth_test_failure > 0) {
		//Msg Injection - Spoofing
		//Man in middle - Case 1,2,3
		//Replay Attacks - Case 4
		//Malformed Packet - Case1,2,3
		//Evas Dropping - Packet Cant give useful data Attacker
		if (can_auth_test_failure == 1) {
			printf("Corrupting the Message ID\n");
			//Corrupt Add Data
			cmsg_st_tx.msg_header.message_id = 0xBEEF;
		}
		if (can_auth_test_failure == 2) {
			printf("Corrupting the Encrypted Data\n");
			//Corrupt Encrypted Message
			cmsg_st_tx.chipher_data.data.gear = 40;
		}
		if (can_auth_test_failure == 3) {
			printf("Corrupting the TAG\n");
			//Corrupt TAG
			cmsg_st_tx.chipher_data.tag[0] = 0xad;
		}
		if (can_auth_test_failure == 4) {
			//Message Replay
			printf("Replay Attack\n");
			memcpy(&cmsg_st_tx, &cmsg_st_tx_replay,sizeof(cmsg_st_tx));
		}
		can_auth_test_failure--;
	}


	/* Prepare Tx message Header */
	if(nonce_pkt == 1 ) {
		txHeader.Identifier          = CAN_NONCE_UPDATE_ID;
	} else {
		txHeader.Identifier          = CAN_CAR_DATA_ID;
	}
	txHeader.IdType              = FDCAN_STANDARD_ID;
	txHeader.TxFrameType         = FDCAN_DATA_FRAME;
	if (getFDCANDLCBytes(c_sz + sizeof(cmsg_st_tx.msg_header),&txHeader.DataLength) != HAL_OK)
	{
		printf("Invalid CAN Size\n");
		return -1;
	}
	txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	txHeader.BitRateSwitch       = FDCAN_BRS_OFF;
	txHeader.FDFormat            = FDCAN_FD_CAN;
	txHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
	txHeader.MessageMarker       = 0U;

	/* Add message to TX FIFO */
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, (uint8_t *)&cmsg_st_tx) != HAL_OK)
	{
		printf("Failed to add CAN Message\n");
		return -1;
	}

	BSP_LED_Off(LED_GREEN);
	return 0;
}

static int update_nonce()
{
	int rand_val = rand() % 100;

	send_update_nonce(rand_val);
	return 0;
}
