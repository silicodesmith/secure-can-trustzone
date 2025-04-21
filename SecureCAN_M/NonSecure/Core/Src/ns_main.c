/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <time.h>
#include "can_common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MY_DATA_SIZE 12
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */
static CarData ns_rxData;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static void MX_GPIO_Init(void);
static void MX_GTZC_NS_Init(void);
static void MX_LPUART1_UART_Init(void);
void generate_car_data(CarData *data);
void send_car_data();
int send_data_enable = 0;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void NS_CANDATA_Notification(void)
{
	// Packet structure
	const uint8_t header[3] = {'C', 'A', 'R'};
	// Your 40-byte struct data here
	const uint8_t footer[3] = {'E', 'N', 'D'};

	// Send function
	HAL_UART_Transmit(&hlpuart1, header, 3, 2000);
	// Called by secure world when data is ready
	HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ns_rxData, sizeof(ns_rxData), 2000);
	HAL_UART_Transmit(&hlpuart1, footer, 3, 2000);
}


void NS_CANDATA_senddata_enable(void)
{
	send_data_enable = 1;
}

#include <sys/time.h>

int _gettimeofday_r(struct _reent *ptr, struct timeval *tv, void *tz) {
	(void)ptr;
	(void)tz;

	if (tv) {
		// Replace this with actual time source (e.g., RTC, SysTick)
		tv->tv_sec = 0;   // Seconds since epoch
		tv->tv_usec = 0;  // Microseconds
	}

	// Return 0 for success, -1 for error
	return 0; // Success
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	//int i, val = 0;
	//int speed = 0;
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* GTZC initialisation */
	MX_GTZC_NS_Init();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_LPUART1_UART_Init();
	/* USER CODE BEGIN 2 */
	// Register buffer
	SECURE_RegisterNSBuffer((uint8_t *) &ns_rxData, sizeof(ns_rxData));
	SECURE_RegisterCallback(CAN_DATA_READY_ID,NS_CANDATA_Notification);
	SECURE_RegisterCallback(CAN_DATA_SEND_ENABLE_ID,NS_CANDATA_senddata_enable);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

		/* -- Sample board code for User push-button in interrupt mode ---- */
		//if (BspButtonState == BUTTON_PRESSED)
		//{
		/* Update button state */
		// BspButtonState = BUTTON_RELEASED;
		/* -- Sample board code to toggle leds ---- */
		// BSP_LED_Toggle(LED_GREEN);
		// BSP_LED_Toggle(LED_YELLOW);
		// BSP_LED_Toggle(LED_RED);
		/* ..... Perform your action ..... */
		//}

		/* USER CODE END WHILE */
		if(send_data_enable) {
			send_car_data();
			HAL_Delay(100U);
		}
		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief GTZC_NS Initialization Function
 * @param None
 * @retval None
 */
static void MX_GTZC_NS_Init(void)
{

	/* USER CODE BEGIN GTZC_NS_Init 0 */

	/* USER CODE END GTZC_NS_Init 0 */

	/* USER CODE BEGIN GTZC_NS_Init 1 */

	/* USER CODE END GTZC_NS_Init 1 */
	/* USER CODE BEGIN GTZC_NS_Init 2 */

	/* USER CODE END GTZC_NS_Init 2 */

}

/**
 * @brief LPUART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_LPUART1_UART_Init(void)
{

	/* USER CODE BEGIN LPUART1_Init 0 */

	/* USER CODE END LPUART1_Init 0 */

	/* USER CODE BEGIN LPUART1_Init 1 */

	/* USER CODE END LPUART1_Init 1 */
	hlpuart1.Instance = LPUART1;
	hlpuart1.Init.BaudRate = 115200;
	hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
	hlpuart1.Init.StopBits = UART_STOPBITS_1;
	hlpuart1.Init.Parity = UART_PARITY_NONE;
	hlpuart1.Init.Mode = UART_MODE_TX_RX;
	hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
	if (HAL_UART_Init(&hlpuart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN LPUART1_Init 2 */

	/* USER CODE END LPUART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void send_car_data()
{
	static CarData data;

	generate_car_data(&data);
	ns_can_tx_message((uint8_t *)&data,sizeof(data));
}

int max_to_min_iterations = 28;

void generate_car_data(CarData *data)
{
	static int initialized = 0;
	static uint32_t base_timestamp = 0;
	static float base_engine_temp = 70.0f;
	static float base_battery_temp = 20.0f;
	static uint16_t base_tire_pressure = 30;
	static uint16_t base_speed = 0;
	static uint16_t base_rpm = 800;
	static uint8_t base_fuel = 100;

	static int direction = 1; // 1 for increasing, -1 for decreasing
	static int iteration_count = 0;

	if (!initialized) {
		srand(time(NULL));
		initialized = 1;
	}

	if (iteration_count == max_to_min_iterations) {
		direction = direction == 1 ? -1 : 1;
		iteration_count = 0;
	}

	iteration_count++;

	// Generate realistic varying values with constraints
	base_speed += direction * 10; // Increment/decrement speed
	data->speed = (uint16_t)fmax(0, fmin(300, base_speed));

	data->gear = (uint8_t)((data->speed < 20) ? 0 : 1 + (data->speed / 40)); // Simple gear logic
	data->gear = data->gear > 8 ? 8 : data->gear;

	// Temperatures with gradual changes
	base_engine_temp += direction * 0.5f;
	data->engine_temp = fmax(70.0f, fmin(120.0f, base_engine_temp));

	base_battery_temp += direction * 0.2f;
	data->battery_temp = fmax(20.0f, fmin(45.0f, base_battery_temp));

	// Tire pressures with individual variations
	for(int i = 0; i < 4; i++) {
		data->tire_pressure[i] = base_tire_pressure + direction * ((rand() % 3) - 1);
		data->tire_pressure[i] = fmax(30, fmin(35, data->tire_pressure[i]));
	}

	// RPM proportional to speed with noise
	base_rpm += direction * 200; // Increment/decrement RPM
	data->rpm = fmax(800, fmin(7000, base_rpm));

	// Fuel level gradually decreases during max-to-min iterations
	if(iteration_count == 1) {
		base_fuel -= (rand() % 2);
		data->fuel_level = base_fuel;
		if(base_fuel == 5) {
			base_fuel = 100;
		}
	}


	// Brake status (randomly set bits: ABS/EBD/Brake Fluid Low)
	data->brake_status = (rand() % 100 < 5) ? (1 << (rand() % 3)) : 0;

	// Error codes randomly set during decreasing phase
	data->error_codes = (direction == -1 && rand() % 100 < 10) ? (1 << (rand() % 16)) : 0;

	// Timestamp with realistic increments
	base_timestamp += direction * (15 + (rand() % 10));
	data->timestamp = base_timestamp;

	// Reserved bytes set to zero
	for(int i = 0; i < sizeof(data->reserved); i++) {
		data->reserved[i] = 0;
	}
}


/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
