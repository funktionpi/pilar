#pragma once
#include <Arduino.h>

class CRGB;

class FX
{
  public:
    virtual void setup() {}
    virtual void loop() = 0;

    // return the number of milliseconds for the next loop call
    virtual int nextDelay() { return 0; }

    virtual const char* name() = 0;

    virtual void setParam(int index, uint8 value) {}
    virtual uint8 param(int index) { return 0; }
    virtual int paramCount() { return 0; }
};
