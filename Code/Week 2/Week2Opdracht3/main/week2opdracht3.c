#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void task(void* pvParameter)
{
    char* name = (char*)pvParameter;
    while(1)
    {
        printf("%s\n", name);
        
        //Setting a delay on the task if it is the first one to allow other tasks to execute
        if(strstr(name, "Task0") != NULL)
            vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

char* concat_strings(const char* a, char* b)
{
    char* string = (char*)malloc(strlen(a) + strlen(b) + 1);
    strcpy(string, a);
    strcat(string, b);
    return string;
}

void app_main()
{
    for(int i = 0; i < 20; i++)
    {
        char taskNumber[3];
        itoa(i, taskNumber, 10);
        char* taskName = concat_strings("Task", taskNumber);

        /*
            In this case starvation will occure due to the first task having a higher priority than the other tasks.
            This can only be evaded bij setting a delay in the task itself to make it wait a certain ammount of time 
            or by gradually decreasing its priority and setting it back to a higher priority after reaching a low enough priority.
        */
        if(i == 0)
            xTaskCreate(&task, taskName, 2048, taskName, 10, NULL);
        else
            xTaskCreate(&task, taskName, 2048, taskName, 5, NULL);
    }
}