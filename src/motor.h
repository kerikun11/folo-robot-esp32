#pragma once

#include <Arduino.h>

class Motor {
private:
  static constexpr float kEmergencyThreshold = 1.3f;

public:
  Motor() {}
  int begin(std::array<int, 2> pins_trans, std::array<int, 2> pins_rot) {
    pins_trans_ = pins_trans;
    pins_rot_ = pins_rot;
    analogWriteFrequency(20000);
    analogWriteResolution(8);
    free();
    return 0;
  }
  void drive(float trans, float rot) {
    drive_trans(trans);
    drive_rot(rot);
  }
  void drive_trans(float trans) {
    drive_pins(pins_trans_, trans);
  }
  void drive_rot(float rot) {
    drive_pins(pins_rot_, rot);
  }
  void free() {
    analogWrite(pins_rot_[0], 0);
    analogWrite(pins_rot_[1], 0);
    analogWrite(pins_trans_[0], 0);
    analogWrite(pins_trans_[1], 0);
  }
  void emergency_stop() {
    emergency_ = true;
    free();
  }
  void emergency_release() {
    emergency_ = false;
    free();
  }
  bool is_emergency() const {
    return emergency_;
  }

private:
  bool emergency_ = false;
  std::array<int, 2> pins_trans_, pins_rot_;

  void drive_pins(const std::array<int, 2>& pins, float duty) {
    if (emergency_) return;
    /* saturate */
    duty = std::max(-1.0f, std::min(duty, 1.0f));
    const int pwm_max = 255;
    /* apply */
    if (duty > 0) {
      analogWrite(pins[0], pwm_max);
      analogWrite(pins[1], pwm_max * (1 - duty));
    } else {
      analogWrite(pins[0], pwm_max * (1 + duty));
      analogWrite(pins[1], pwm_max);
    }
  }
};
