#pragma once
#include "ProcessInterface.hpp"
#include "PauseController.hpp"
#include "RelayController.hpp"
#include "StorageService.hpp"

class ModoBarreraSimple : public ProcessInterface {
private:
  int tiempoAbrir;
  int tiempoCerrar;
  int tiempoLazo;
  int etapaInterna;
  bool pausaActivaInterna;
  unsigned long tiempoEtapaInicio;  // Tiempo cuando inicia la etapa
  int tiempoEsperaActual;           // Tiempo que se debe esperar en esta etapa
  bool esperandoEtapa;              // Si estamos esperando

public:
  ModoBarreraSimple()
    : tiempoAbrir(), tiempoCerrar(), tiempoLazo(1), etapaInterna(0),
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
      if (tiempoTranscurrido >= (tiempoEsperaActual * 1000)) {
        esperandoEtapa = false;
        // La siguiente etapa se ejecutará en la próxima llamada a execute()
      } else {
        // Todavía estamos esperando, no hacer nada
        return;
      }
    }

    // Máquina de estados
    switch (etapaInterna) {
      case 0:
        Serial.println("MODO SIMPLE - ETAPA 0: Cerrando...");
        RelaysOp();
        iniciarEspera(tiempoAbrir, 1);
        break;

      case 1:
        Serial.println("MODO SIMPLE - ETAPA 1: Abriendo...");
        RelaysCl();
        iniciarEspera(tiempoCerrar, 2);
        break;

      case 2:
        Serial.println("MODO SIMPLE - ETAPA 2: Subiendo datos...");
        guardarConteoLocal();
        Serial.print("Cuentas actuales: ");
        Serial.println(counter);
        etapaInterna = 0;
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
    return "Simple Barrier";
  }

private:
  // Inicia una espera sin bloqueo
  void iniciarEspera(int segundos, int sigEtapa) {
    tiempoEtapaInicio = millis();
    tiempoEsperaActual = segundos;
    etapaInterna = sigEtapa;
    esperandoEtapa = true;
    Serial.print("Esperando ");
    Serial.print(segundos);
    Serial.println(" segundos...");
  }
};
