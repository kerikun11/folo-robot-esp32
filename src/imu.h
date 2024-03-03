#pragma once

#include <MPU9250.h>

#include <mutex>
#include <condition_variable>

class IMU {
public:
  IMU()
    : mpu_(Wire, 0x68) {
  }
  int begin() {
    int res = mpu_.begin();
    if (res < 0) {
      log_e("MPU init failed. res: %d", res);
      return -1;
    }
    xTaskCreatePinnedToCore(
      [](void* arg) {
        static_cast<decltype(this)>(arg)->task();
      },
      "Button", 4096, this, 1, NULL, tskNO_AFFINITY);
    return 0;
  }
  float getAngle() const {
    return angle_;
  }
  void calibration() {
    calibration_req_ = true;
    /* wait for calibration finished */
    std::unique_lock<std::mutex> unique_lock(calibration_mutex_);
    calibration_cv_.wait(unique_lock, [&] {
      return !calibration_req_;
    });
  }

private:
  MPU9250 mpu_;
  float angle_ = 0;
  uint32_t last_timestamp_us_ = 0;

  bool calibration_req_ = false;
  std::mutex calibration_mutex_;
  std::condition_variable calibration_cv_;

  void task() {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
      update();
      /* calibration */
      if (calibration_req_) {
        task_calibration();
        std::lock_guard<std::mutex> lock_guard(calibration_mutex_);
        calibration_req_ = false;
        calibration_cv_.notify_all();
      }
    }
  }
  void task_calibration() {
    mpu_.calibrateGyro();
    angle_ = 0;
    last_timestamp_us_ = 0;
  }
  void update() {
    mpu_.readSensor();
    float gyro = mpu_.getGyroX_rads() * 180 / float(M_PI);
    uint32_t us = micros();
    if (last_timestamp_us_ > 0) {
      uint32_t diff_us = us - last_timestamp_us_;
      angle_ += gyro * diff_us * 1e-6f;
    }
    last_timestamp_us_ = us;
  }
};
