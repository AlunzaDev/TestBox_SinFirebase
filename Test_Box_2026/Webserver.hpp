#pragma once
#include <WebServer.h>
#include "ProcessInterface.hpp"
#include "ModoBarreraSimple.hpp"
#include "ModoBarreraLazo.hpp"
#include "Eventos.hpp"
#include <Preferences.h>
extern Preferences prefs;


// Variables globales
extern int tiempoAbrir;
extern int tiempoCerrar;
extern Eventos eventos;
extern int counter;
extern volatile bool flagtostart;
extern int modo;  // 0 = Simple, 1 = Lazo, 2 = Eventos
extern unsigned long testStartMs;
extern unsigned long testDurationMs;
extern ProcessInterface* modoActual;
extern ModoBarreraSimple modoSimple;
extern ModoBarreraLazo modoLazo;

const char* www_username = "admin";
const char* www_password = "1234";

class WebServerHandler {
public:
  WebServerHandler() {}

  void begin() {
    server.on("/", HTTP_GET, std::bind(&WebServerHandler::handleRoot, this));

    server.on("/start", HTTP_GET, [this]() {
      unsigned long minutos = 20;
      if (server.hasArg("duracion")) {
        minutos = server.arg("duracion").toInt();
        if (minutos < 1) {
          minutos = 20;
        }
      }

      testDurationMs = minutos * 60000UL;
      testStartMs = millis();
      flagtostart = true;
      addLogEvent("[START] Duración " + String(minutos) + " minutos");
      server.send(200, "text/plain", "Sistema iniciado manualmente");
    });

    server.on("/setTimes", HTTP_POST, [this]() {
      handleSetTimes();
    });

    server.on("/get-counter", HTTP_GET, [this]() {
      server.send(200, "text/plain", String(counter));
    });

    server.on("/reset-counter", HTTP_POST, [this]() {
      if (!server.authenticate(www_username, www_password)) {
        return server.requestAuthentication();
      }

      counter = 0;

      // Guarda el valor reiniciado en memoria interna
      prefs.putInt("contador", counter);

      Serial.println("Contador reiniciado en memoria local");
      server.send(200, "text/plain", "Conteo reiniciado");
    });


    server.on("/event-log", HTTP_GET, [this]() {
      String response;
      for (const auto& entry : eventLog) {
        response += entry + "\n";
      }
      server.send(200, "text/plain", response);
    });

    server.on("/set-mode", HTTP_POST, [this]() {
      String modoStr;
      if (server.hasArg("modo")) {
        int nuevoModo = server.arg("modo").toInt();

        if (nuevoModo == 0) {
          modoActual = &modoSimple;
          modoStr = "Barrera Simple";
        } else if (nuevoModo == 1) {
          modoActual = &modoLazo;
          modoStr = "Barrera con Lazo";
        } else if (nuevoModo == 2) {
          modoActual = &eventos;
          modoStr = "Eventos (L0-L2)";
        }
        modo = nuevoModo;

        Serial.print("Modo cambiado a: ");
        Serial.println(nuevoModo);
        addLogEvent("[CAMBIO MODO] " + modoStr);
      }
      server.send(200, "text/plain", "OK");
    });

    // Endpoint para cambiar el evento dentro de "Eventos" (fetch)
    server.on("/set-evento", HTTP_POST, [this]() {
      if (server.hasArg("evento")) {
        int nuevoEvento = server.arg("evento").toInt();
        eventos.setEvento(nuevoEvento);
        Serial.print("Evento cambiado a: ");
        Serial.println(nuevoEvento);
      }
      server.send(200, "text/plain", "OK");
    });

    server.on("/update", HTTP_POST, [this]() {
      if (!updateAuthorized) {
        server.requestAuthentication();
        return;
      }

      if (Update.hasError()) {
        server.send(500, "text/plain", "Error al actualizar el firmware");
        updateAuthorized = false;
        return;
      }

      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", "Firmware actualizado. El ESP se reiniciara.");
      updateAuthorized = false;
      delay(500);
      ESP.restart();
    }, [this]() {
      HTTPUpload& upload = server.upload();

      if (upload.status == UPLOAD_FILE_START) {
        updateAuthorized = server.authenticate(www_username, www_password);
        if (!updateAuthorized || flagtostart) {
          updateAuthorized = false;
          return;
        }

        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
          Update.printError(Serial);
        }
      } else if (updateAuthorized && upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (updateAuthorized && upload.status == UPLOAD_FILE_END) {
        if (!Update.end(true)) {
          Update.printError(Serial);
        }
      }
    });

    server.begin();
  }

  void handleClient() {
    server.handleClient();
  }

private:
  WebServer server;
  bool updateAuthorized = false;
  void handleRoot() {
    String html = "<!DOCTYPE html><html lang='es'><head>\
  <meta charset='UTF-8'>\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
  <title>Control de Barrera</title>\
  <style>\
    :root {\
      --bg: #141f42;\
      --bg-secondary: #1e2e5c;\
      --card-bg: rgba(255,255,255,0.05);\
      --border: #34495e;\
      --text: #ffffff;\
      --muted: #a0aec0;\
      --accent: #66d43d;\
      --accent-hover: #57bd32;\
      --start: #4facfe;\
      --start-hover: #3b97ea;\
    }\
    * {\
      box-sizing: border-box;\
      margin: 0;\
      padding: 0;\
    }\
    body {\
      min-height: 100vh;\
      padding: 16px;\
      background: var(--bg);\
      color: var(--text);\
      font-family: Arial, sans-serif;\
      line-height: 1.4;\
    }\
    .container {\
      max-width: 1200px;\
      margin: 0 auto;\
      padding: 20px;\
      background: rgba(0,0,0,0.25);\
      border: 1px solid var(--border);\
      border-radius: 12px;\
    }\
    header {\
      display: flex;\
      justify-content: space-between;\
      align-items: flex-start;\
      gap: 16px;\
      margin-bottom: 20px;\
      padding-bottom: 14px;\
      border-bottom: 1px solid var(--border);\
    }\
    .header-copy {\
      flex: 1;\
    }\
    h1 {\
      font-size: 1.9rem;\
      margin-bottom: 8px;\
    }\
    .status {\
      color: var(--muted);\
      font-size: 0.92rem;\
    }\
    .status-panel {\
      min-width: 240px;\
      padding: 12px 14px;\
      background: var(--card-bg);\
      border: 1px solid var(--border);\
      border-radius: 8px;\
    }\
    .status-title {\
      color: #c2d1e9;\
      font-size: 0.82rem;\
      font-weight: 700;\
      margin-bottom: 6px;\
    }\
    .status-row {\
      display: flex;\
      align-items: center;\
      gap: 8px;\
      margin-bottom: 6px;\
    }\
    .status-dot {\
      width: 10px;\
      height: 10px;\
      border-radius: 50%;\
      background: var(--accent);\
    }\
    .status-text {\
      font-size: 0.95rem;\
      font-weight: 700;\
    }\
    .status-note {\
      color: var(--muted);\
      font-size: 0.88rem;\
    }\
    .dashboard {\
      display: grid;\
      grid-template-columns: repeat(4, minmax(0, 1fr));\
      gap: 16px;\
    }\
    .card {\
      background: var(--card-bg);\
      border: 1px solid var(--border);\
      border-radius: 10px;\
      padding: 16px;\
    }\
    .full-width {\
      grid-column: 1 / -1;\
    }\
    h2 {\
      font-size: 1.05rem;\
      margin-bottom: 12px;\
      color: #c2d1e9;\
    }\
    .helper {\
      color: var(--muted);\
      font-size: 0.9rem;\
      margin-bottom: 14px;\
    }\
    .field {\
      margin-bottom: 14px;\
    }\
    label {\
      display: block;\
      margin-bottom: 6px;\
      font-size: 0.92rem;\
    }\
    select, input[type='number'] {\
      width: 100%;\
      padding: 10px 12px;\
      border: 1px solid var(--border);\
      border-radius: 6px;\
      background: #34495e;\
      color: #ffffff;\
      font: inherit;\
    }\
    button, input[type='submit'] {\
      width: 100%;\
      padding: 10px 12px;\
      border: none;\
      border-radius: 6px;\
      color: #ffffff;\
      background: var(--accent);\
      font: inherit;\
      font-weight: 700;\
      cursor: pointer;\
    }\
    button:hover, input[type='submit']:hover {\
      background: var(--accent-hover);\
    }\
    .boton-start {\
      background: var(--start);\
    }\
    .boton-start:hover {\
      background: var(--start-hover);\
    }\
    .counter-display {\
      padding: 24px 16px;\
      margin-bottom: 14px;\
      border: 1px solid var(--accent);\
      border-radius: 8px;\
      background: rgba(0,0,0,0.2);\
      text-align: center;\
    }\
    .counter-label {\
      display: block;\
      color: var(--muted);\
      font-size: 0.82rem;\
      margin-bottom: 8px;\
    }\
    #contador {\
      font-size: 3rem;\
      font-weight: 700;\
    }\
    .log-container {\
      min-height: 220px;\
      padding: 12px;\
      border: 1px solid var(--border);\
      border-radius: 8px;\
      background: rgba(0,0,0,0.2);\
      overflow-y: auto;\
    }\
    #logContent {\
      margin: 0;\
      white-space: pre-wrap;\
      font-family: Consolas, monospace;\
      font-size: 0.85rem;\
    }\
    .footer {\
      margin-top: 18px;\
      padding-top: 14px;\
      border-top: 1px solid var(--border);\
      color: var(--muted);\
      font-size: 0.85rem;\
      text-align: center;\
    }\
    @media (max-width: 1100px) {\
      .dashboard {\
        grid-template-columns: repeat(2, minmax(0, 1fr));\
      }\
    }\
    @media (max-width: 640px) {\
      body {\
        padding: 10px;\
      }\
      header {\
        flex-direction: column;\
      }\
      .status-panel {\
        width: 100%;\
      }\
      .container {\
        padding: 14px;\
      }\
      .dashboard {\
        grid-template-columns: 1fr;\
      }\
      h1 {\
        font-size: 1.5rem;\
      }\
    }\
  </style>\
</head>\
<body>\
  <div class='container'>\
    <header>\
      <div class='header-copy'>\
        <h1>Control de Test Sikkounter</h1>\
      </div>\
    </header>\
    \
    <main class='dashboard'>\
      <section class='card'>\
        <h2>Modo de operación</h2>\
        <p class='helper'>Selecciona el modo y el evento si aplica.</p>\
        \
        <div class='field'>\
          <label for='modoSelect'>Selecciona el modo</label>\
          <select id='modoSelect'>\
            <option value='0'"
                  + String((modo == 0) ? " selected" : "") + ">Barrera Simple</option>\
            <option value='1'"
                  + String((modo == 1) ? " selected" : "") + ">Barrera con Lazo</option>\
            <option value='2'"
                  + String((modo == 2) ? " selected" : "") + ">Eventos (L0-L2)</option>\
          </select>\
        </div>\
        \
        <button type='button' onclick='cambiarModo()'>Cambiar modo</button>";

    if (modo == 2) {
      html += "<div style='height:10px'></div>\
        <div class='field'>\
          <label for='eventoSelect'>Evento</label>\
          <select id='eventoSelect'>\
            <option value='0'"
              + String((eventos.getEvento() == 0) ? " selected" : "") + ">L0</option>\
            <option value='1'"
              + String((eventos.getEvento() == 1) ? " selected" : "") + ">L1</option>\
            <option value='2'"
              + String((eventos.getEvento() == 2) ? " selected" : "") + ">L2</option>\
          </select>\
        </div>\
        <button type='button' onclick='cambiarEvento()'>Cambiar evento</button>";
    }

    html += "</section>\
      \
      <section class='card'>\
        <h2>Control de prueba</h2>\
        <p class='helper'>Configura la duración y arranca el test.</p>\
        \
        <div class='field'>\
          <label for='duracion'>Duración del test (minutos)</label>\
          <input type='number' id='duracion' min='1' max='1440' value='20'>\
        </div>\
        \
        <button class='boton-start' type='button' onclick='iniciarTest()'>Iniciar test</button>\
      </section>\
      \
      <section class='card'>\
        <h2>Configuración de tiempos</h2>\
        <p class='helper'>Ajusta apertura y cierre.</p>\
        \
        <div class='field'>\
          <label for='abrir'>Tiempo para abrir (s)</label>\
          <input type='number' id='abrir' value='"
            + String(tiempoAbrir) + "' min='1'>\
        </div>\
        \
        <div class='field'>\
          <label for='cerrar'>Tiempo para cerrar (s)</label>\
          <input type='number' id='cerrar' value='"
            + String(tiempoCerrar) + "' min='1'>\
        </div>\
        \
        <button type='button' onclick='cambiarTiempos()'>Actualizar tiempos</button>\
      </section>\
      \
      <section class='card'>\
        <h2>Contador de eventos</h2>\
        <p class='helper'>Conteo automático del sistema.</p>\
        \
        <div class='counter-display'>\
          <span class='counter-label'>Eventos detectados</span>\
          <span id='contador'>0</span>\
        </div>\
        \
        <form method='POST' action='/reset-counter' onsubmit=\"return confirm('¿Estás seguro de que deseas reiniciar el contador?');\">\
          <input type='submit' value='Reiniciar contador'>\
        </form>\
      </section>\
      \
      <section class='card full-width'>\
        <h2>Logs del sistema</h2>\
        <p class='helper'>Registro simple de acciones y eventos.</p>\
        \
        <div class='log-container'>\
          <pre id='logContent'></pre>\
        </div>\
      </section>\
      \
      <section class='card full-width'>\
        <h2>Actualizar firmware</h2>\
        <p class='helper'>Selecciona el archivo .bin. Deten la prueba antes de actualizar.</p>\
        <form method='POST' action='/update' enctype='multipart/form-data' onsubmit='return confirmarActualizacion();'>\
          <div class='field'>\
            <input type='file' name='firmware' accept='.bin' required>\
          </div>\
          <input type='submit' value='Actualizar ESP'>\
        </form>\
      </section>\
    </main>\
    \
    <div class='footer'>\
      Sistema de testing para contadores Sikker · ESP32\
    </div>\
  </div>\
  \
  <script>\
    function cambiarModo(){\
      const modo = document.getElementById('modoSelect').value;\
      fetch('/set-mode',{\
        method: 'POST',\
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},\
        body: 'modo=' + modo\
      })\
      .then(response => {\
        if(response.ok) {\
          location.reload();\
        }\
      });\
    }\
    \
    function cambiarEvento(){\
      const evento = document.getElementById('eventoSelect').value;\
      fetch('/set-evento',{\
        method: 'POST',\
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},\
        body: 'evento=' + evento\
      })\
      .then(response => {\
        if(response.ok) {\
          location.reload();\
        }\
      });\
    }\
    \
    function cambiarTiempos(){\
      const abrir = document.getElementById('abrir').value;\
      const cerrar = document.getElementById('cerrar').value;\
      if(abrir < 1 || cerrar < 1) {\
        alert('Los tiempos deben ser al menos 1 segundo');\
        return;\
      }\
      fetch('/setTimes',{\
        method: 'POST',\
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},\
        body: 'abrir=' + abrir + '&cerrar=' + cerrar\
      })\
      .then(response => {\
        if(response.ok) {\
          location.reload();\
        }\
      });\
    }\
    \
    function iniciarTest(){\
      const duracion = document.getElementById('duracion').value;\
      if (duracion < 1) {\
        alert('La duración debe ser al menos 1 minuto');\
        return;\
      }\
      fetch('/start?duracion=' + encodeURIComponent(duracion))\
        .then(response => {\
          if(response.ok) {\
            alert('Test iniciado por ' + duracion + ' minutos');\
          }\
        });\
    }\
    \
    function confirmarActualizacion(){\
      return confirm('El ESP se reiniciara al finalizar. ¿Continuar?');\
    }\
    \
    function actualizarConteo(){\
      fetch('/get-counter')\
        .then(r => r.text())\
        .then(d => {\
          document.getElementById('contador').innerText = d;\
        });\
    }\
    \
    function actualizarLogs(){\
      fetch('/event-log')\
        .then(r => r.text())\
        .then(d => {\
          document.getElementById('logContent').innerText = d;\
          const container = document.querySelector('.log-container');\
          container.scrollTop = container.scrollHeight;\
        });\
    }\
    \
    setInterval(actualizarConteo, 1000);\
    setInterval(actualizarLogs, 3000);\
    actualizarConteo();\
    actualizarLogs();\
  </script>\
</body></html>";

    server.send(200, "text/html", html);
  }

  void handleSetTimes() {
    if (server.hasArg("abrir") && server.hasArg("cerrar")) {
      tiempoAbrir = server.arg("abrir").toInt();
      tiempoCerrar = server.arg("cerrar").toInt();

      // Actualizar AMBOS modos
      modoSimple.setTiempoAbrir(tiempoAbrir);
      modoSimple.setTiempoCerrar(tiempoCerrar);

      modoLazo.setTiempoAbrir(tiempoAbrir);
      modoLazo.setTiempoCerrar(tiempoCerrar);

      Serial.println("Tiempos actualizados en ambos modos:");
      Serial.println("Abrir: " + String(tiempoAbrir));
      Serial.println("Cerrar: " + String(tiempoCerrar));
    }
    server.send(200, "text/plain", "OK");
  }
};
