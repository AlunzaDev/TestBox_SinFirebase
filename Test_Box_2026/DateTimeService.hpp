#pragma once

#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "config.h"

String getDateTimeString() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  const char* url = "https://api.timezonedb.com/v2.1/get-time-zone?key=9W5GP4UHSQYY&format=json&by=zone&zone=America/Mexico_City";
  if (!https.begin(client, url)) {
    Serial.println("Error al iniciar conexión HTTPS");
    return "Fecha no disponible";
  }

  int httpCode = https.GET();
  if (httpCode != 200) {
    Serial.printf("Error HTTP: %d\n", httpCode);
    https.end();
    return "Fecha no disponible";
  }

  String payload = https.getString();
  https.end();

  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, payload) != DeserializationError::Ok) {
    Serial.println("Error al parsear JSON");
    return "Fecha no disponible";
  }

  String dateTime = doc["formatted"];
  int year = dateTime.substring(0, 4).toInt();
  int month = dateTime.substring(5, 7).toInt();
  int day = dateTime.substring(8, 10).toInt();
  int hour = dateTime.substring(11, 13).toInt();
  int minute = dateTime.substring(14, 16).toInt();
  int second = dateTime.substring(17, 19).toInt();

  fechaArr[0] = day;
  fechaArr[1] = month;
  fechaArr[2] = year;
  horaArr[0] = hour;
  horaArr[1] = minute;
  horaArr[2] = second;

  char buffer[20];
  snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d",
           day, month, year, hour, minute, second);

  return String(buffer);
}
