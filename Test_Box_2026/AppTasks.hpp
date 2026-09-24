#pragma once

#include "Webserver.hpp"
#include "config.h"
#include "log.hpp"
#include "ProcessInterface.hpp"
#include "RelayController.hpp"

extern WebServerHandler webServer;
extern ProcessInterface* modoActual;
extern volatile bool flagtostart;
extern unsigned long testStartMs;
extern unsigned long testDurationMs;

TaskHandle_t webServerTaskHandle = NULL;
TaskHandle_t processTaskHandle = NULL;
SemaphoreHandle_t wifiMutex = NULL;

void webServerTask(void *parameter) {
  Serial.println("[FREERTOS] Tarea Web Server iniciada");

  while (1) {
    webServer.handleClient();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void processTask(void *parameter) {
  Serial.println("[FREERTOS] Tarea Proceso iniciada");

  while (1) {
    if (flagtostart) {
      if (testDurationMs > 0 && millis() - testStartMs >= testDurationMs) {
        flagtostart = false;
        RelaysInitialState();
        addLogEvent("[FIN TEST] Duración alcanzada");
        Serial.println("Test finalizado por duración");
      } else if (modoActual != nullptr) {
        modoActual->execute();
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void startAppTasks() {
  wifiMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    webServerTask,
    "WebServerTask",
    4096,
    NULL,
    1,
    &webServerTaskHandle,
    1
  );

  xTaskCreatePinnedToCore(
    processTask,
    "ProcessTask",
    4096,
    NULL,
    2,
    &processTaskHandle,
    0
  );

  Serial.println("[FREERTOS] Tareas creadas y ejecutándose");
}
