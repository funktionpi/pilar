#include "fx/cylon.h"

#include <FastLED.h>

#include "display.h"
#include "fx/registry.h"

Cylon::Cylon() { FxRegistry.registerFX(this); }

Cylon::~Cylon() {}

void Cylon::fadeall() {
  for (int i = 0; i < Display.count(); i++) {
    Display.pixel(i).nscale8(235);
  }
}

void Cylon::setup() {
  goingLeft = true;
  current = 0;
}

void Cylon::loop() {
  auto count = Display.count();
  static uint8_t hue = 0;

  if (goingLeft) {
    fadeall();
    Display.pixel(current) = CHSV(hue++, 255, 255);

    if (current == count - 1) {
      goingLeft = false;
    } else {
      current++;
    }
  } else {
    fadeall();
    Display.pixel(current) = CHSV(hue++, 255, 255);

    current--;

    if (current == 0) {
      goingLeft = true;
    }
  }
}
