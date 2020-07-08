#include <ArtnetnodeWifi.h>
#include "config.h"
#include "task.h"
#include "network.h"

void artnet_process(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data);

bool artnet_start();
void artnet_end();
void artnet_loop();

Task tArtnet(TASK_IMMEDIATE, TASK_FOREVER, &artnet_loop, nullptr, false, &artnet_start, &artnet_end);
ArtnetnodeWifi artnetnode;

void artnet_setup()
{
#if ENABLE_ARTNET
  tArtnet.setOnEnable([] () -> bool {
    Serial.println(F("[ARTNET] task enabled"));
    return true;
  });
  network_addtask(tArtnet);
  ts.addTask(tArtnet);
#endif
}

bool artnet_start()
{
  Serial.println(F("[ARTNET] enabled"));
  artnetnode.setName(network_hostname().c_str());
  artnetnode.setLongName("PILar ArtNet node");
  artnetnode.setNumPorts(3);
  artnetnode.setStartingUniverse(1);
  artnetnode.setUniverse(1);
  artnetnode.disableDMXOutput(0);
  artnetnode.setArtDmxCallback(artnet_process);
  artnetnode.begin(network_hostname().c_str());

  return true;
}

void artnet_end()
{
  Serial.println(F("[ARTNET] disabled"));
}

void artnet_loop()
{
  artnetnode.read();
}

void artnet_process(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
#if DEBUG_ARTNET
  Serial.printf("[ARTNET] received packet - universe %d, lenght: %d, sequence: %d\n", universe, length, sequence);
#endif
}
