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

#include "config.h"

#include "BleController.h"
#include "DebounceIn.h"
#include "HidEngine.h"
#include "RotaryEncoder.h"
#include "keymap.h"
#include "variant.h"

using namespace hidpg;

Set ids;
RotaryEncoder<ENCODER_LEFT_ID> EncoderLeft;
RotaryEncoder<ENCODER_RIGHT_ID> EncoderRight;
SoftwareTimer SystemOffTimer;

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

  HidEngine.rotateEncoder(ENCODER_LEFT_ID);
}

void encoder_right_callback()
{
  SystemOffTimer.reset();

  HidEngine.rotateEncoder(ENCODER_RIGHT_ID);
}

void read_encoder_step_callback(uint8_t encoder_id, int32_t &step)
{
  switch (encoder_id)
  {
  case ENCODER_LEFT_ID:
    step = EncoderLeft.readStep();
    break;

  case ENCODER_RIGHT_ID:
    step = EncoderRight.readStep();
    break;

  default:
    break;
  }
}


void setup()
{
  // Serial.begin(115200);

  BleController.begin();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleController.Periph.setCannnotConnectCallback(cannot_connect_callback);
  BleController.Periph.startConnection();
  HidReporter *hid_reporter = BleController.Periph.getHidReporter();

  SystemOffTimer.begin(AUTO_SYSTEM_OFF_TIMER_MS, system_off_timer_callback, nullptr, false);
  SystemOffTimer.start();

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

  EncoderLeft.setCallback(encoder_left_callback);
  EncoderLeft.setPins(4, 5);
  EncoderLeft.useFullStep(true);
  EncoderLeft.start();

  EncoderRight.setCallback(encoder_right_callback);
  EncoderRight.setPins(6, 7);
  EncoderRight.useFullStep(true);
  EncoderRight.start();

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
  BleController.Periph.setBatteryLevel(readBatteryLevel());
  delay(READ_BATTERY_VOLTAGE_INTERVAL_MS);
}
