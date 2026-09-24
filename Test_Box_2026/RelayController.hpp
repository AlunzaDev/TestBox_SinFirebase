#pragma once

#include "config.h"

void RelaysOp() {
  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay3, HIGH);
}

void RelaysCl() {
  digitalWrite(Relay1, LOW);
  digitalWrite(Relay3, LOW);
}

void RelaysAct() {
  digitalWrite(Relay2, HIGH);
  digitalWrite(Relay4, HIGH);
}

void RelaysDes() {
  digitalWrite(Relay2, LOW);
  digitalWrite(Relay4, LOW);
}

void RelaysInitialState() {
  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);
  digitalWrite(Relay3, HIGH);
  digitalWrite(Relay4, HIGH);
}
