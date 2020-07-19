#pragma once

#include <Arduino.h>
#include <fx/fx.h>

class Fire2012 : public FX
{
  public:
    Fire2012();
    ~Fire2012();

    void setup();
    void loop();

    const char* name() { return Name; }
    static constexpr const char* Name = "fire2012";

  private:
    byte* _heat;
};
