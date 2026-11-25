#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#define EVENT_BUTTON (1 << 0)
#define EVENT_SENSOR (1 << 1)
#define ALL_EVENTS   (EVENT_BUTTON | EVENT_SENSOR)

TimerHandle_t periodicTimer;
EventGroupHandle_t eventGroup;

void timerCallback(TimerHandle_t timer) {
  Serial.println("Software Timer: periodic callback executed");
}

void buttonTask(void *pv) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("Button Task: setting BUTTON event");
    xEventGroupSetBits(eventGroup, EVENT_BUTTON);
  }
}

void sensorTask(void *pv) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println("Sensor Task: setting SENSOR event");
    xEventGroupSetBits(eventGroup, EVENT_SENSOR);
  }
}

void eventMonitorTask(void *pv) {
  for (;;) {
    EventBits_t bits = xEventGroupWaitBits(
      eventGroup, ALL_EVENTS, pdTRUE, pdTRUE, portMAX_DELAY);

    if ((bits & ALL_EVENTS) == ALL_EVENTS)
      Serial.println("Event Monitor: both BUTTON and SENSOR events received");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nExperiment 16: FreeRTOS Software Timers and Event Groups");

  eventGroup = xEventGroupCreate();
  periodicTimer = xTimerCreate(
    "PeriodicTimer", pdMS_TO_TICKS(2000), pdTRUE, nullptr, timerCallback);

  if (!eventGroup || !periodicTimer) {
    Serial.println("ERROR: Timer or event group creation failed.");
    while (true) delay(1000);
  }

  xTimerStart(periodicTimer, 0);
  xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(sensorTask, "SensorTask", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(eventMonitorTask, "EventMonitor", 3072, nullptr, 2, nullptr, 1);

  Serial.println("Timer started; event tasks created.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
