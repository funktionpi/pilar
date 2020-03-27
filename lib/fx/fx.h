#pragma once

class CRGB;

class FX
{
  public:
    virtual void setup() {}
    virtual void loop() = 0;

    virtual int nextDelay() { return 0; }

    virtual const char* name() = 0;

    static void setLeds(CRGB* leds, int ledCount)
    {
      FX::leds = leds;
      FX::ledCount = ledCount;
    }

    static int Count();

    static CRGB* leds;
    static int ledCount;
};
