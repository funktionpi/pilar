#pragma once

#define LED_STRIP_COUNT 2
#define LED_COUNT_PER_STRIP 144
#define LED_COUNT LED_COUNT_PER_STRIP * LED_STRIP_COUNT

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define LED_PIN1 5 // aka D1
#define LED_PIN2 4 // aka D2

#define UNIVERSE 1       // First DMX Universe to listen for
#define UNIVERSE_COUNT 2 // Total number of Universes to listen for, starting at UNIVERSE

#define ENABLE_ARTNET 1
#define ENABLE_SACN 1

#define DEBUG_ARTNET 0
#define DEBUG_SACN 1
