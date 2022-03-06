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

#include "Bluefruit_ConnectionController.h"
#include "HidEngine.h"
#include "PMW3360DM.h"

using namespace hidpg;

// デバイス名
constexpr const char *DEVICE_NAME = "SimpleTrackball";
// 電池の最大電圧
constexpr double MAX_BATTERY_VOLTAGE = 3.0;
// 電池の最小電圧
constexpr double MIN_BATTERY_VOLTAGE = 2.0;
// 電池の電圧を読み取る間隔 (1min)
constexpr uint32_t READ_BATTERY_VOLTAGE_INTERVAL_MS = (1000 * 60);
// マウスセンサーピン
constexpr uint8_t PMW3360DM_NCS_PIN = 30;
constexpr uint8_t PMW3360DM_INTERRUPT_PIN = 27;

BLEPeripheralProfileHid BLEProfile(BLE_APPEARANCE_HID_MOUSE);
BlinkLed AdvLed(PIN_LED1);
PMW3360DM &pmw3360dm = PMW3360DM::create<0>(ThreadSafeSPI, PMW3360DM_NCS_PIN, PMW3360DM_INTERRUPT_PIN);
constexpr PointingDeviceId PdTrackbacll{0};

void cannot_connect_callback()
{
  pmw3360dm.stop_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void motion_callback()
{
  HidEngine.movePointer(PdTrackbacll);
}

void read_pointer_delta_callback(PointingDeviceId pointing_device_id, int16_t &delta_x, int16_t &delta_y)
{
  if (pointing_device_id == PdTrackbacll)
  {
    pmw3360dm.readDelta(&delta_x, &delta_y);
    // トラックボールはセンサーを逆向きに取り付けるのでdelta_xを-にする
    delta_x *= -1;
  }
}

void setup()
{
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
  Bluefruit_ConnectionController.Periph.setCannotConnectCallback(cannot_connect_callback);
  Bluefruit_ConnectionController.Periph.start();
  HidReporter *hid_reporter = BLEProfile.getHidReporter();

  // HidEngine
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setReadPointerDeltaCallback(read_pointer_delta_callback);
  HidEngine.start();

  // PMW3360
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
  BLEProfile.Bas.notify(readBatteryLevel());
  delay(READ_BATTERY_VOLTAGE_INTERVAL_MS);
}
