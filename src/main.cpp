#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CoopTask.h>

void dns_setup();
void nats_setup();
void muthur_loop();
void nats_loop();
void network_setup();
void network_loop();
void led_setup();
void led_loop();
bool network_connected();


int led_task();
int wifi_task();

CoopTask<> *ledTask;
CoopTask<> *wifiTask;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println();
  Serial.println(F("[MAIN] starting pilar"));

  ledTask = createCoopTask(F("led"), led_task);
  wifiTask = createCoopTask(F("wifi"), wifi_task);
}

void loop()
{
  runCoopTasks();
}

int wifi_task()
{
  for (;;)
  {
    if (!network_connected())
    {
      led_setup();
      network_setup();
      dns_setup();
      nats_setup();
    }
    else
    {
      network_loop();
      muthur_loop();
      nats_loop();
    }
  }
  return 0;
}

int led_task()
{
  for(;;)
  {
    led_loop();
    yield();
  }
  return 0;
}
