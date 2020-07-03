#include <ArtnetnodeWifi.h>

void artnet_process(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data);

ArtnetnodeWifi artnetnode;

void artnet_setup()
{
  Serial.println(F("[ARTNET] setting up ART-NET controller"));

  artnetnode.setName("PILAR - Artnet");
  artnetnode.setNumPorts(3);
  artnetnode.setStartingUniverse(1);
  artnetnode.disableDMXOutput(0);

  artnetnode.setArtDmxCallback(artnet_process);
  artnetnode.begin();
}

void artnet_process(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
  Serial.println(F("received ARTNET packet"));
}

void artnet_loop()
{
  artnetnode.read();
}
