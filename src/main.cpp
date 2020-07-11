#include <Arduino.h>

#include "task.h"

void dns_setup();
void nats_setup();
void muthur_setup();
void network_setup();
bool led_setup();
void artnet_setup();
void ota_setup();
void sacn_setup();

Scheduler ts;

void setup() {
  // Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY);
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    yield();
  }
  Serial.println();
  Serial.println(F("[MAIN] starting pilar"));

  network_setup();
  artnet_setup();
  ota_setup();
  sacn_setup();
  led_setup();
  dns_setup();
}

void loop() { ts.execute(); }
