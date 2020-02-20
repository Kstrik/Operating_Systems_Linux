#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <freertos/semphr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <stdbool.h>

typedef struct
{
    char* data;
    int size;
    int length;
} Buffer;

Buffer buffer;
SemaphoreHandle_t writerHandle;

int readerCount;

// Write data to buffer
void write_data(const char data)
{
    xSemaphoreTake(writerHandle, portMAX_DELAY);
    if(buffer.length == buffer.size - 1)
    {
        buffer.size += 10;
        buffer.data = (int*)realloc(buffer.data, sizeof(int) * buffer.size);
    }
    buffer.data[buffer.length] = data;
    buffer.data[buffer.length + 1] = '\0';
    buffer.length++;

    printf("Wrote data!\n");
    printf("Bufferlength: %i\n", buffer.length);
    xSemaphoreGive(writerHandle);
}

// Read data form buffer
void read_data()
{
    printf("Read data: %s\n", buffer.data);
}

// Task for writer to write data to the buffer
void writerTask(void* pvParameter)
{
    while(1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
        write_data('d');
    }
    vTaskDelete(NULL);
}

// Task for reader to read data from the buffer
void readerTask(void* pvParameter)
{
    SemaphoreHandle_t readerHandle = xSemaphoreCreateMutex();

    while(1)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
        xSemaphoreTake(readerHandle, portMAX_DELAY);
        readerCount++;
        if(readerCount == 1)
            xSemaphoreTake(writerHandle, portMAX_DELAY);
        xSemaphoreGive(readerHandle);
        read_data();
        xSemaphoreTake(readerHandle, portMAX_DELAY);
        readerCount--;
        if(readerCount == 0)
            xSemaphoreGive(writerHandle);
        xSemaphoreGive(readerHandle);
    }
    vTaskDelete(NULL);
}

void app_main()
{
    readerCount = 0;

    // Init buffer
    buffer.data = (int*)malloc(sizeof(int) * 11);
    buffer.size = 11;
    buffer.length = 0;
    buffer.data[0] = '\0';

    // Create mutex
    writerHandle = xSemaphoreCreateMutex();

    // Create tasks
    xTaskCreate(&writerTask, "WriterTask", 2048, NULL, 10, NULL);
    xTaskCreate(&readerTask, "ReaderTask", 2048, NULL, 5, NULL);
}