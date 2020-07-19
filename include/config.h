#pragma once

#define LED_STRIP_COUNT 2
#define LED_COUNT_PER_STRIP 144
#define LED_COUNT LED_COUNT_PER_STRIP* LED_STRIP_COUNT

// Use D[x] notation
#define LED_PIN1 3
#define LED_PIN2 4

// Raw pin #
#define I2C_CLK_PIN 5    // aka D1
#define I2C_DATA_PIN 4   // aka D2
#define INTERRUPT_PIN 15 // aka D8

#define UNIVERSE 1        // First DMX Universe to listen for
#define UNIVERSE_CTRL  1  // universe to use for controls
#define UNIVERSE_COUNT 2  // Total number of Universes to listen for, starting at UNIVERSE

#define ENABLE_OTA 1
#define ENABLE_ARTNET 1
#define ENABLE_SACN 1
#define ENABLE_SENSOR 1

#define DEBUG_ARTNET 0
#define DEBUG_SACN 0

#define CHANNEL_FX 1
#define CHANNEL_BRIGHTNESS 2
#define CHANNEL_SPEED 3
#define CHANNEL_ORIGIN 4
#define CHANNEL_MIRROR 5
