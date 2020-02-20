#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void red(void* pvParameter)
{
    gpio_pad_select_gpio(13);
    gpio_set_direction(13, GPIO_MODE_OUTPUT);
    while(1)
    {
        gpio_set_level(13, 1);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(13, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void green(void* pvParamter)
{
    gpio_pad_select_gpio(12);
    gpio_set_direction(12, GPIO_MODE_OUTPUT);
    while(1)
    {
        gpio_set_level(12, 1);
        vTaskDelay(200 / portTICK_RATE_MS);
        gpio_set_level(12, 0);
        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

void app_main()
{
    xTaskCreate(&red, "redTask", 2048, NULL, 5, NULL);
    xTaskCreate(&green, "greenTask", 2048, NULL, 5, NULL);
}