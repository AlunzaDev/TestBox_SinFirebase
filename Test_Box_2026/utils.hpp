// // ========================== FUNCIONES PARA LOS PROCESOS ============================
// #pragma once

// #include "log.hpp"
// #include <Preferences.h>
// #include <ArduinoJson.h>
// #include <WiFiClientSecure.h>
// #include <HTTPClient.h>

// extern Preferences prefs;
// bool wifiConnecting = false;

// // ========================== ALMACENAMIENTO LOCAL ============================

// // Inicializa almacenamiento local y recupera conteo previo
// void storageInit() {
//   prefs.begin("conteos", false);  // false = modo lectura/escritura
//   counter = prefs.getInt("contador", 0);
//   Serial.print("Conteo recuperado: ");
//   Serial.println(counter);
// }

// // Incrementa contador y guarda en memoria no volátil
// void guardarConteoLocal() {
//   counter++;
//   prefs.putInt("contador", counter);
//   Serial.print("Nuevo conteo guardado: ");
//   Serial.println(counter);
//   etapa = 0;
// }

// void cargarDatos() {
//   counter = prefs.getInt("contador", 0);
//   Serial.print("Contador cargado desde memoria: ");
//   Serial.println(counter);
// }


// String getDateTimeString() {
//   WiFiClientSecure client;
//   client.setInsecure();  // Evita validar certificados (solo para testing)

//   HTTPClient https;
//   const char* url = "https://api.timezonedb.com/v2.1/get-time-zone?key=9W5GP4UHSQYY&format=json&by=zone&zone=America/Mexico_City";
//   if (!https.begin(client, url)) {
//     Serial.println("Error al iniciar conexión HTTPS");
//     return "Fecha no disponible";
//   }

//   int httpCode = https.GET();
//   if (httpCode != 200) {
//     Serial.printf("Error HTTP: %d\n", httpCode);
//     https.end();
//     return "Fecha no disponible";
//   }

//   String payload = https.getString();
//   https.end();

//   // Parse JSON
//   DynamicJsonDocument doc(1024);
//   if (deserializeJson(doc, payload) != DeserializationError::Ok) {
//     Serial.println("Error al parsear JSON");
//     return "Fecha no disponible";
//   }

//   String dateTime = doc["formatted"];  // Ej: "2025-10-07 14:05:12"

//   // Extraer partes
//   int year = dateTime.substring(0, 4).toInt();
//   int month = dateTime.substring(5, 7).toInt();
//   int day = dateTime.substring(8, 10).toInt();
//   int hour = dateTime.substring(11, 13).toInt();
//   int minute = dateTime.substring(14, 16).toInt();
//   int second = dateTime.substring(17, 19).toInt();

//   fechaArr[0] = day;
//   fechaArr[1] = month;
//   fechaArr[2] = year;
//   horaArr[0] = hour;
//   horaArr[1] = minute;
//   horaArr[2] = second;

//   char buffer[20];
//   snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d",
//            day, month, year, hour, minute, second);

//   return String(buffer);
// }

// void RelaysOp() {
//   digitalWrite(Relay1, HIGH);
//   digitalWrite(Relay3, HIGH);
// }

// void RelaysCl() {
//   digitalWrite(Relay1, LOW);
//   digitalWrite(Relay3, LOW);
// }

// void RelaysAct() {
//   digitalWrite(Relay2, HIGH);  //HIGH
//   digitalWrite(Relay4, HIGH);  //HIGH
// }

// // Desactiva los cuatro relés.
// void RelaysDes() {
//   digitalWrite(Relay2, LOW);  //LOW
//   digitalWrite(Relay4, LOW);  //LOW
// }

// void RelaysInitialState() {
//   digitalWrite(Relay1, HIGH);
//   digitalWrite(Relay2, HIGH);
//   digitalWrite(Relay3, HIGH);
//   digitalWrite(Relay4, HIGH);
// }


// // Verifica si el botón de pausa fue presionado y actualiza el estado del sistema
// void actualizarEstadoPausa() {
//   bool estadoActual = digitalRead(Pause);
//   if (estadoActual == LOW && ultimoEstadoPausa == true) {
//     pausaActiva = !pausaActiva;  // Cambia de estado al presionar el botón
//     String timestamp = getDateTimeString();
//     if (pausaActiva) {
//       digitalWrite(ledV, HIGH);
//       digitalWrite(ledPause, LOW);
//       addLogEvent("[Pausado] " + timestamp);
//       Serial.println("Proceso pausado.");
//     } else {
//       digitalWrite(ledV, LOW);
//       digitalWrite(ledPause, HIGH);
//       addLogEvent("[Reanudado] " + timestamp);
//       Serial.println("Proceso reanudado.");
//     }
//     delay(200);  // Antirebote
//   }
//   ultimoEstadoPausa = estadoActual;
// }

// // Espera `limite` segundos con control de pausa y cambia a `sigEtapa` al finalizar
// void esperar(int limite, int sigEtapa) {
//   int segundosTranscurridos = 0;

//   while (segundosTranscurridos <= limite) {
//     actualizarEstadoPausa();  // Verifica si se presionó el botón de pausa

//     if (pausaActiva) {
//       Serial.println("Pausa detectada durante espera.");
//       // Espera aquí hasta que se quite la pausa
//       while (pausaActiva) {
//         actualizarEstadoPausa();  // Sigue escuchando el botón para reanudar
//         delay(100);
//       }
//       Serial.println("Reanudando espera...");
//     }

//     if (interruptCounter > 0) {
//       Serial.print("SEGUNDO: ");
//       Serial.println(segundosTranscurridos);
//       portENTER_CRITICAL(&timerMux);
//       interruptCounter = 0;
//       portEXIT_CRITICAL(&timerMux);
//       segundosTranscurridos++;
//     }
//     delay(5);  // Evita saturar la CPU
//   }

//   etapa = sigEtapa;
// }

// void wifiBegin() {
//   if (wifiConnecting) return;  // Ya hay un intento en curso
//   wifiConnecting = true;

//   // Red local independiente: no requiere router ni conexión a Internet.
//   IPAddress local_ip(192, 168, 4, 1);
//   IPAddress gateway(192, 168, 4, 1);
//   IPAddress subnet(255, 255, 255, 0);

//   WiFi.mode(WIFI_AP);

//   if (!WiFi.softAPConfig(local_ip, gateway, subnet)) {
//     Serial.println("Error al configurar IP estática");
//   }

//   if (WiFi.softAP(apSsid.c_str(), apPassword.c_str())) {
//     digitalWrite(WifiConn, HIGH);
//     Serial.println("Punto de acceso iniciado.");
//     Serial.print("SSID: ");
//     Serial.println(apSsid);
//     Serial.print("IP del ESP: ");
//     Serial.println(WiFi.softAPIP());
//     wifiConnectedPreviously = true;
//   } else {
//     Serial.println("No se pudo iniciar el punto de acceso.");
//     digitalWrite(WifiConn, LOW);
//     wifiConnectedPreviously = false;
//   }

//   wifiConnecting = false;  // Marca fin del intento
// }
