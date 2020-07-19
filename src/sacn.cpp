#include <ESPAsyncE131.h>

#include "config.h"
#include "display.h"
#include "led.h"
#include "network.h"
#include "task.h"

ESPAsyncE131 e131(UNIVERSE_COUNT);

bool sacn_start();
void sacn_end();
void sacn_loop();

Task tsACN(TASK_IMMEDIATE, TASK_FOREVER, &sacn_loop, nullptr, false, &sacn_start, &sacn_end);

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

    auto universe = htons(packet.universe);
    auto channelCount = htons(packet.property_value_count) - 1;
#if DEBUG_SACN
    if (tsACN.getIterations() % 5) {
      Serial.printf(
          "Universe %d / %d Channels / Type: %d | Packet#: %u / Errors: %u / CH%d: %u\n",
          universe,                  // The Universe for this packet
          channelCount,              // Start code is ignored, we're interested in dimmer data
          packet.type,               // packet type
          e131.stats.num_packets,    // Packet counter
          e131.stats.packet_errors,  // Packet error counter
          CHANNEL_BRIGHTNESS,
          packet.property_values[CHANNEL_BRIGHTNESS]);  // Dimmer data for Channel 1
    }
#endif

    if (universe == UNIVERSE_CTRL) {
      if (packet.property_values[CHANNEL_MIRROR] < 127) {
        Display.setMirror(Mirroring::Strip);
      } else {
        Display.setMirror(Mirroring::None);
      }

      if (channelCount >= CHANNEL_BRIGHTNESS) {
        {
          if (packet.property_values[CHANNEL_ORIGIN] < 127) {
            Display.setOrigin(Origin::Bottom);
          } else {
            Display.setOrigin(Origin::Top);
          }
        }

        if (channelCount >= CHANNEL_BRIGHTNESS) {
          auto bright = packet.property_values[CHANNEL_BRIGHTNESS];
          if (bright != FastLED.getBrightness()) {
            Serial.print(F("[SACN] updating brigthness to "));
            Serial.println(bright);
            FastLED.setBrightness(bright);
          }
        }
      }
    }
  }
}
