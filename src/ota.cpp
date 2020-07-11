#include <ArduinoOTA.h>

#include "config.h"
#include "network.h"
#include "task.h"

bool ota_start();
void ota_end();
void ota_loop();

Task tOta(TASK_IMMEDIATE, TASK_FOREVER, &ota_loop, nullptr, false, &ota_start, &ota_end);

void ota_setup() {
#if ENABLE_OTA
  ArduinoOTA.setHostname(network_hostname().c_str());
  ArduinoOTA.onProgress([](unsigned int total, unsigned int size) {
    if (total / size % 5 == 0) {
      Serial.printf("[OTA] receiving firmware: %d%%\n", int(float(total) / float(size) * 100));
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
    static const char* ota_errors[] = {
        "OTA_AUTH_ERROR",
        "OTA_BEGIN_ERROR",
        "OTA_CONNECT_ERROR",
        "OTA_RECEIVE_ERROR",
        "OTA_END_ERROR"};

    Serial.printf("[OTA] error: %s", ota_errors[error]);
    ESP.restart();
  });
  ArduinoOTA.begin();
  network_addtask(tOta);
  ts.addTask(tOta);
#endif
}

bool ota_start() {
  Serial.println(F("[OTA] enabled"));
  return true;
}

void ota_end() { Serial.println(F("[OTA] disabled")); }

void ota_loop() { ArduinoOTA.handle(); }
