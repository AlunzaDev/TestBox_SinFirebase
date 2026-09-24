// ========================== DEFINICIÓN DE PINES ============================
#pragma once
#define Relay1 19     //K1 relé
#define Relay2 21     //K2 relé
#define Relay3 22     //K3 relé
#define Relay4 23     //K4 relé
#define ledPause 4    //Led rojo(pausa)
#define ledV   15     //Led verde(ejecutandose)
#define Pause  5      //Boton para pausar el proceso
#define WifiConn 2    //Led para saber si esta conectado al wifi
#define ledOn 13      //Led para equipo encendido.

// ========================== CREDENCIALES DEL AP ============================

String apSsid = "TestBox";
String apPassword = "TestBox2026";

// ========================== CONFIGURACIÓN DEL TIMER  ======================

volatile int interruptCounter;
int seg = 0;
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// ========================== INTERRUPCIÓN TIMER ============================

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;  //Activa bandera para indicar que la interrupcion sucedio
  portEXIT_CRITICAL_ISR(&timerMux);
}

// ========================== INICIALIZACIÓN DEL TIMER ======================

void initTimer() {
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 1000000, true, 0); 
}

// ========================== VARIABLES DE CONTROL ==========================

bool wifiConnectedPreviously = false;
volatile bool flagtostart = false;
bool ultimoEstadoPausa = true;
bool pausaActiva = false;
unsigned long timestamp;
int tiempoCerrar = 3;
int tiempoAbrir = 3;
String httpAnswer;
String jsonInfo;
int fechaArr[3];
int horaArr[3];
int epochs = 0;
int etapa = 0;
String fecha;
String hora;
int counter;
int modo;
unsigned long testStartMs = 0;
unsigned long testDurationMs = 0;
