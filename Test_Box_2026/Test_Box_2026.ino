//VERSIÓN FINAL CÓDIGO TEST BOX CON FREERTOS

#include <WiFi.h>
#include <HTTPClient.h>
#include <TimeLib.h>
#include <Update.h>
#include "config.h"
#include "log.hpp"
#include "Webserver.hpp"
#include "Eventos.hpp"
#include "NetworkManager.hpp"
#include "StorageService.hpp"
#include "ProcessInterface.hpp"
#include "ModoBarreraSimple.hpp"
#include "ModoBarreraLazo.hpp"
#include "AppTasks.hpp"
#include <Preferences.h>

Preferences prefs;

WebServerHandler webServer;
ProcessInterface* modoActual = nullptr;
ModoBarreraSimple modoSimple;
ModoBarreraLazo modoLazo;
Eventos eventos;

extern int tiempoAbrir;
extern int tiempoCerrar;
extern int counter;
extern volatile bool flagtostart;
extern bool pausaActiva;
extern int etapa;

// ========================== CONFIGURACIÓN ===============================
void setup() {
  Serial.begin(115200);
  prefs.begin("conteos", false);
  pinMode(ledPause, OUTPUT);
  pinMode(ledV, OUTPUT);
  pinMode(ledOn, OUTPUT);
  pinMode(Pause, INPUT_PULLUP);
  pinMode(WifiConn, OUTPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);
  digitalWrite(Relay3, HIGH);
  digitalWrite(Relay4, HIGH);
  digitalWrite(ledPause, LOW);
  digitalWrite(ledV, LOW);
  digitalWrite(ledOn, LOW);
  digitalWrite(WifiConn, LOW);
  
  modo = 0; 
  modoActual = &modoSimple;
  
  // Configurar tiempos iniciales
  modoSimple.setTiempoAbrir(tiempoAbrir);
  modoSimple.setTiempoCerrar(tiempoCerrar);
  
  modoLazo.setTiempoAbrir(tiempoAbrir);
  modoLazo.setTiempoCerrar(tiempoCerrar);
  
  //Configurar el modo actual
  modoActual->setTiempoAbrir(tiempoAbrir);
  modoActual->setTiempoCerrar(tiempoCerrar);

  wifiBegin();
  webServer.begin();
  initTimer();
  cargarDatos();
  
  startAppTasks();
}

// ========================== LOOP ===============================
void loop() {
  digitalWrite(ledOn, HIGH);
  
  // Pequeño delay para no saturar el loop
  delay(1000);
  
  // Imprimir información de depuración
  // Serial.print("[DEBUG] WiFi: ");
  // Serial.print(WiFi.status() == WL_CONNECTED ? "Conectado" : "Desconectado");
  // Serial.print(" | flagtostart: ");
  // Serial.print(flagtostart);
  // Serial.print(" | pausaActiva: ");
  // Serial.println(pausaActiva);
}
