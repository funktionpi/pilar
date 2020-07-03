#include <cylon.h>
#include <FastLED.h>
#include <fx_registry.h>

Cylon::Cylon()
{
  FxRegistry.registerFX(this);
}

Cylon::~Cylon()
{
}

void Cylon::fadeall()
{
  for (int i = 0; i < ledCount; i++)
  {
    leds[i].nscale8(250);
  }
}

void Cylon::setup()
{
  goingLeft = true;
  current = 0;
}

void Cylon::loop()
{
  auto count = ledCount / 2;
  static uint8_t hue = 0;

  if (goingLeft)
  {
    leds[current] = CHSV(hue++, 255, 255);
    leds[count + current] = CHSV(hue++, 255, 255);
    fadeall();

    if (current == count - 1)
    {
      goingLeft = false;
    }
    else
    {
      current++;
    }
  }
  else
  {
    // Set the i'th led to red
    leds[current] = CHSV(hue++, 255, 255);
    leds[count + current] = CHSV(hue++, 255, 255);
    fadeall();

    current--;

    if (current == 0)
    {
      goingLeft = true;
    }
  }
}
