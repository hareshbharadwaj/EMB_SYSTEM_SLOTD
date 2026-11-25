#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t binarySem;
SemaphoreHandle_t countingSem;
SemaphoreHandle_t counterMutex;
SemaphoreHandle_t serialMutex;

int sharedCounter = 0;

void logLine(const char *msg) {
  xSemaphoreTake(serialMutex, portMAX_DELAY);
  Serial.println(msg);
  xSemaphoreGive(serialMutex);
}

void binarySender(void *pv) {
  for (;;) {
    logLine("Binary Sender: giving signal");
    xSemaphoreGive(binarySem);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void binaryReceiver(void *pv) {
  for (;;) {
    if (xSemaphoreTake(binarySem, portMAX_DELAY) == pdTRUE)
      logLine("Binary Receiver: signal received");
  }
}

void producer(void *pv) {
  for (;;) {
    logLine("Counting Producer: trying to add an item");
    if (xSemaphoreGive(countingSem) == pdTRUE)
      logLine("Counting Producer: item added");
    else
      logLine("Counting Producer: semaphore is full");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void consumer(void *pv) {
  for (;;) {
    if (xSemaphoreTake(countingSem, portMAX_DELAY) == pdTRUE)
      logLine("Counting Consumer: item consumed");
    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void counterTask1(void *pv) {
  for (;;) {
    xSemaphoreTake(counterMutex, portMAX_DELAY);
    int value = ++sharedCounter;
    xSemaphoreTake(serialMutex, portMAX_DELAY);
    Serial.printf("Mutex Task 1: shared counter = %d\n", value);
    xSemaphoreGive(serialMutex);
    xSemaphoreGive(counterMutex);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void counterTask2(void *pv) {
  for (;;) {
    xSemaphoreTake(counterMutex, portMAX_DELAY);
    int value = ++sharedCounter;
    xSemaphoreTake(serialMutex, portMAX_DELAY);
    Serial.printf("Mutex Task 2: shared counter = %d\n", value);
    xSemaphoreGive(serialMutex);
    xSemaphoreGive(counterMutex);
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nExperiment 14: Binary Semaphore, Counting Semaphore and Mutex");

  binarySem = xSemaphoreCreateBinary();
  countingSem = xSemaphoreCreateCounting(5, 0);
  counterMutex = xSemaphoreCreateMutex();
  serialMutex = xSemaphoreCreateMutex();

  if (!binarySem || !countingSem || !counterMutex || !serialMutex) {
    Serial.println("ERROR: Semaphore or mutex creation failed.");
    while (true) delay(1000);
  }

  xTaskCreatePinnedToCore(binarySender, "BinarySender", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(binaryReceiver, "BinaryReceiver", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(producer, "Producer", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(consumer, "Consumer", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(counterTask1, "MutexTask1", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(counterTask2, "MutexTask2", 3072, nullptr, 1, nullptr, 1);

  Serial.println("All synchronization objects and tasks created.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
