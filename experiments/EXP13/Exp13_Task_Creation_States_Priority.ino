#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_PIN 2

TaskHandle_t ledTaskHandle = nullptr;
TaskHandle_t counterTaskHandle = nullptr;
TaskHandle_t monitorTaskHandle = nullptr;

const char *stateName(eTaskState state) {
  switch (state) {
    case eRunning:   return "RUNNING";
    case eReady:     return "READY";
    case eBlocked:   return "BLOCKED";
    case eSuspended: return "SUSPENDED";
    case eDeleted:   return "DELETED";
    default:         return "INVALID";
  }
}

void ledTask(void *pv) {
  pinMode(LED_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Serial.println("LED task: toggled LED");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void counterTask(void *pv) {
  uint32_t count = 0;
  for (;;) {
    Serial.printf("Counter task: count = %lu\n", (unsigned long)++count);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void monitorTask(void *pv) {
  for (;;) {
    Serial.println("Monitor task: system running");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void printTaskStates() {
  Serial.println("\n--- Task states ---");
  if (ledTaskHandle)
    Serial.printf("LED task: %s, priority %u\n",
      stateName(eTaskGetState(ledTaskHandle)),
      (unsigned)uxTaskPriorityGet(ledTaskHandle));
  if (counterTaskHandle)
    Serial.printf("Counter task: %s, priority %u\n",
      stateName(eTaskGetState(counterTaskHandle)),
      (unsigned)uxTaskPriorityGet(counterTaskHandle));
  if (monitorTaskHandle)
    Serial.printf("Monitor task: %s, priority %u\n",
      stateName(eTaskGetState(monitorTaskHandle)),
      (unsigned)uxTaskPriorityGet(monitorTaskHandle));
  Serial.println("-------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nExperiment 13: FreeRTOS Task Creation, States and Priorities");

  BaseType_t a = xTaskCreatePinnedToCore(ledTask, "LED_Task", 2048, nullptr, 3, &ledTaskHandle, 1);
  BaseType_t b = xTaskCreatePinnedToCore(counterTask, "Counter_Task", 2048, nullptr, 2, &counterTaskHandle, 1);
  BaseType_t c = xTaskCreatePinnedToCore(monitorTask, "Monitor_Task", 2048, nullptr, 1, &monitorTaskHandle, 1);

  if (a != pdPASS || b != pdPASS || c != pdPASS) {
    Serial.println("ERROR: One or more tasks could not be created.");
    return;
  }

  Serial.println("Three tasks created successfully.");
  Serial.println("Serial commands: s=suspend LED, r=resume LED, p=set LED priority 4, t=show states");
}

void loop() {
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 's' && ledTaskHandle) {
      vTaskSuspend(ledTaskHandle);
      Serial.println("LED task suspended.");
    } else if (ch == 'r' && ledTaskHandle) {
      vTaskResume(ledTaskHandle);
      Serial.println("LED task resumed.");
    } else if (ch == 'p' && ledTaskHandle) {
      vTaskPrioritySet(ledTaskHandle, 4);
      Serial.println("LED task priority changed to 4.");
    } else if (ch == 't') {
      printTaskStates();
    }
  }
  vTaskDelay(pdMS_TO_TICKS(50));
}
