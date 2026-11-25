#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t resourceMutex;

void lowPriorityTask(void *pv) {
  for (;;) {
    Serial.println("LOW: waiting to acquire shared resource");
    xSemaphoreTake(resourceMutex, portMAX_DELAY);
    Serial.println("LOW: acquired mutex; using shared resource");

    // Keep the mutex for a short period so HIGH can become blocked on it.
    vTaskDelay(pdMS_TO_TICKS(2000));

    Serial.println("LOW: releasing mutex");
    xSemaphoreGive(resourceMutex);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void mediumPriorityTask(void *pv) {
  for (;;) {
    Serial.println("MEDIUM: running independent work");
    // Periodic work allows the scheduler to run the other tasks.
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

void highPriorityTask(void *pv) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.println("HIGH: requesting shared resource");
    xSemaphoreTake(resourceMutex, portMAX_DELAY);
    Serial.println("HIGH: acquired mutex after LOW released it");
    xSemaphoreGive(resourceMutex);
    vTaskDelay(pdMS_TO_TICKS(4000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nExperiment 19: Priority Inversion and Priority Inheritance");

  resourceMutex = xSemaphoreCreateMutex();
  if (!resourceMutex) {
    Serial.println("ERROR: Mutex creation failed.");
    while (true) delay(1000);
  }

  // All tasks run on the same core to make scheduling easier to observe.
  xTaskCreatePinnedToCore(lowPriorityTask, "LOW", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(mediumPriorityTask, "MEDIUM", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(highPriorityTask, "HIGH", 3072, nullptr, 3, nullptr, 1);

  Serial.println("LOW=1, MEDIUM=2, HIGH=3. FreeRTOS mutex provides priority inheritance.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
