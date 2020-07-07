#include "config.h"
#include <FastLED.h>
#include <fx/registry.h>
#include <fx/fx.h>
#include <fx/pacifica.h>
#include <fx/fire2012.h>
#include <fx/cylon.h>

#include "task.h"

#include "display.h"

void led_loop();
Task tLed(TASK_SECOND / 90, TASK_FOREVER, &led_loop, &ts);

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
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);

  // allocate fx
  new Pacifica();
  new Fire2012();
  new Cylon();

  FastLED.addLeds<WS2812B, LED_PIN1, GRB>(Display.target(), 0, LED_COUNT_PER_STRIP);
  FastLED.addLeds<WS2812B, LED_PIN2, GRB>(Display.target(), LED_COUNT_PER_STRIP, LED_COUNT_PER_STRIP);
  FastLED.setCorrection(TypicalSMD5050);
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, 12 * 1000);
  FastLED.setMaxRefreshRate(60, false);
  FastLED.setBrightness(255);
  FastLED.clear();

  FxRegistry.printRegistered();
  Display.printSettings();

  random16_add_entropy(millis()); // add random
  changefx();
  FxRegistry.select("fire2012");

  tLed.setOnEnable([]() -> bool {
    Serial.println(F("[LED] task enabled"));
    return true;
  });
  tLed.setOnDisable([]() {
    Serial.println(F("[LED] task disabled"));
  });

  ts.addTask(tLed);
  tLed.enable();
}

void led_loop()
{
  EVERY_N_MINUTES(1)
  // EVERY_N_SECONDS(10)
  {
    changefx();
  }

  FxRegistry.loop();
  Display.update();

  FastLED.show();

  EVERY_N_SECONDS(5)
  {
    Serial.print(F("[LED] Current FPS: "));
    Serial.print(FastLED.getFPS());
    Serial.println();
  }
}
