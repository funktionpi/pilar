#pragma once

#include <fx/fx.h>

class Cylon : public FX
{
  public:
    Cylon();
    ~Cylon();

    void setup();
    void loop();

    int nextDelay() { return 10; }

    const char* name() { return "cylon"; }

  private:
    void fadeall();

    bool goingLeft;
    int  current;
};
