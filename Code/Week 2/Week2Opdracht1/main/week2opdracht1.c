#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void priorityTask(void* pvParamter)
{
    int priority = 5;

    while(1)
    {
        vTaskDelay(3000 / portTICK_RATE_MS);
        printf("Weer 3 seconden!\n");
        priority--;
        vTaskPrioritySet(NULL, priority);

        if(priority == 1)
            break;
    }

    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(&priorityTask, "priorityTask", 2048, NULL, 5, NULL);
}