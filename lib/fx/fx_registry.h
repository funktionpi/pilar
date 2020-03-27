#pragma once

#include <fx.h>
#include <map>
#include <string>

class CFxRegistry
{
public:
  CFxRegistry();

  FX *select(const char *name);

  // Can only be from 0 to 255
  FX *select(int id);

  // pick randomly a new FX
  FX *random();

  // call this every tick, ideally without delays
  void loop();

  void registerFX(FX *fx);

  void printRegistered();

private:

  std::map<std::string, FX*> fxmap;
  FX* current;
  unsigned long lastFxTick;
  FX* fxes[255];
  int count;
};

extern CFxRegistry FxRegistry;
