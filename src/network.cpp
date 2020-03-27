#include <SPI.h>
#include <ESP8266WiFi.h>

#include "creds.h"
int status = WL_IDLE_STATUS;

// create a creds.cpp with the credentials baked into it
extern const Creds creds[];
extern const int CredsCount;

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
      Serial.print("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.print("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.print("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.print("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.print("Auto");
      break;
    default:
      Serial.print("Unknown");
      break;
  }
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("[WIFI] ** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("[WIFI] ouldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("[WIFI] number of available networks: ");
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
  const char* names[] = {
        "WL_IDLE_STATUS",
        "WL_NO_SSID_AVAIL",
        "WL_SCAN_COMPLETED",
        "WL_CONNECTED",
        "WL_CONNECT_FAILED",
        "WL_CONNECTION_LOST",
        "WL_DISCONNECTED",
        "WL_AP_LISTENING",
        "WL_AP_CONNECTED",
        "WL_AP_FAILED"
  };

  Serial.println(names[status]);
}

bool network_connected()
{
  return WiFi.status() == WL_CONNECTED;
}

void network_setup()
{
  Serial.println(F("[WIFI] Getting IP address..."));

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println(F("[WIFI] WiFi module not present"));
    // don't continue:
    while (true)
      ;
  }

  WiFi.mode(WIFI_STA);
  WiFi.hostname("pilar#1");
  WiFi.setAutoReconnect(true);
  WiFi.setAutoConnect(true);

  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  Serial.print("[WIFI] Wireless MAC: ");
  printMacAddress(mac);

  auto wifiIdx = 0;
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
    WiFi.disconnect();
    Serial.print(F("[WIFI] Attempting to connect to SSID: "));
    Serial.println(creds[wifiIdx].ssid);

    if (strlen(creds[wifiIdx].pwd))
    {
      // Connect to WPA/WPA2 network.
      status = WiFi.begin(creds[wifiIdx].ssid, creds[wifiIdx].pwd);
    }
    else
    {
      status = WiFi.begin(creds[wifiIdx].ssid);
    }

    status = WiFi.waitForConnectResult();

    Serial.print(F("[WIFI] Wifi status: "));
    printStatus(status);

    wifiIdx = (wifiIdx + 1) % CredsCount;

    if (!wifiIdx && status != WL_CONNECTED)
    {
      listNetworks();
    }
  }

  Serial.print(F("[WIFI] IP address is "));
  Serial.println(WiFi.localIP());
}

void network_loop()
{
}
