#include <Arduino.h>
#include <FastLED.h>
#include "fx/registry.h"

CFxRegistry FxRegistry;

#pragma GCC diagnostic ignored "-Wsign-compare"

using std::string;

CFxRegistry::CFxRegistry()
{
  count = 0;
}

void CFxRegistry::registerFX(FX *fx)
{
  // Serial.print(F("[FX] registering "));
  // Serial.print(fx->name());
  // Serial.println();

  fxmap[fx->name()] = fx;
  fxes[count] = fx;
  count++;
}

FX *CFxRegistry::random()
{
  return select(random8(count));
}

FX *CFxRegistry::select(int id)
{
  if (id >= count)
  {
    return nullptr;
  }
  current = fxes[id];
  current->setup();
  return current;
}

FX *CFxRegistry::select(const char *name)
{
  auto fx = fxmap.find(name);
  if (fx != fxmap.end())
  {
    current = fx->second;
    current->setup();
    return current;
  }
  else
  {
    return nullptr;
  }
}

void CFxRegistry::loop()
{
  if (current)
  {
    if (millis() - lastFxTick >= current->nextDelay())
    {
      current->loop();
      lastFxTick = millis();
    }
  }
}

void CFxRegistry::printRegistered()
{
  Serial.println(F("[FX] List of registered FXs:"));
  int i = 0;
  for (auto it : fxes)
  {
    if (i >= count)
    {
      return;
    }
    Serial.print(F("  "));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.println(it->name());
    i++;
  }
}
