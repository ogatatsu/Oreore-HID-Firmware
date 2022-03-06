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

#include "BleControllerSlave.h"
#include "PMW3360DM.h"

using namespace hidpg;

PMW3360DM pmw3360dm = PMW3360DM::create<0>(ThreadSafeSPI, PMW3360DM_NCS_PIN, PMW3360DM_INTERRUPT_PIN);

void cannot_connect_callback()
{
  pmw3360dm.stop_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void receive_data_callback(uint8_t *data, uint16_t len)
{
  // マスターがsystemOffになったらスレーブもsystemOffにする
  if (len == 1 && data[0] == 0)
  {
    pmw3360dm.stop_and_setWakeUpInterrupt();
    sd_power_system_off();
  }
}

void motion_callback()
{
  struct
  {
    int16_t delta_x;
    int16_t delta_y;
  } buf;

  BleControllerSlave.waitReady();
  pmw3360dm.readDelta(&buf.delta_x, &buf.delta_y);
  // トラックボールはセンサーを逆向きに取り付けるのでdelta_xを-にする
  buf.delta_x *= -1;
  BleControllerSlave.sendData(reinterpret_cast<uint8_t *>(&buf), sizeof(buf));
}

void setup()
{
  BleControllerSlave.begin();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleControllerSlave.setCannotConnectCallback(cannot_connect_callback);
  BleControllerSlave.setReceiveDataCallback(receive_data_callback);
  BleControllerSlave.startConnection();

  pmw3360dm.setCallback(motion_callback);
  pmw3360dm.start();
  pmw3360dm.changeCpi(PMW3360DM::Cpi::_1000);
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
  BleControllerSlave.setBatteryLevel(readBatteryLevel());
  delay(READ_BATTERY_VOLTAGE_INTERVAL_MS);
}
