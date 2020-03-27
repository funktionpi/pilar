#include "config.h"
#include <FastLED.h>
#include <fx_registry.h>

#include <fx.h>
#include <pacifica.h>
#include <fire2012.h>
#include <cylon.h>

CRGB leds[LED_COUNT];

// allocate fx
Pacifica pacifica;
Fire2012 fire2012;
Cylon cylon;

void changefx()
{
  auto fx = FxRegistry.random();
  if (fx)
  {
    Serial.print(F("[LED] switching to FX "));
    Serial.println(fx->name());
  }
  else
  {
    Serial.println(F("[LED] no FX selected."));
  }
}

void led_setup()
{
  Serial.println(F("[LED] setting up FastLED"));
  pinMode(LED_PIN, OUTPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, 0, LED_COUNT_PER_STRIP);
  FastLED.addLeds<WS2812B, LED_PIN+1, GRB>(leds, LED_COUNT_PER_STRIP, LED_COUNT_PER_STRIP);
  FastLED.setCorrection(TypicalSMD5050);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 16 * 1000);
  FastLED.setMaxRefreshRate(60, false);
  FastLED.setBrightness(32);
  FastLED.clear();

  FX::setLeds(leds, LED_COUNT);

  FxRegistry.printRegistered();

  random16_add_entropy(millis()); // add random
  // changefx();
  FxRegistry.select("fire2012");
}

void led_loop()
{
  EVERY_N_MINUTES(1)
  {
    changefx();
  }

  FxRegistry.loop();

  FastLED.show();

  EVERY_N_SECONDS(5)
  {
    Serial.print(F("[LED] Current FPS: "));
    Serial.print(FastLED.getFPS());
    Serial.println();
  }
}
