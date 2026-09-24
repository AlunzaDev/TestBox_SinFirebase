#pragma once

#include <WiFi.h>
#include "config.h"

bool wifiConnecting = false;

void wifiBegin() {
  if (wifiConnecting) return;
  wifiConnecting = true;

  IPAddress local_ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);

  if (!WiFi.softAPConfig(local_ip, gateway, subnet)) {
    Serial.println("Error al configurar IP estática");
  }

  if (WiFi.softAP(apSsid.c_str(), apPassword.c_str())) {
    digitalWrite(WifiConn, HIGH);
    Serial.println("Punto de acceso iniciado.");
    Serial.print("SSID: ");
    Serial.println(apSsid);
    Serial.print("IP del ESP: ");
    Serial.println(WiFi.softAPIP());
    wifiConnectedPreviously = true;
  } else {
    Serial.println("No se pudo iniciar el punto de acceso.");
    digitalWrite(WifiConn, LOW);
    wifiConnectedPreviously = false;
  }

  wifiConnecting = false;
}
