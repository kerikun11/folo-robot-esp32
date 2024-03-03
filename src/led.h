#pragma once

#include <Arduino.h>

class LED {
public:
  LED() {}
  int begin(int pin_l, int pin_r) {
    pins_[0] = pin_l;
    pins_[1] = pin_r;
    setBoth();
    return 0;
  }
  void setBoth(float value_l = 1, float value_r = 1) {
    set(0, value_l);
    set(1, value_r);
  }
  void set(int ch, float value) {
    value_[ch] = std::max(0.0f, std::min(value, 1.0f));
    analogWrite(pins_[ch], 255 * value_[ch]);
  }
  float get(int ch) const {
    return value_[ch];
  }
  void off() {
    setBoth(0, 0);
  }

private:
  std::array<int, 2> pins_;
  std::array<float, 2> value_;
};
