#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BLOCK_COUNT 5
#define BLOCK_SIZE  32

// Simple fixed-size memory pool (static storage).
uint8_t memoryPool[BLOCK_COUNT][BLOCK_SIZE];
bool blockUsed[BLOCK_COUNT] = {false};

TaskHandle_t memoryTaskHandle = nullptr;

void *allocateBlock() {
  for (int i = 0; i < BLOCK_COUNT; i++) {
    if (!blockUsed[i]) {
      blockUsed[i] = true;
      return memoryPool[i];
    }
  }
  return nullptr;
}

void releaseBlock(void *ptr) {
  for (int i = 0; i < BLOCK_COUNT; i++) {
    if (ptr == memoryPool[i]) {
      blockUsed[i] = false;
      return;
    }
  }
}

void memoryTask(void *pv) {
  for (;;) {
    void *block = allocateBlock();

    if (block) {
      Serial.println("Memory Pool: allocated one 32-byte block");
      memset(block, 0x5A, BLOCK_SIZE);
      vTaskDelay(pdMS_TO_TICKS(500));
      releaseBlock(block);
      Serial.println("Memory Pool: block released");
    } else {
      Serial.println("Memory Pool: no free block available");
    }

    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void reportMemory() {
  Serial.printf("Free heap: %u bytes\n", (unsigned)ESP.getFreeHeap());
  Serial.printf("Minimum free heap: %u bytes\n", (unsigned)ESP.getMinFreeHeap());

  if (memoryTaskHandle) {
    UBaseType_t remaining = uxTaskGetStackHighWaterMark(memoryTaskHandle);
    Serial.printf("Memory task stack high-water mark: %u words\n", (unsigned)remaining);
  }

  int used = 0;
  for (int i = 0; i < BLOCK_COUNT; i++)
    if (blockUsed[i]) used++;

  Serial.printf("Memory pool blocks used: %d / %d\n", used, BLOCK_COUNT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nExperiment 20: RTOS Heap, Stack High-Water Mark and Memory Pool");

  xTaskCreatePinnedToCore(
    memoryTask, "MemoryTask", 3072, nullptr, 1, &memoryTaskHandle, 1);

  Serial.println("Memory monitoring task created.");
}

void loop() {
  reportMemory();
  Serial.println("------------------------------");
  vTaskDelay(pdMS_TO_TICKS(3000));
}
