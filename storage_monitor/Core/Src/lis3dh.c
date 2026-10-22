/*
 * lis3dh.c
 *
 *  Created on: Aug 25, 2026
 */


/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "lis3dh.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern COM_InitTypeDef BspCOMInit;
extern ADC_HandleTypeDef hadc1;

extern I2C_HandleTypeDef hi2c2;

extern RTC_HandleTypeDef hrtc;

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim15;

extern UART_HandleTypeDef huart2;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

 extern uint8_t check, data[2], accel_data[6], temp_data[2];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */


/* USER CODE BEGIN 4 */

void LIS3DH_write(uint8_t reg_addr, uint8_t value)
{
	data[0] = reg_addr;
	data[1] = value;

	while(HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_ADDR, data, 2, 1000)!= HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
}

void LIS3DH_read_accel(uint8_t reg_addr)
{
	reg_addr |= 0x80;		//multibyte read enabled

	while(HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_ADDR, &reg_addr, 1, 1000) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
	while(HAL_I2C_Master_Receive(&hi2c2, LIS3DH_ADDR, accel_data, 6, 1000) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
}

void LIS3DH_init(uint8_t reg_addr)
{
	while(HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_ADDR, &reg_addr, 1, 1000) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
	while(HAL_I2C_Master_Receive(&hi2c2, LIS3DH_ADDR, &check, 1, 1000) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
	if(check == LIS3DH_DEVICE_ID)
	{

//		Startup sequence
		// 1. Reboot memory content to ensure a clean slate
		LIS3DH_write(CTRL_REG5, 0x80);
		HAL_Delay(20);

		// 2. Set Full Scale to ±2g and enable Block Data Update (BDU)
		LIS3DH_write(CTRL_REG4, 0x80);        // CTRL_REG4 = 0x80 (1 LSB threshold = 16mg)

		// 3. Route the INT1 IA1 (Interrupt Activity 1) signal to the physical INT1 pin
		LIS3DH_write(CTRL_REG3, 0x40);        // CTRL_REG3 = 0x40 (Enables I1_IA1)

		// 4. Set the threshold (250mg / 16mg = 16 = 0x10)
		LIS3DH_write(INT1_THS, 0x10);        // INT1_THS = 0x10

		// 5. Set the minimum duration the event must last to trigger (0 = triggers instantly)
		LIS3DH_write(INT1_DUR, 0x00);        // INT1_DURATION = 0x00

		// 6. Configure the event condition: High event (OR combination) on X, Y, or Z axes
		LIS3DH_write(INT1_CFG, 0x2A);        // INT1_CFG = 0x2A (00101010b -> ZHIE, YHIE, XHIE enabled)

		// 7. Latch the interrupt on the INT1 pin until read INT1_SRC
		LIS3DH_write(CTRL_REG5, 0x08);     // CTRL_REG5 = 0x08 (LIR1 = 1)

		 LIS3DH_write(CTRL_REG2, 0x01);  // High-pass filter enabled for INT1
		// 8. Turn the sensor on. (Power-up normal mode, 400Hz, XYZ enabled)
		LIS3DH_write(CTRL_REG1, 0x77);        // CTRL_REG1 = 0x77
		HAL_Delay(20);
	}
}

/* USER CODE END 4 */



