#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <freertos/semphr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

typedef struct
{
    int* data;
    int size;
    int length;
} Buffer;

Buffer buffer;
SemaphoreHandle_t semaphoreHandle;

// Add data to buffer
void add_data(int data)
{
    xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
    if(buffer.length == buffer.size)
        return;
    buffer.data[buffer.length] = data;
    buffer.length++;

    printf("Added data!\n");
    printf("Bufferlength: %i\n", buffer.length);
    xSemaphoreGive(semaphoreHandle);
}

// Remove data from buffer
void remove_data(int index)
{
    xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
    if(buffer.length != 0)
        buffer.length--;

    printf("Removed data!\n");
    printf("Bufferlength: %i\n", buffer.length);
    xSemaphoreGive(semaphoreHandle);
}

// Get data form buffer on a specific index
int* get_data(int index)
{
    xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
    if(buffer.length != 0 && index < buffer.length)
        return buffer.data[index];
    return NULL;
    xSemaphoreGive(semaphoreHandle);
}

// Task for producer to add data to the buffer
void producerTask(void* pvParameter)
{
    while(1)
    {
        srand(time(0));
        vTaskDelay((rand() % 1000) / portTICK_RATE_MS);
        add_data(10);
    }
    vTaskDelete(NULL);
}

// Task for customer to remove data in the buffer
void consumerTask(void* pvParameter)
{
    while(1)
    {
        srand(time(0));
        vTaskDelay((rand() % 1000) / portTICK_RATE_MS);
        remove_data(1);
    }
    vTaskDelete(NULL);
}

void app_main()
{
    // Init buffer
    buffer.data = (int*)malloc(sizeof(int) * 10);
    buffer.size = 10;
    buffer.length = 0;

    // Create mutex
    semaphoreHandle = xSemaphoreCreateMutex();

    // Create tasks
    xTaskCreate(&producerTask, "ProducerTask", 2048, NULL, 5, NULL);
    xTaskCreate(&consumerTask, "ConsumerTask", 2048, NULL, 5, NULL);
}