#pragma once

#include <array>

class Reflector {
public:
  Reflector() {}
  int begin(int pin_tx, std::array<int, 2> pins_rx) {
    pin_tx_ = pin_tx;
    pins_rx_ = pins_rx;
    pinMode(pin_tx, OUTPUT);
    digitalWrite(pin_tx_, LOW);
    return 0;
  }
  void update() {
    digitalWrite(pin_tx_, HIGH);
    delayMicroseconds(10);
    const float value_min = 200;
    const float value_max = 2500;
    value_[0] = (analogRead(pins_rx_[0]) - value_min) / (value_max - value_min);
    value_[1] = (analogRead(pins_rx_[1]) - value_min) / (value_max - value_min);
    value_[0] = std::max(0.0f, std::min(value_[0], 1.0f));
    value_[1] = std::max(0.0f, std::min(value_[1], 1.0f));
    digitalWrite(pin_tx_, LOW);
  }
  float read(int ch) {
    return value_.at(ch);
  }

private:
  int pin_tx_;
  std::array<int, 2> pins_rx_;
  std::array<float, 2> value_;
};
