# Smart Storage Monitor

A dual-MCU embedded monitoring system built with an **STM32G431** and **Raspberry Pi Pico W**.

The STM32G431 collects environmental and motion data from an **AHT20 temperature/humidity sensor** and a **LIS3DH accelerometer**, handles local fan control and OLED output, and sends relevant sensor data to the Pico W via UART. The Pico W receives and processes this information and makes the system status available to users through a web interface.

## Features

- Temperature and humidity monitoring with AHT20
- 3-axis acceleration monitoring with LIS3DH
- Impact/movement detection using the LIS3DH interrupt functionality
- Automatic fan control based on temperature
- PWM-based fan speed control
- Local OLED display for sensor values and warnings
- UART communication between STM32G431 and Raspberry Pi Pico W
- Wi-Fi connectivity through the Pico W
- Web-based system monitoring
- LED status indication

## System Architecture

```text
                        Smart Storage Monitor

     +-------------------+
     |       AHT20       |
     | Temperature / RH  |
     +---------+---------+
               |
               | I2C
               v
     +-------------------+            UART            +-------------------+
     |     STM32G431     | --------------------------> | Raspberry Pi      |
     |                   |                             | Pico W            |
     | Sensor processing |                             |                   |
     | Fan control       |                             | Data processing   |
     | Alarm detection   |                             | Wi-Fi             |
     +----+----------+---+                             | Web interface     |
          |          |                                 +---------+---------+
      I2C |          | PWM                                      |
          v          v                                          | Wi-Fi
     +---------+  +---------+                                    v
     | LIS3DH  |  |   Fan   |                              +----------+
     | Accel.  |  +---------+                              | Browser  |
     +---------+                                           +----------+
          |
     +---------+
     |  OLED   |
     +---------+
```

## Hardware

### Main Controllers

- STM32G431
- Raspberry Pi Pico W

### Sensors and Peripherals

- AHT20 temperature and humidity sensor
- LIS3DH 3-axis accelerometer
- OLED display
- PWM-controlled cooling fan
- Status LEDs

## How It Works

### 1. Sensor Acquisition

The STM32G431 acts as the main sensor and hardware controller.

The **AHT20** is used to obtain:

- Temperature
- Relative humidity

The **LIS3DH** is used to obtain:

- X-axis acceleration
- Y-axis acceleration
- Z-axis acceleration
- Threshold/impact detection

Both sensors communicate with the STM32 through I2C.

### 2. Temperature-Based Fan Control

The STM32 monitors the measured temperature.

When the temperature reaches:

```text
25.5 °C
```

the cooling fan is activated.

The fan is controlled using PWM, with the PWM value gradually increasing while the temperature remains above the threshold.

When the temperature falls below the threshold, the fan is stopped.

### 3. Impact Detection

The LIS3DH accelerometer monitors acceleration and can generate a threshold event.

When such an event is detected, the system displays:

```text
HIT DETECTED !!
```

on the OLED display.

### 4. STM32 to Pico W Communication

The STM32 sends monitoring data to the Pico W over UART at:

```text
115200 baud
8 data bits
1 stop bit
No parity
```

A transmitted message has the following format:

```text
Temp:25.80 C Hum:45.20% Fan speed:30
```

The STM32 currently sends:

- Temperature
- Humidity
- Fan speed

The Pico W receives each UART message and parses the values using `sscanf()`.

### 5. Pico W

The Raspberry Pi Pico W is responsible for the network-facing part of the project.

It:

1. Receives sensor data from the STM32 through UART.
2. Parses temperature, humidity, and fan speed.
3. Monitors incoming messages.
4. Uses its onboard LED as a communication/activity indicator.
5. Connects to Wi-Fi.
6. Provides relevant monitoring information through a web interface.

## Communication Flow

```text
AHT20 ─────┐
           │
           ├── I2C ──> STM32G431
           │              │
LIS3DH ────┘              │
                          ├──> OLED
                          │
                          ├── PWM ──> Fan
                          │
                          └── UART ──> Pico W ── Wi-Fi ──> Web Browser
```

## UART Configuration

### STM32G431

The STM32 uses USART2:

```c
huart2.Init.BaudRate = 115200;
huart2.Init.WordLength = UART_WORDLENGTH_8B;
huart2.Init.StopBits = UART_STOPBITS_1;
huart2.Init.Parity = UART_PARITY_NONE;
```

Sensor information is transmitted using:

```c
sprintf(
    pico_tx_buf,
    "Temp:%.2f C Hum:%.2f%% Fan speed:%d\n",
    temp,
    humidity,
    pwm_val
);

HAL_UART_Transmit(
    &huart2,
    (uint8_t *)pico_tx_buf,
    pico_msg,
    HAL_MAX_DELAY
);
```

### Raspberry Pi Pico W

The Pico W uses UART1:

```c
#define UART_ID uart1
#define BAUDRATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9
```

The received message is parsed using

```c
sscanf(
    rx_buf,
    "Temp:%f C Hum:%f%% Fan speed:%d",
    &temp,
    &hum,
    &speed
);
```

## Software
### STM32

The STM32 firmware use*:

- STM32 HAL
- GPIO
- I2C2
- UART
- Timers / PWM
- LIS3DH driver
- AHT20 communication
- OLED display routines

### Raspberr* Pi Pico W

The Pico W firmware us*s:

- Raspberry Pi Pico SDK
- CYW43 WiFi driver
- lwIP
- UART
- GPIO

##Project Structure

The repository contains firmware for two different microcontrollers:

```text
.
├── STM32/
│   └── STM32G431 firmware │
├── Pico/
│   └── Raspberry Pi Pico W firmware
│
└──*README.md
```

> The exact directory structure may vary depending on how the STM32CubeIDE and Pico SDK projects are stored*in the repository.

## Data Flow Summary

```text
Sensors
   │
   ▼
STM32G431
   │
   ├── Process sensor values
   ├── D*tect excessive movement*impact
   ├── Control cooling fan
   ├── Update OLED display
   │
   ▼
UART2   │
   ▼
Raspberry Pi Pico W
   │    ├── Parse STM32 data
   ├── Wi-Fi connectivity
   │
   ▼
Web Interface
   │
   ▼
User
```

## Future Improvements

Possible improv*ments include:

- Send LIS3DH acceleration data to the Pico W
- Display motion/impact alerts on the web interface
- Add historical temperature and humidity graphs
- Add configurable tempera*ure thresholds
- Improve the UART *rotocol with structured messages
- Add UART error checking
- Add sensor error handling
- Add remote fan status monitoring
- Store historical*sensor data
- Add timestamps to me*surements

## Purpose

This project demonstrates how two microcontrollers can divide responsibilities in an embedded IoT system.

The **STM32G431** focuses on sensor acquisition, real-time processing, display*output, alarms, and actuator*control, while the **Raspberry Pi pico W** provides wireless connectivity and exposes the collected information through a web interface.
