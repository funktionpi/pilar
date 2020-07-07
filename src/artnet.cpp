#include <ArtnetnodeWifi.h>
#include "config.h"
#include "task.h"
#include "network.h"

void artnet_process(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data);

void artnet_loop();

Task tArtnet(TASK_IMMEDIATE, TASK_FOREVER, &artnet_loop);
ArtnetnodeWifi artnetnode;

void artnet_setup()
{
  Serial.println(F("[ARTNET] setting up ART-NET controller"));

  artnetnode.setName(network_hostname().c_str());
  artnetnode.setLongName("PILar ArtNet node");
  artnetnode.setNumPorts(3);
  artnetnode.setStartingUniverse(1);
  artnetnode.setUniverse(1);
  artnetnode.disableDMXOutput(0);
  artnetnode.setArtDmxCallback(artnet_process);
  artnetnode.begin(network_hostname().c_str());

  tArtnet.setOnEnable([] () -> bool {
    Serial.println(F("[ARTNET] task enabled"));
    return true;
  });
  tArtnet.enableDelayed(TASK_MILLISECOND * 250);
  ts.addTask(tArtnet);
}

void artnet_process(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
#if DEBUG_ARTNET
  Serial.printf("[ARTNET] received packet - universe %d, lenght: %d, sequence: %d\n", universe, length, sequence);
#endif
}

void artnet_loop()
{
  if (network_connected())
  {
    artnetnode.read();
  }
}
