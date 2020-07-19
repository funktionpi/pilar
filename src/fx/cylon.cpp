#include "fx/cylon.h"

#include "led.h"
#include "display.h"
#include "fx/registry.h"

Cylon::Cylon() { FxRegistry.registerFX(this); }

Cylon::~Cylon() {}

void Cylon::fadeall() {
  for (int i = 0; i < Display.count(); i++) {
    Display[current].nscale8(225);
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
    Display[current] = CHSV(hue++, 255, 255);

    if (++current == count - 1) {
      goingLeft = false;
    }
  } else {
    fadeall();
    Display[current] = CHSV(hue++, 255, 255);

    if (--current == 0) {
      goingLeft = true;
    }
  }
}
