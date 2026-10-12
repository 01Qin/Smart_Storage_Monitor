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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


uint8_t check, data[2], accel_data[6], temp_data[2];

int16_t X_RAW, Y_RAW, Z_RAW, TEMP_RAW;

float Ax, Ay, Az, temperature;

char tx[100];
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
int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_TIM15_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN BSP */

  /* -- Sample board code to send message over COM1 port ---- */
  printf("Welcome to STM32 world !\n\r");

  /* -- Sample board code to switch on leds ---- */
  BSP_LED_On(LED_GREEN);
  /* USER CODE END BSP */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	LIS3DH_init(0x0F);		//WHO_AM_I (0Fh)
  //Display Initialisation
  HAL_Delay(100);
  OLED_Init(Display);
  HAL_Delay(10);
  OLED_CLS(Display);
  HAL_Delay(10);
  OLED_P8x16Str(Display, 0, 4, "MaKE sOme FuN!;)");
  HAL_Delay(2000);
  OLED_CLS(Display);
  HAL_Delay(10);

  uint8_t int1_status = 0;
  uint8_t alarm_triggered = 0;

  while (1)
  {

/* -- Sample board code to toggle leds ---- */
       BSP_LED_Toggle(LED_GREEN);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		LIS3DH_read_accel(OUT_X_L);		//X-axis acceleration data output.
		LIS3DH_read_temp(OUT_ADC1_L);			//temperature sensor data output.

		X_RAW = (accel_data[1]<<8|accel_data[0]);
		Y_RAW = (accel_data[3]<<8|accel_data[2]);
		Z_RAW = (accel_data[5]<<8|accel_data[4]);

		// Convert into 'mg'
		Ax = ( (float)X_RAW / 64.0f ) *  4.0f;
		Ay = ( (float)Y_RAW / 64.0f ) *  4.0f;
		Az = ( (float)Z_RAW / 64.0f ) *  4.0f;

//		// Convert into 'g'
//		Ax = Ax / 16000.0f;
//		Ay = Ay / 16000.0f;
//		Az = Az / 16000.0f;


		TEMP_RAW = (temp_data[1]<<8|temp_data[0]);
		// Convert into 'celsius',
		temperature = ( ( (float)TEMP_RAW / 64.0f ) / 4.0f ) + 25.0f;
// todo: use aht20 sensor
		 printf("\n");
		 printf("X = %.2f, Y = %.2f, Z = %.2f, temperature = %.2f\r\n", Ax, Ay, Az, temperature);

		 uint8_t reg_addr = 0x31;
		     HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_ADDR, &reg_addr, 1, 1000);
		     HAL_I2C_Master_Receive(&hi2c2, LIS3DH_ADDR, &int1_status, 1, 1000);

		     // Bit 6 (0x40) of INT1_SRC goes HIGH if any configured threshold event happened
		     if (int1_status & 0x40)
		     {
		         alarm_triggered = 1;
		         printf("ALARM TRIGGERED !!! Threshold exceeded!\r\n");
		     }
		     else
		     {
		         alarm_triggered = 0;
		     }

		 uint8_t line = 0;
		 char buf_x[16];
		 char buf_y[16];
		 char buf_z[16];

		 sprintf(buf_x, "X: %.2f  ", Ax);
		 sprintf(buf_y, "Y: %.2f  ", Ay);
		 sprintf(buf_z, "Z: %.2f  ", Az);

		 OLED_P6x8Str(Display, 0, 0, "Sensor values:");

		 OLED_P6x8Str(Display, 0, (line + 1) * 2, buf_x);
		 line++;
		 OLED_P6x8Str(Display, 0, (line + 1) * 2, buf_y);
		 line++;
		 OLED_P6x8Str(Display, 0, (line + 1) * 2, buf_z);



		 if (alarm_triggered)
		     {
		         // Highlight an active warning at the bottom of the screen

			 	 OLED_P6x8Str(Display, 0, (line + 1) * 2, "HIT DETECTED !!");
			 	 HAL_Delay(1000);
		    	 OLED_P6x8Str(Display, 0, (line + 1) * 2, "                ");

		     }

		HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}




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


void LIS3DH_read_temp(uint8_t reg_addr)
{
	reg_addr |= 0x80;		//multibyte read enabled

	while(HAL_I2C_Master_Transmit(&hi2c2, LIS3DH_ADDR, &reg_addr, 1, 1000) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
	while(HAL_I2C_Master_Receive(&hi2c2, LIS3DH_ADDR, temp_data, 2, 1000) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)		//Acknowledge
		{
			Error_Handler();
		}
	}
}

/* USER CODE END 4 */

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

/**
  * @}
  */

/**
  * @}
  */

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
#ifdef USE_FULL_ASSERT
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

