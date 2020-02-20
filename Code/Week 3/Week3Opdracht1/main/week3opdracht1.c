#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <freertos/semphr.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    THINKING,
    HUNGRY,
    EATING
} state;

typedef struct
{
    state state;
} philosopher;

typedef struct
{
    bool isDown;
} fork;

philosopher philosophers[5];
fork forks[5];
SemaphoreHandle_t semaphoreHandles[5];

// Gets the correct index to the left of the index specified
int get_left_index(int index)
{
    if(index == 0)
        return 4;
    return index - 1;
}

// Gets the correct index to the right of the index specified
int get_right_index(int index)
{
    if(index == 4)
        return 0;
    return index + 1;
}

/* Checks if left and right side philosophers are not eating and if the philosophers himself 
    is hungry to then pickup a fork and start eating */
void eat(int philosopher)
{
    if(philosophers[philosopher].state == HUNGRY && 
        philosophers[get_left_index(philosopher)].state != EATING && 
        philosophers[get_right_index(philosopher)].state != EATING)
    {
        philosophers[philosopher].state = EATING;
        forks[philosopher].isDown = false;
        printf("Philosopher %i eating\n", philosopher);
    }
}

// Takes a fork and start eating if a fork is available
void take_forks(int philosopher)
{
    xSemaphoreTake(semaphoreHandles[philosopher], portMAX_DELAY);
    philosophers[philosopher].state = HUNGRY;
    eat(philosopher);
    xSemaphoreGive(semaphoreHandles[philosopher]);
    forks[philosopher].isDown = true;
}

// Puts a fork down after eating
void put_forks(int philosopher)
{
    xSemaphoreTake(semaphoreHandles[philosopher], portMAX_DELAY);
    philosophers[philosopher].state = THINKING;
    printf("Philosopher %i thinking\n", philosopher);
    eat(get_left_index(philosopher));
    eat(get_right_index(philosopher));
    xSemaphoreGive(semaphoreHandles[philosopher]);
}

// Represents the task of a philosophere to constantly take a fork if available and start eating to then put it down when done
void philosopherTask(void* pvParameter)
{
    int number = *((int*)pvParameter);
    // Create semaphore for this philosopher
    semaphoreHandles[number] = xSemaphoreCreateMutex();

    while(1)
    {
        philosophers[number].state = THINKING;
        take_forks(number);
        vTaskDelay(1000 / portTICK_RATE_MS);
        put_forks(number);
    }

    vTaskDelete(NULL);
}

// Combines to strings together
char* concat_strings(const char* a, char* b)
{
    char* string = (char*)malloc(strlen(a) + strlen(b) + 1);
    strcpy(string, a);
    strcat(string, b);
    return string;
}

void app_main()
{
    // Put all forks down
    for(int i = 0; i < 5; i++)
        forks[i].isDown = true;

    // Create tasks
    for(int j = 0; j < 5; j++)
    {
        char taskNumber[3];
        itoa(j, taskNumber, 10);
        char* taskName = concat_strings("Task", taskNumber);
        // Create task
        xTaskCreate(&philosopherTask, taskName, 2048, &j, 5, NULL);
        //Set delay so value can of j can be read by task before it is canged in the next iteration
        vTaskDelay(1 / portTICK_RATE_MS);
    }
}