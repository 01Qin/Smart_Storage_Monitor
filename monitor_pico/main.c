#include <stdio.h>

#include "wifi.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "lwip/ip_addr.h"

#define UART_ID uart1
#define BAUDRATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9



int main() {
    // Initialize chosen serial port
    stdio_init_all();
    cyw43_arch_init(); // initialise wifi chip
    cyw43_arch_enable_sta_mode(); // enable wifi station mode


    sleep_ms(200);
    printf("pico!\n");

    uart_init(UART_ID, BAUDRATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    bool led_state = false;
    char rx_buf[64];
    size_t rx_idx = 0;


    // Loop forever
    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || c == '\r') {
                if (rx_idx > 0) {
                    rx_buf[rx_idx] = '\0';

                    float temp = 0.0f;
                    float hum = 0.0f;
                    int speed = 0;

                    if (sscanf(rx_buf, "Temp:%f C Hum:%f%% Fan speed:%d", &temp, &hum, &speed)) {
                        if (temp >= 25.50 ) {
                            printf("\nTemp: %.2f C, Hum: %.2f%%\n", temp, hum);
                            printf("The fan is turned on, speed is: %d\n", speed);
                        } else {
                            printf("\nTemp: %.2f C, Hum: %.2f%%\n", temp, hum);
                        }
                    }
                    // Toggle the onboard LED every time a full message arrives from the STM32
                    led_state = !led_state;
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
                    rx_idx = 0;
                }

            }
            else if (rx_idx < sizeof (rx_buf) - 1) {
                rx_buf[rx_idx++] = c;
            }
        }
    }
}
