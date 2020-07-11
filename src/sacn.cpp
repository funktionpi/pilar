#include <ESPAsyncE131.h>

#include "config.h"
#include "network.h"
#include "task.h"

ESPAsyncE131 e131(UNIVERSE_COUNT);

// E1.17 ACN Packet Identifier
const byte ESPAsyncE131::ACN_ID[12] = {0x41, 0x53, 0x43, 0x2d, 0x45, 0x31,
                                       0x2e, 0x31, 0x37, 0x00, 0x00, 0x00};

// Art-Net Packet Identifier
// const byte ESPAsyncE131::ART_ID[8]  = { 0x41, 0x72, 0x74, 0x2d, 0x4e, 0x65,
// 0x74, 0x00 };

bool sacn_start();
void sacn_end();
void sacn_loop();

Task tsACN(TASK_IMMEDIATE, TASK_FOREVER, &sacn_loop, nullptr, false,
           &sacn_start, &sacn_end);

void sacn_setup() {
#if ENABLE_SACN
  network_addtask(tsACN);
  ts.addTask(tsACN);
#endif
}

bool sacn_start() {
  Serial.println(F("[SACN] enabled"));
  e131.begin(E131_UNICAST, UNIVERSE, UNIVERSE_COUNT);
  return true;
}

void sacn_end() { Serial.println(F("[SACN] disabled")); }

void sacn_loop() {
  if (!e131.isEmpty()) {
    e131_packet_t packet;
    e131.pull(&packet);  // Pull packet from ring buffer

#if DEBUG_SACN
    Serial.printf(
        "Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
        htons(packet.universe),  // The Universe for this packet
        htons(packet.property_value_count) -
            1,                       // Start code is ignored, we're interested in dimmer data
        e131.stats.num_packets,      // Packet counter
        e131.stats.packet_errors,    // Packet error counter
        packet.property_values[1]);  // Dimmer data for Channel 1
#endif
  }
}
