/*
 * lis3dh.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Quinn
 */

#ifndef INC_LIS3DH_H_
#define INC_LIS3DH_H_

#include <string.h>
#include "main.h"
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

#endif /* INC_LIS3DH_H_ */
