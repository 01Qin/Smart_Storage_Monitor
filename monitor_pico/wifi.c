//
// Created by Quinn on 8/11/2026.
//

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
// #include "lwipopts.h"
#include "wifi.h"
#include "lwip/apps/httpd.h"


void wifi_init() {

    // Attempt to connect to wifi network
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 3000)) {
            printf("Attempt to connect...\n");
    }
    // Print the success message
    printf("Connected!\n");

    // Initialise server
    httpd_init();
    printf("Http server initialised!\n");


}
