#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <SPI.h>

#include <list>

#include "creds.h"
#include "task.h"

// int status = WL_IDLE_STATUS;
auto wifiIdx = 0;

#define CONNECT_TIMEOUT 11    // Seconds
#define CONNECT_OK 0          // Status of successful connection to WiFi
#define CONNECT_FAILED (-99)  // Status of failed connection to WiFi

void network_connect();
void network_loop();

Task tConnect(TASK_SECOND, TASK_FOREVER, &network_connect);
Task tNetwork(TASK_SECOND / 100, TASK_FOREVER, &network_loop);

// create a creds.cpp with the credentials baked into it
extern const Creds creds[];
extern const int CredsCount;

std::list<Task *> networked_tasks;

void print2Digits(byte thisByte) {
  if (thisByte < 0xF) {
    Serial.print("0");
  }
  Serial.print(thisByte, HEX);
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
  case ENC_TYPE_WEP:
    Serial.print(F("WEP"));
    break;
  case ENC_TYPE_TKIP:
    Serial.print(F("WPA"));
    break;
  case ENC_TYPE_CCMP:
    Serial.print(F("WPA2"));
    break;
  case ENC_TYPE_NONE:
    Serial.print(F("None"));
    break;
  case ENC_TYPE_AUTO:
    Serial.print(F("Auto"));
    break;
  default:
    Serial.print(F("Unknown"));
    break;
  }
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println(F("[WIFI] ** Scan Networks **"));
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println(F("[WIFI] ouldn't get a WiFi connection"));
    while (true)
      ;
  }

  // print the list of networks seen:
  Serial.print(F("[WIFI] number of available networks: "));
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet + 1);
    Serial.print(") ");
    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tChannel: ");
    Serial.print(WiFi.channel(thisNet));
    Serial.print("\t\tBSSID: ");
    printMacAddress(WiFi.BSSID(thisNet));
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
    Serial.print("\t\tSSID: ");
    Serial.println(WiFi.SSID(thisNet));
    Serial.flush();
  }
  Serial.println();
}

void printStatus(int status) {
  const char *names[] = {
      "WL_IDLE_STATUS",
      "WL_NO_SSID_AVAIL",
      "WL_SCAN_COMPLETED",
      "WL_CONNECTED",
      "WL_CONNECT_FAILED",
      "WL_CONNECTION_LOST",
      "WL_DISCONNECTED",
      "WL_AP_LISTENING",
      "WL_AP_CONNECTED",
      "WL_AP_FAILED"};

  Serial.println(names[status]);
}

String network_uid() {
  String id = "";
#if defined(ESP8266)
  id = String(ESP.getChipId());
#elif defined(ESP32)
  id = String((uint32_t)ESP.getEfuseMac(), HEX);
#endif
  id.toUpperCase();
  return id;
}

String network_hostname() {
  return "pilar-" + network_uid();
}

bool network_connected() {
  return WiFi.status() == WL_CONNECTED;
}

void network_addtask(Task &task) {
  networked_tasks.push_back(&task);
}

void network_setup() {
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("[WIFI] WiFi module not present"));
    // don't continue:
    while (true)
      ;
  }

  auto hostname = network_hostname();

  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname.c_str());
  WiFi.setAutoReconnect(true);
  WiFi.setAutoConnect(true);
  Serial.printf("[WIFI] hostname: %s\n", hostname.c_str());

  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  Serial.print("[WIFI] Wireless MAC: ");
  printMacAddress(mac);
  yield();

  tConnect.setOnEnable([]() -> bool {
    Serial.println(F("[WIFI] starting connection"));
    return true;
  });
  tConnect.setOnDisable([]() {
    Serial.println(F("[WIFI] connection successful"));
  });

  tNetwork.setOnEnable([]() -> bool {
    Serial.println(F("[WIFI] network tasks enabled"));
    for (auto &&task : networked_tasks) {
      task->enable();
    }
    return true;
  });
  tNetwork.setOnDisable([]() {
    Serial.println(F("[WIFI] network tasks disabled"));
    for (auto &&task : networked_tasks) {
      task->disable();
    }
  });

  ts.addTask(tConnect);
  ts.addTask(tNetwork);
  tConnect.enable();
}

void network_connect() {
  auto status = WiFi.status();
  Serial.print(F("[WIFI] Wifi status: "));
  printStatus(status);

  if (network_connected()) {
    Serial.print(F("[WIFI] Connected to "));
    Serial.println(creds[wifiIdx].ssid);
    Serial.print(F("[WIFI] IP address is "));
    Serial.println(WiFi.localIP());
    tConnect.disable();
    tNetwork.enable();
    return;
  }

  if (tConnect.getRunCounter() % 5 == 0) {
    WiFi.disconnect(true);
    yield();
    Serial.print(F("[WIFI] Attempting to connect to SSID: "));
    Serial.println(creds[wifiIdx].ssid);

    if (strlen(creds[wifiIdx].pwd)) {
      // Connect to WPA/WPA2 network.
      status = WiFi.begin(creds[wifiIdx].ssid, creds[wifiIdx].pwd);
    } else {
      status = WiFi.begin(creds[wifiIdx].ssid);
    }
    yield();
  }

  if (tConnect.getRunCounter() == CONNECT_TIMEOUT) {
    // tConnect.getInternalStatusRequest()->signal(CONNECT_FAILED); // Signal unsuccessful completion
    // tConnect.disable();
    Serial.print(F("[WIFI] Connection Timeout"));
    wifiIdx = (wifiIdx + 1) % CredsCount;
    listNetworks();
  }
}

void network_loop() {
  if (!network_connected()) {
    tNetwork.disable();
    tConnect.enable();
  }
}
