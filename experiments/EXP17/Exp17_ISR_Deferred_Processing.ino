#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define INTERRUPT_PIN 4

QueueHandle_t interruptQueue;
volatile uint32_t interruptCount = 0;

void IRAM_ATTR buttonISR() {
  BaseType_t higherPriorityTaskWoken = pdFALSE;
  uint32_t eventNumber = ++interruptCount;

  // ISR-safe API: do not use Serial, delay, or normal queue functions here.
  xQueueSendFromISR(interruptQueue, (void *)&eventNumber, &higherPriorityTaskWoken);

  if (higherPriorityTaskWoken == pdTRUE)
    portYIELD_FROM_ISR();
}

void interruptProcessingTask(void *pv) {
  uint32_t eventNumber;
  for (;;) {
    if (xQueueReceive(interruptQueue, &eventNumber, portMAX_DELAY) == pdPASS) {
      Serial.printf("Deferred Task: processed interrupt event %lu\n",
                    (unsigned long)eventNumber);
      Serial.println("Deferred Task: ISR work completed outside interrupt context");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);

  interruptQueue = xQueueCreate(10, sizeof(uint32_t));
  if (!interruptQueue) {
    Serial.println("ERROR: Interrupt queue creation failed.");
    while (true) delay(1000);
  }

  xTaskCreatePinnedToCore(
    interruptProcessingTask, "ISR_Deferred_Task", 3072, nullptr, 3, nullptr, 1);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), buttonISR, FALLING);

  Serial.println("\nExperiment 17: ISR Deferred Processing");
  Serial.println("Press the push button between GPIO 4 and GND.");
  Serial.println("The ISR sends an event to a queue; a task performs the processing.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
