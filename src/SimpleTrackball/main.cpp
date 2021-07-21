/*
  The MIT License (MIT)

  Copyright (c) 2019 ogatatsu.

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
#include "HidEngine.h"
#include "PMW3360DM.h"

using namespace hidpg;

PMW3360DM pmw3360dm = PMW3360DM::create<0>(ThreadSafeSPI, PMW3360DM_NCS_PIN, PMW3360DM_INTERRUPT_PIN);

void cannot_connect_callback()
{
  pmw3360dm.stopTask_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void motion_callback()
{
  HidEngine.mouseMove();
}

void read_mouse_delta_callback(int16_t *delta_x, int16_t *delta_y)
{
  pmw3360dm.readDelta(delta_x, delta_y);
  // トラックボールはセンサーを逆向きに取り付けるのでdelta_xを-にする
  *delta_x *= -1;
}

void setup()
{
  BleController.begin();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleController.Periph.setCannnotConnectCallback(cannot_connect_callback);
  BleController.Periph.startConnection();
  HidReporter *hid_reporter = BleController.Periph.getHidReporter();

  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.start();

  pmw3360dm.setCallback(motion_callback);
  pmw3360dm.start();
  pmw3360dm.changeCpi(PMW3360DM::Cpi::_1000);
}

// 1/6 gain (GND ~ 3.6V) and 10bit (0 ~ 1023)
#define MIN_ANALOG_VALUE (MIN_BATTERY_VOLTAGE / 3.6 * 1023)
#define MAX_ANALOG_VALUE (MAX_BATTERY_VOLTAGE / 3.6 * 1023)

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
  delay(60000); //1 minites
}
