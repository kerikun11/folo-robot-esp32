#pragma once

#include <ArduinoOTA.h>

class OTA {
public:
  static int start(const char* hostname) {
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.onStart([]() {
      log_i("OTA Start");
    });
    ArduinoOTA.onEnd([]() {
      log_i("OTA End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      log_i("OTA Progress: %u%%", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t err) {
      log_e("OTA Error[%u]: ", err);
    });
    ArduinoOTA.begin();
    return 0;
  }
  static void handle() {
    ArduinoOTA.handle();
  }

private:
};
