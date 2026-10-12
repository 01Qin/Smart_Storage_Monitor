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
//#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "i2c.h"
#include "gpio.h"
#include "OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LIS3DH_ADDR             0x19<<1  // 7-bit I2C address. If SA0 is pulled high, address is 0x19.
#define LIS3DH_DEVICE_ID        0x33  // Contents of WHO_AM_I register.
#define WHO_AM_I  	0x0F
#define OUT_X_L		0x28 // X-axis acceleration data
#define OUT_Y_L		0x2A // Y-axis acceleration data
#define OUT_Z_L		0x2C // Z-axis acceleration data
#define OUT_ADC1_L	0x08	 // ADC channel 1 conversion or temperature sensor data output.

#define CTRL_REG1	0x20
#define CTRL_REG2	0x21
#define CTRL_REG3	0x22
#define CTRL_REG4	0x23
#define CTRL_REG5	0x24

#define INT1_CFG    0x30
#define INT1_SRC    0x31  // Interrupt 1 source register, r only
#define INT1_THS    0x32  // Interrupt 1 threshold, rw
#define INT1_DUR    0x33

#define TEMP_CFG_REG	  0x1F

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
void LIS3DH_write(uint8_t reg_addr, uint8_t value);
void LIS3DH_read_accel(uint8_t reg_addr);
void LIS3DH_init(uint8_t reg_addr);
void LIS3DH_read_temp(uint8_t reg_addr);
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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  /* I2C2_EV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
  /* I2C2_ER_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T15_TRGO;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR_ADC1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x40B285C2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 0;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 65535;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
//
//		LIS3DH_write(CTRL_REG5, 0x80);		//Reboot memory content.
//		HAL_Delay(100);
//		LIS3DH_write(CTRL_REG1, 0x00);		//Power-down.
//		HAL_Delay(100);
//		LIS3DH_write(CTRL_REG1, 0x77);		//Power-up normal mode, 400Hz, XYZ-axis enabled
//		HAL_Delay(100);
//		LIS3DH_write(CTRL_REG4, 0x80);		//0x00-> 2g, 0x10-> 4g, 0x20-> 8g, 0x30-> 16g and high-resolution disabled.
//		HAL_Delay(100);
//		LIS3DH_write(TEMP_CFG_REG, 0x08);		//Temperature sensor and ADC enabled.
//		HAL_Delay(100);

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

