#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

#include "config.h"
#include "ota.h"
#include "button.h"
#include "speaker.h"
#include "led.h"
#include "motor.h"
#include "reflector.h"
#include "imu.h"

Speaker speaker;
LED led;
Button button;
Motor motor;
Reflector reflector;
IMU imu;
WebServer server;

void setup() {
  log_i("Hello, from " ARDUINO_BOARD ".");

  /* Drivers */
  speaker.begin(PIN_SPEAKER, LEDC_TIMER_1, LEDC_CHANNEL_0);
  speaker.play(Speaker::BOOT);
  led.begin(PIN_LED_L, PIN_LED_R);
  analogWriteFrequency(20000);
  analogWriteResolution(8);

  /* WiFi */
  led.setBoth();
  WiFi.begin();
  log_i("Connecting to %s ...", WiFi.SSID().c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    log_e("WiFi connection failed");
    speaker.play(Speaker::ERROR);
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    while (!WiFi.smartConfigDone()) {
      led.set(true, false);
      delay(200);
      led.set(false, true);
      delay(200);
    }
  }
  log_i("WiFi connected. SSID: %s IP: %s", WiFi.SSID().c_str(),
        WiFi.localIP().toString().c_str());
  OTA::start(HOSTNAME);
  speaker.play(Speaker::SUCCESSFUL);

  /* Other Drivers */
  SPIFFS.begin();
  button.begin((gpio_num_t)PIN_BUTTON);
  motor.begin(PINS_MOTOR_TRANSLATION, PINS_MOTOR_ROTATION);
  reflector.begin(PIN_REFLECTOR_TX, PINS_REFLECTOR_RX);
  speaker.play(Speaker::CALIBRATION);
  if (imu.begin() < 0) speaker.play(Speaker::DOWN);
  speaker.play(Speaker::CANCEL);

  /* Web Server */
  log_i("WebServer start");
  server.on("/led/set", [&]() {
    log_i("URI: %s", server.uri().c_str());
    for (int i = 0; i < server.args(); i++)
      log_i("- args[%s]: %s", server.argName(i).c_str(), server.arg(i).c_str());
    if (server.hasArg("l")) {
      float value_l = server.arg("l").toFloat();
      led.set(0, value_l);
    }
    if (server.hasArg("r")) {
      float value_r = server.arg("r").toFloat();
      led.set(1, value_r);
    }
    server.send(200, "plain/text", "OK");
  });
  server.on("/motor/set", [&]() {
    log_i("URI: %s", server.uri().c_str());
    for (int i = 0; i < server.args(); i++)
      log_i("- args[%s]: %s", server.argName(i).c_str(), server.arg(i).c_str());
    float trans = server.arg("trans").toFloat();
    float rot = server.arg("rot").toFloat();
    motor.drive(trans, rot);
    delay(100);
    motor.free();
    server.send(200, "plain/text", "OK");
  });
  server.serveStatic("/", SPIFFS, "/www/");
  server.begin();

  /* main loop*/
  int counter = 0;
  float angle_integral = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
    OTA::handle();
    server.handleClient();

#if 0
    reflector.update();
    led.set(reflector.read(0), reflector.read(1));
    log_i("%f %f", reflector.read(0), reflector.read(1));
#endif

#if 0  //< 宴会芸
    if (counter % 2 == 1) {
      const float Kp = 5e-2f;
      const float Ki = 1e-5f;
      float angle = imu.getAngle();
      angle_integral += angle;
      float value = Kp * angle + Ki * angle_integral;
      motor.drive(0, -value);
      log_i("Kp: %f Ki: %f angle: %10f int: %10f value: %10f = %10f + %10f",
            Kp, Ki, angle, angle_integral, value, Kp * angle, Ki * angle_integral);
    } else {
      motor.free();
    }
#endif

    if (button.pressed) {
      button.pressed = 0;
      speaker.play(Speaker::SELECT);
      // if (counter % 5 == 0) motor.drive(+1, 0);
      // if (counter % 5 == 1) motor.drive(-1, 0);
      // if (counter % 5 == 2) motor.drive(0, +1);
      // if (counter % 5 == 3) motor.drive(0, -1);
      // if (counter % 5 == 4) motor.free();
      counter++;
    }
    if (button.long_pressed_1) {
      button.long_pressed_1 = 0;
      speaker.play(Speaker::CALIBRATION);
      delay(1000);
      imu.calibration();
      angle_integral = 0;
      speaker.play(Speaker::CANCEL);
    }
  }
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
