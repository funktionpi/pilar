#include <ESPAsyncE131.h>
#include "task.h"
#include "network.h"

#define UNIVERSE 1       // First DMX Universe to listen for
#define UNIVERSE_COUNT 2 // Total number of Universes to listen for, starting at UNIVERSE

ESPAsyncE131 e131(UNIVERSE_COUNT);

void sacn_loop();

Task tsACN(TASK_IMMEDIATE, TASK_FOREVER, &sacn_loop);

void sacn_setup()
{
  e131.begin(E131_UNICAST, UNIVERSE, UNIVERSE_COUNT);
  ts.addTask(tsACN);
  tsACN.enableDelayed(TASK_MILLISECOND * 250);
}

void sacn_loop()
{
  if (network_connected())
  {
    if (!e131.isEmpty())
    {
      e131_packet_t packet;
      e131.pull(&packet); // Pull packet from ring buffer

#if DEBUG_SACN
      Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
                    htons(packet.universe),                 // The Universe for this packet
                    htons(packet.property_value_count) - 1, // Start code is ignored, we're interested in dimmer data
                    e131.stats.num_packets,                 // Packet counter
                    e131.stats.packet_errors,               // Packet error counter
                    packet.property_values[1]);             // Dimmer data for Channel 1
#endif
    }
  }
}
