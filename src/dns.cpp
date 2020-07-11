#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "network.h"
#include "task.h"

/*
    bool addServiceTxt(const String& p_strService,
                       const String& p_strProtocol,
                       const String& p_strKey,
                       const String& p_strValue);
*/

void dns_setup() {
  auto svc = MDNS.addService(0, "_artnet", "udp", 6454);
  MDNS.addServiceTxt(svc, "universes", "1,2,3");
  MDNS.begin(network_hostname().c_str());
  Serial.println(F("[MDNS] service broadcasted"));
}

void dns_loop() {}
