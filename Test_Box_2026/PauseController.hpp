#pragma once

#include "config.h"
#include "DateTimeService.hpp"
#include "RelayController.hpp"
#include "log.hpp"

void actualizarEstadoPausa() {
  bool estadoActual = digitalRead(Pause);
  if (estadoActual == LOW && ultimoEstadoPausa == true) {
    pausaActiva = !pausaActiva;
    String timestamp = getDateTimeString();
    if (pausaActiva) {
      digitalWrite(ledV, HIGH);
      digitalWrite(ledPause, LOW);
      addLogEvent("[Pausado] " + timestamp);
      Serial.println("Proceso pausado.");
    } else {
      digitalWrite(ledV, LOW);
      digitalWrite(ledPause, HIGH);
      addLogEvent("[Reanudado] " + timestamp);
      Serial.println("Proceso reanudado.");
    }
    delay(200);
  }
  ultimoEstadoPausa = estadoActual;
}

void esperar(int limite, int sigEtapa) {
  int segundosTranscurridos = 0;

  while (segundosTranscurridos <= limite) {
    actualizarEstadoPausa();

    if (pausaActiva) {
      Serial.println("Pausa detectada durante espera.");
      while (pausaActiva) {
        actualizarEstadoPausa();
        delay(100);
      }
      Serial.println("Reanudando espera...");
    }

    if (interruptCounter > 0) {
      Serial.print("SEGUNDO: ");
      Serial.println(segundosTranscurridos);
      portENTER_CRITICAL(&timerMux);
      interruptCounter = 0;
      portEXIT_CRITICAL(&timerMux);
      segundosTranscurridos++;
    }
    delay(5);
  }

  etapa = sigEtapa;
}
