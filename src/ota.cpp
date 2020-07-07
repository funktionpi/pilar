#include <ArduinoOTA.h>
#include "task.h"
#include "network.h"
#include "config.h"

void ota_loop();

Task tOta (TASK_IMMEDIATE, TASK_FOREVER, &ota_loop);

const char* ota_errors[] =
{
  "OTA_AUTH_ERROR",
  "OTA_BEGIN_ERROR",
  "OTA_CONNECT_ERROR",
  "OTA_RECEIVE_ERROR",
  "OTA_END_ERROR"
};

void ota_setup()
{
  ArduinoOTA.setHostname(network_hostname().c_str());

  ArduinoOTA.onStart([]() {
    Serial.println(F("[OTA] enabled"));
  });

  ArduinoOTA.onEnd([]() {
    Serial.println(F("[OTA] disabled"));
  });

  ArduinoOTA.onProgress([](unsigned int total, unsigned int size) {
    Serial.printf("[OTA] updating firmware , %d / %d\n", total, size);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] error: %s", ota_errors[error]);
    ESP.restart();
  });

  ArduinoOTA.begin();

  ts.addTask(tOta);
  tOta.enableDelayed(TASK_MILLISECOND * 250);
}

void ota_loop()
{
  ArduinoOTA.handle();
}
