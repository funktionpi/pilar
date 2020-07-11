#pragma once

class CRGB;

enum Origin {
  Top,  // led zero is near the power supply
  Bottom,
  Center,  // if centered, this will automatically mirror based on center
};

enum Mirroring {
  None = 0x0,
  Strip = 0x1,
};

class CDisplay {
 public:
  CDisplay();

  // number of LEDs which can be addressed
  int count();
  int maxCount();
  void setAllColor(CRGB& color);

  CRGB& pixel(int index);

  Mirroring mirror() { return _mirror; }
  void setMirror(Mirroring mirror) { _mirror = mirror; }

  void setOrigin(Origin origin) { _origin = origin; }
  Origin origin() { return _origin; }

  void update();
  CRGB* raw() { return _tmp; }
  CRGB* target() { return _target; }

  void printSettings();

 private:
  Mirroring _mirror;  // set leds on both side to the same color
  Origin _origin;

  CRGB* _tmp;
  CRGB* _target;
};

extern CDisplay Display;
