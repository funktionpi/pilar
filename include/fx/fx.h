#pragma once

class CRGB;

class FX
{
  public:
    virtual void setup() {}
    virtual void loop() = 0;

    virtual int nextDelay() { return 0; }

    virtual const char* name() = 0;
};
