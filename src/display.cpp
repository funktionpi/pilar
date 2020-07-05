#include "config.h"
#include <FastLED.h>
#include "display.h"

CDisplay Display;

static const char *debugMirrorStr[] =
    {
        "None",
        "Strip",
};

static const char *debugOriginStr[] =
    {
        "Top",
        "Bottom",
        "Center",
};

CDisplay::CDisplay()
{
  _leds = new CRGB[LED_COUNT];
  _tmp = new CRGB[LED_COUNT];
  _mirror = Strip;
  _origin = Top;
}

int CDisplay::count()
{
  auto count = LED_COUNT;

  if (_mirror)
  {
    count /= LED_STRIP_COUNT;
  }

  if (_origin == Origin::Center)
  {
    count /= 2;
  }

  return count;
}

int CDisplay::maxCount()
{
  return LED_COUNT;
}

CRGB &CDisplay::pixel(int index)
{
  if (index >= count())
  {
    Serial.print("[DISPLAY] invalid index: ");
    Serial.println(index);
    index = LED_COUNT - 1;
  }
  return _tmp[index];
}

/*
  Copy buffer into target led information, taking into consideration mirror and origin
*/
void CDisplay::update()
{
  auto cnt = count();

  for (size_t i = 0; i < cnt; i++)
  {
    auto idx = _origin == Top ? LED_COUNT_PER_STRIP - 1 - i : 1;
    _leds[idx] = _tmp[i];
  }

  // invert content of led array
  // else if (_origin == Origin::Center)
  // {
  //   // index = LED_COUNT_PER_STRIP / 2 + i;
  //   // _leds[index - i] = color;
  // }

  if (_mirror == Mirroring::Strip)
  {
    for (size_t i = 0; i < cnt; i++)
    {
      _leds[LED_COUNT_PER_STRIP + i] = _leds[i];
    }
  }
}

void CDisplay::setAllColor(CRGB &color)
{
  for (size_t i = 0; i < LED_COUNT; i++)
  {
    _tmp[i] = color;
  }
}

void CDisplay::printSettings()
{
  Serial.print("[DISPLAY] Pixel count = ");
  Serial.print(LED_COUNT);
  Serial.print(" = ");
  Serial.print(LED_COUNT_PER_STRIP);
  Serial.print("(leds) x ");
  Serial.print(LED_STRIP_COUNT);
  Serial.println("(strip)");

  Serial.print("[DISPLAY] Mirroring: ");
  Serial.println(debugMirrorStr[_mirror]);

  Serial.print("[DISPLAY] Origin: ");
  Serial.println(debugOriginStr[_origin]);

  Serial.print("[DISPLAY] Computed LED count: ");
  Serial.println(count());
}
