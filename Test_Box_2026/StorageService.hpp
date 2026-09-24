#pragma once

#include <Preferences.h>
#include "config.h"

extern Preferences prefs;

void storageInit() {
  prefs.begin("conteos", false);
  counter = prefs.getInt("contador", 0);
  Serial.print("Conteo recuperado: ");
  Serial.println(counter);
}

void guardarConteoLocal() {
  counter++;
  prefs.putInt("contador", counter);
  Serial.print("Nuevo conteo guardado: ");
  Serial.println(counter);
  etapa = 0;
}

void cargarDatos() {
  counter = prefs.getInt("contador", 0);
  Serial.print("Contador cargado desde memoria: ");
  Serial.println(counter);
}
