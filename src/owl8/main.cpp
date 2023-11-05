/*
  The MIT License (MIT)

  Copyright (c) 2021 ogatatsu.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "Bluefruit_ConnectionController.h"
#include "DebounceIn.h"
#include "HidEngine.h"
#include "RotaryEncoder.h"
#include "keymap.h"
#include "variant.h"

using namespace hidpg;

// デバイス名
constexpr const char *DEVICE_NAME = "owl8";
// ロータリーエンコーダーピン
constexpr uint16_t ENCODER_LEFT_PIN_A = 4;
constexpr uint16_t ENCODER_LEFT_PIN_B = 5;
constexpr uint16_t ENCODER_RIGHT_PIN_A = 6;
constexpr uint16_t ENCODER_RIGHT_PIN_B = 7;
// 電池の最大電圧
constexpr double MAX_BATTERY_VOLTAGE = 3.0;
// 電池の最小電圧
constexpr double MIN_BATTERY_VOLTAGE = 2.0;
// 電池の電圧を読み取る間隔 (1min)
constexpr uint32_t READ_BATTERY_VOLTAGE_INTERVAL_MS = (1000 * 60);
// 一定時間操作しない場合自動的にSystemOffモードに (5min)
constexpr uint32_t AUTO_SYSTEM_OFF_TIMER_MS = (1000 * 60 * 5);

BLEPeripheralProfileHid BLEProfile(BLE_APPEARANCE_HID_KEYBOARD);
BlinkLed AdvLed(PIN_LED1, LOW);
RotaryEncoder &EncoderLeft = RotaryEncoder::create<EncLeft.value>(ENCODER_LEFT_PIN_A, ENCODER_LEFT_PIN_B);
RotaryEncoder &EncoderRight = RotaryEncoder::create<EncRight.value>(ENCODER_RIGHT_PIN_A, ENCODER_RIGHT_PIN_B);
SoftwareTimer SystemOffTimer;
Set ids;

void system_off()
{
  EncoderLeft.stop();
  EncoderRight.stop();
  DebounceIn.stop_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void cannot_connect_callback()
{
  system_off();
}

void system_off_timer_callback(void *timer_id)
{
  system_off();
}

void debounce_in_callback(uint8_t pin, bool state)
{
  SystemOffTimer.reset();

  switch (pin)
  {
  case A3:
    ids.update(1, !state);
    break;

  case A0:
    ids.update(2, !state);
    break;

  case MISO:
    ids.update(3, !state);
    break;

  case 10:
    ids.update(4, !state);
    break;

  case A2:
    ids.update(5, !state);
    break;

  case A1:
    ids.update(6, !state);
    break;

  case SCK:
    ids.update(7, !state);
    break;

  case MOSI:
    ids.update(8, !state);
    break;

  default:
    break;
  }

  HidEngine.applyToKeymap(ids);
}

void encoder_left_callback()
{
  SystemOffTimer.reset();

  HidEngine.rotateEncoder(EncLeft);
}

void encoder_right_callback()
{
  SystemOffTimer.reset();

  HidEngine.rotateEncoder(EncRight);
}

void read_encoder_step_callback(EncoderId encoder_id, int16_t &step)
{
  switch (encoder_id.value)
  {
  case EncLeft.value:
    step = EncoderLeft.readStep();
    break;

  case EncRight.value:
    step = EncoderRight.readStep();
    break;

  default:
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  // Bluefruit
  Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_DEFAULT, BLE_GAP_EVENT_LENGTH_DEFAULT, 2, BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
  Bluefruit.begin(1, 0);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  Bluefruit.setTxPower(8);
  Bluefruit.setName(DEVICE_NAME);

  // ConnectionController
  BLEProfile.begin();
  AdvLed.begin();
  Bluefruit_ConnectionController.begin();
  Bluefruit_ConnectionController.Periph.setProfile(&BLEProfile);
  Bluefruit_ConnectionController.Periph.setAdvLed(&AdvLed);
<<<<<<< HEAD
  Bluefruit_ConnectionController.Periph.setCannotConnectCallback(cannot_connect_callback);
=======
  Bluefruit_ConnectionController.Periph.setCannnotConnectCallback(cannot_connect_callback);
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34
  Bluefruit_ConnectionController.Periph.start();
  HidReporter *hid_reporter = BLEProfile.getHidReporter();

  // SystemOffTimer
  SystemOffTimer.begin(AUTO_SYSTEM_OFF_TIMER_MS, system_off_timer_callback, nullptr, false);
  SystemOffTimer.start();

  // Switch
  DebounceIn.addPin(A3, INPUT_PULLUP_SENSE, 5);   // sw1
  DebounceIn.addPin(A0, INPUT_PULLUP_SENSE, 5);   // sw2
  DebounceIn.addPin(MISO, INPUT_PULLUP_SENSE, 5); // sw3
  DebounceIn.addPin(10, INPUT_PULLUP_SENSE, 5);   // sw4
  DebounceIn.addPin(A2, INPUT_PULLUP_SENSE, 5);   // sw5
  DebounceIn.addPin(A1, INPUT_PULLUP_SENSE, 5);   // sw6
  DebounceIn.addPin(SCK, INPUT_PULLUP_SENSE, 5);  // sw7
  DebounceIn.addPin(MOSI, INPUT_PULLUP_SENSE, 5); // sw8
  DebounceIn.setCallback(debounce_in_callback);
  DebounceIn.start();

  // Encoder
  EncoderLeft.setCallback(encoder_left_callback);
  EncoderLeft.useFullStep(true);
  EncoderLeft.start();

  EncoderRight.setCallback(encoder_right_callback);
  EncoderRight.useFullStep(true);
  EncoderRight.start();

  // HidEngine
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setKeymap(keymap);
  HidEngine.setEncoderMap(encoderMap);
  HidEngine.setReadEncoderStepCallback(read_encoder_step_callback);
  HidEngine.start();
}

// 1/6 gain (GND ~ 3.6V) and 10bit (0 ~ 1023)
constexpr long MIN_ANALOG_VALUE = (MIN_BATTERY_VOLTAGE / 3.6 * 1023);
constexpr long MAX_ANALOG_VALUE = (MAX_BATTERY_VOLTAGE / 3.6 * 1023);

uint8_t readBatteryLevel()
{
  analogReference(AR_INTERNAL);
  analogReadResolution(10);
  uint32_t val = analogReadVDD();

  int level = map(val, MIN_ANALOG_VALUE, MAX_ANALOG_VALUE, 0, 100);
  return constrain(level, 0, 100);
}

void loop()
{
  BLEProfile.Bas.notify(readBatteryLevel());
  delay(READ_BATTERY_VOLTAGE_INTERVAL_MS);
}
