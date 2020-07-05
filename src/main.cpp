#include <Arduino.h>
#include "Tasks.h"

void dns_setup();
void nats_setup();
void muthur_loop();
void nats_loop();
void network_init();
bool led_setup();
void led_loop();
void artnet_setup();
void artnet_loop();

void ledCallback();
void wifiCallback();
void networkCallback();

Scheduler ts;

Task tLed(TASK_SECOND / 90, TASK_FOREVER, &led_loop, &ts);
extern Task tNetwork;
extern Task tConnect;

void setup()
{
  // Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY);
  Serial.begin(115200);
  while (!Serial)
  {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println();
  Serial.println(F("[MAIN] starting pilar"));

  ts.addTask(tConnect);
  ts.addTask(tNetwork);

  led_setup();
  network_init();
  tLed.enable();
}

void loop()
{
  ts.execute();
}
