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
    free();
    pinMode(pins_trans_[0], OUTPUT);
    pinMode(pins_trans_[1], OUTPUT);
    pinMode(pins_rot_[0], OUTPUT);
    pinMode(pins_rot_[1], OUTPUT);
    return 0;
  }
  void drive(float trans, float rot) {
    if (emergency_) return;
    /* saturate */
    trans = std::max(-1.0f, std::min(trans, 1.0f));
    rot = std::max(-1.0f, std::min(rot, 1.0f));
    /* apply */
    if (trans > 0) {
      analogWrite(pins_trans_[0], 255 * trans);
      analogWrite(pins_trans_[1], 0);
    } else {
      analogWrite(pins_trans_[0], 0);
      analogWrite(pins_trans_[1], -255 * trans);
    }
    if (rot > 0) {
      analogWrite(pins_rot_[0], 255 * rot);
      analogWrite(pins_rot_[1], 0);
    } else {
      analogWrite(pins_rot_[0], 0);
      analogWrite(pins_rot_[1], -255 * rot);
    }
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
};
