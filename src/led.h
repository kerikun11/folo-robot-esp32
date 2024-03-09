#pragma once

#include <Arduino.h>

class LED {
public:
  enum Pattern {
    OFF,
    L_ON,
    L_ONLY,
    R_ON,
    R_ONLY,
    BOTH_ON,
    BLINK,
    BLINK_ALT,
  };

public:
  LED() {}
  int begin(int pin_l, int pin_r) {
    pins_[0] = pin_l;
    pins_[1] = pin_r;
    setBoth();
    // Player Task
    xTaskCreatePinnedToCore(
      [](void* arg) {
        static_cast<decltype(this)>(arg)->task();
      },
      "LED",
      4096, this, 1, NULL, tskNO_AFFINITY);
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
  void play(const Pattern pattern, TickType_t xTicksToWait = 0) {
    xQueueSendToBack(play_list_, &pattern, xTicksToWait);
  }

private:
  std::array<int, 2> pins_;
  std::array<float, 2> value_;
  QueueHandle_t play_list_ = xQueueCreate(20, sizeof(Pattern));

  void task() {
    while (1) {
      Pattern pattern = OFF;
      if (xQueueReceive(play_list_, &pattern, portMAX_DELAY) == pdTRUE) {
        set_pattern(pattern);
      }
    }
  }
  void set_pattern(Pattern pattern) {
    switch (pattern) {
      case OFF:
        return off();
      case L_ON:
        return set(0, 1);
      case R_ON:
        return set(1, 1);
      case BOTH_ON:
        return setBoth(1, 1);
      case BLINK:
        for (int i = 0; i < 10; ++i) {
          setBoth(0, 0);
          delay(100);
          setBoth(1, 1);
          delay(100);
        }
        return;
      case BLINK_ALT:
        for (int i = 0; i < 10; ++i) {
          setBoth(0, 1);
          delay(100);
          setBoth(1, 0);
          delay(100);
        }
        setBoth(1, 1);
        return;
    }
  }
};
