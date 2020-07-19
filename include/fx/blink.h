#pragma once

#include <fx/fx.h>

class Blink : public FX
{
  public:
    Blink();
    ~Blink();

    void setup();
    void loop();

    int nextDelay() { return 10; }

    const char* name() { return Name; }
    static constexpr const char* Name = "blink";


};
