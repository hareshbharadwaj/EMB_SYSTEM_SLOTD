#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/stream_buffer.h"

QueueHandle_t messageQueue;
QueueHandle_t mailboxQueue;       // A queue of length 1 acts as a mailbox
StreamBufferHandle_t streamBuffer;

struct Message {
  uint32_t number;
  char text[32];
};

void queueProducer(void *pv) {
  uint32_t n = 1;
  Message msg;
  for (;;) {
    msg.number = n++;
    snprintf(msg.text, sizeof(msg.text), "Message_%lu", (unsigned long)msg.number);
    if (xQueueSend(messageQueue, &msg, pdMS_TO_TICKS(100)) == pdPASS)
      Serial.printf("Queue Producer: sent %s\n", msg.text);
    else
      Serial.println("Queue Producer: queue full");
    vTaskDelay(pdMS_TO_TICKS(1200));
  }
}

void queueConsumer(void *pv) {
  Message msg;
  for (;;) {
    if (xQueueReceive(messageQueue, &msg, portMAX_DELAY) == pdPASS)
      Serial.printf("Queue Consumer: received %lu - %s\n",
                    (unsigned long)msg.number, msg.text);
  }
}

void mailboxWriter(void *pv) {
  int value = 100;
  for (;;) {
    int latestValue = value++;
    if (xQueueOverwrite(mailboxQueue, &latestValue) == pdPASS)
      Serial.printf("Mailbox: latest value written = %d\n", latestValue);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void mailboxReader(void *pv) {
  int value;
  for (;;) {
    if (xQueueReceive(mailboxQueue, &value, portMAX_DELAY) == pdPASS)
      Serial.printf("Mailbox: value read = %d\n", value);
  }
}

void streamWriter(void *pv) {
  const char *samples[] = {"TEMP=28;", "TEMP=29;", "TEMP=30;"};
  size_t i = 0;
  for (;;) {
    const char *data = samples[i++ % 3];
    size_t sent = xStreamBufferSend(streamBuffer, data, strlen(data), pdMS_TO_TICKS(100));
    Serial.printf("Stream Writer: sent %u bytes\n", (unsigned)sent);
    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void streamReader(void *pv) {
  char data[32];
  for (;;) {
    size_t received = xStreamBufferReceive(streamBuffer, data, sizeof(data) - 1, portMAX_DELAY);
    data[received] = '\0';
    Serial.printf("Stream Reader: received %s\n", data);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nExperiment 15: Queues, Mailbox and Stream Buffer");

  messageQueue = xQueueCreate(5, sizeof(Message));
  mailboxQueue = xQueueCreate(1, sizeof(int));
  streamBuffer = xStreamBufferCreate(128, 1);

  if (!messageQueue || !mailboxQueue || !streamBuffer) {
    Serial.println("ERROR: Communication object creation failed.");
    while (true) delay(1000);
  }

  xTaskCreatePinnedToCore(queueProducer, "QueueProducer", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(queueConsumer, "QueueConsumer", 3072, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(mailboxWriter, "MailboxWriter", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(mailboxReader, "MailboxReader", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(streamWriter, "StreamWriter", 3072, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(streamReader, "StreamReader", 3072, nullptr, 1, nullptr, 1);

  Serial.println("Queue, mailbox and stream buffer tasks started.");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
