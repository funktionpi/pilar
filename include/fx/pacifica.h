#pragma once

#include <fx/fx.h>

class Pacifica : public FX
{
public:
  Pacifica();
  int nextDelay() { return 20; }
  void loop();

  const char* name() { return "pacifica"; }
};
