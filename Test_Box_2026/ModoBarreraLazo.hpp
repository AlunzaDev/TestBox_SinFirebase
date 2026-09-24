#pragma once

#include "ProcessInterface.hpp"
#include "PauseController.hpp"
#include "RelayController.hpp"
#include "StorageService.hpp"

class ModoBarreraLazo : public ProcessInterface {
private:
  int tiempoAbrir;
  int tiempoCerrar;
  int tiempoLazo;
  int etapaInterna;
  bool pausaActivaInterna;
  unsigned long tiempoEtapaInicio;
  int tiempoEsperaActual;
  bool esperandoEtapa;

public:
  ModoBarreraLazo()
    : tiempoAbrir(), tiempoCerrar(), tiempoLazo(95), etapaInterna(-1),
      pausaActivaInterna(false), tiempoEtapaInicio(0), tiempoEsperaActual(0),
      esperandoEtapa(false) {}

  void execute() override {

    actualizarEstadoPausa();
    if (pausaActiva) return;
    digitalWrite(ledPause, HIGH);
    digitalWrite(ledV, LOW);

    // Si estamos esperando, verificar si ha pasado el tiempo
    if (esperandoEtapa) {
      unsigned long tiempoTranscurrido = millis() - tiempoEtapaInicio;
      // Determinar si esperar en ms o segundos
      unsigned long limiteMs = tiempoEsperaActual;
      if (tiempoEsperaActual > 5000) {
        // Es un valor que se debe interpretar como segundos
        limiteMs = tiempoEsperaActual * 1000;
      }

      if (tiempoTranscurrido >= limiteMs) {
        esperandoEtapa = false;
      } else {
        return;
      }
    }

    switch (etapaInterna) {
      case -1:
        etapaInterna = 0;
        return;

      case 0:  // R1 ON (LOW), R2 OFF (HIGH)
        RelaysCl();
        RelaysAct();
        Serial.println("ETAPA 0: R1 ON (activado), R2 OFF (desactivado)");
        iniciarEspera(tiempoAbrir * 1000, 1);  // en ms
        break;

      case 1:  // R1 ON, R2 ON (tiempoLazo)
        RelaysCl();
        RelaysDes();
        Serial.println("ETAPA 1: R2 activado por tiempoLazo");
        iniciarEspera(tiempoLazo, 2);  // en ms
        break;

      case 2:  // R1 ON, R2 OFF
        RelaysCl();
        RelaysAct();
        guardarConteoLocal();
        Serial.println("ETAPA 2: R2 apagado, R1 sigue activado");
        Serial.print("Cuentas actuales: ");
        Serial.println(counter);
        iniciarEspera(1000, 3);  // 1 segundo en ms
        break;

      case 3:  // R1 OFF, R2 OFF - FIN DEL CICLO
        RelaysOp();
        RelaysAct();
        Serial.println("ETAPA 3: R1 y R2 desactivados - Fin de ciclo");
        iniciarEspera(tiempoCerrar * 1000, 0);  // en ms
        break;
    }
  }

  void setTiempoAbrir(int tiempo) override {
    tiempoAbrir = tiempo;
  }

  void setTiempoCerrar(int tiempo) override {
    tiempoCerrar = tiempo;
  }

  void setTiempoLazo(int tiempo) override {
    tiempoLazo = tiempo;
  }

  int getTiempoAbrir() override {
    return tiempoAbrir;
  }

  int getTiempoCerrar() override {
    return tiempoCerrar;
  }

  int getTiempoLazo() override {
    return tiempoLazo;
  }

  String getModeName() override {
    return "Barrier with Loop";
  }

private:
  void iniciarEspera(int tiempoMs, int sigEtapa) {
    tiempoEtapaInicio = millis();
    tiempoEsperaActual = tiempoMs;
    etapaInterna = sigEtapa;
    esperandoEtapa = true;
    Serial.print("Esperando ");
    Serial.print(tiempoMs);
    Serial.println(" ms...");
  }
};