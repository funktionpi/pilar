#include <Arduino.h>
#include <ArduinoNATS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "task.h"

WiFiClient natsWifi;
NATS* nats;
char natsHostname[64];

void muthur_setup() {
  Serial.println(F("[MDNS] Looking for MUTHUR ... "));

  auto u32AnswerCount = MDNS.queryService("_nats", "_tcp");

  for (uint32_t u = 0; u < u32AnswerCount; ++u) {
    const char* pHostname = MDNS.answerHostname(u);
    Serial.print(F("[MDNS] MUTHUR found! Host: "));
    Serial.print(pHostname);
    auto ip = MDNS.answerIP(u);
    auto port = MDNS.answerPort(u);
    Serial.print(", Ip: ");
    ip.printTo(Serial);
    Serial.print(", Port: ");
    Serial.print(port);
    Serial.println();

    if (u == 0) {
      // if (!WiFi.hostByName("protopi", ip))
      // {
      //   Serial.println("[MDNS] could not resolve hostname");
      // }

      auto ipstr = ip.toString();
      Serial.print(F("[NATS] connecting to host: "));
      Serial.println(ipstr);
      strcpy(natsHostname, ipstr.c_str());
      nats = new NATS(&natsWifi, natsHostname, port);
      nats->on_connect = []() {
        Serial.println(F("[NATS] connected to MUTHUR."));
      };
      nats->on_disconnect = []() {
        Serial.println(F("[NATS] disconnected from MUTHUR."));
      };
      nats->on_error = []() { Serial.println(F("[NATS] error occured.")); };
    }
  }

  Serial.println(F("[MDNS] discovery done."));
}

void nats_setup() {
  if (nats) {
    nats->connect();
  }

  // if (nats)
  // {
  //   Serial.println("[NATS] connecting to MUTHUR... ");
  //   while (!nats->connect())
  //   {
  //     Serial.println("[NATS] failed to connect to MUTHUR");
  //     Serial.print("[NATS] wifi status: ");
  //     Serial.print(natsWifi.status());
  //     Serial.println();

  //     delay(2500);
  //   }
  // }
}

void nats_loop() {
  if (nats) {
    nats->process();
  }
}

void muthur_loop() { MDNS.update(); }
