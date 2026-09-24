#pragma once
#include "ProcessInterface.hpp"
#include "RelayController.hpp"
#include "StorageService.hpp"

class Eventos : public ProcessInterface {
private:
    int etapaInterna;      // Etapa de ejecución interna
    int eventoSeleccionado; // 0 = L0, 1 = L1, 2 = L2
    int repeticiones;       // Para contar ON/OFF de R2 en L1

public:
    Eventos() : etapaInterna(0), eventoSeleccionado(0), repeticiones(0) {}

    void setEvento(int ev) {
        eventoSeleccionado = ev;
        etapaInterna = 0; // Reinicia ejecución
        repeticiones = 0;
    }

    int getEvento() { return eventoSeleccionado; }

    void execute() override {
        if (pausaActiva) return;

        switch(eventoSeleccionado) {
            case 0: ejecutarL0(); break;
            case 1: ejecutarL1(); break;
            case 2: ejecutarL2(); break;
        }
    }

    // ==================== EVENTOS =====================
    void ejecutarL0() {
        switch (etapaInterna) {
            case 0:
                //digitalWrite(Relay1, LOW);
                RelaysCl();
                Serial.println("[L0] R1 ON, R2 OFF");
                delay(4000); 
                etapaInterna = 1;
                break;
            case 1:
                digitalWrite(Relay1, HIGH);
                RelaysOp();
                delay(4000); 
                Serial.println("[L0] Reinicio");
                etapaInterna = 0;
                break;
        }
    }
void ejecutarL1() {
    switch (etapaInterna) {
        case 0:
            RelaysCl();
            RelaysAct();
            Serial.println("[L1] R1 ON fijo, R2 OFF");
            delay(1000);  // Pequeña pausa para estabilizar
            repeticiones = 0;  // Reiniciar contador
            etapaInterna = 1;
            break;
        case 1:
            if (repeticiones < 4) {
                // Alternar R2 mientras R1 permanece ON
                RelaysDes();
                Serial.println("[L1] R2 ON");
                delay(1000);
                RelaysAct();
                guardarConteoLocal();
                Serial.println("[L1] R2 OFF");
                delay(1000);
                repeticiones++;
            } else {
                etapaInterna = 2;
            }
            break;
        case 2:
            RelaysOp();
            RelaysAct();
            Serial.println("[L1] Reinicio - Ambos relays OFF");
            delay(1000);
            etapaInterna = 0;
            break;
    }
}
    void ejecutarL2() {
        switch (etapaInterna) {
            case 0:
                RelaysOp();
                RelaysDes();
                Serial.println("[L2] R2 ON 1s, R1 OFF");
                delay(2000);
                etapaInterna = 1;
                break;
            case 1:
                RelaysAct();
                delay(1000); 
                Serial.println("[L2] Reinicio");
                etapaInterna = 0;
                break;
        }
    }

    // ==================== INTERFAZ =====================
    void setTiempoAbrir(int) override {}
    void setTiempoCerrar(int) override {}
    void setTiempoLazo(int) override {}

    int getTiempoAbrir() override { return 0; }
    int getTiempoCerrar() override { return 0; }
    int getTiempoLazo() override { return 0; }

    String getModeName() override { return "Eventos L0-L2"; }
};
