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

<<<<<<< HEAD
=======
#include "config.h"

>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34
#include "Bluefruit_ConnectionController.h"
#include "keymap.h"
#include "matrix.h"

using namespace hidpg;

<<<<<<< HEAD
// デバイス名
constexpr const char *DEVICE_NAME = "owl8";
// 電池の最大電圧
constexpr double MAX_BATTERY_VOLTAGE = 3.0;
// 電池の最小電圧
constexpr double MIN_BATTERY_VOLTAGE = 2.0;
// 電池の電圧を読み取る間隔 (1min)
constexpr uint32_t READ_BATTERY_VOLTAGE_INTERVAL_MS = (1000 * 60);
// アドバタイジング時に点滅させるLEDのピン番号
#define ADV_LED_PIN 14

BLEPeripheralProfileHid BLEProfile(BLE_APPEARANCE_HID_KEYBOARD);
BlinkLed AdvLed(ADV_LED_PIN);
=======
BLEPeripheralProfileHid BLEProfile(BLE_APPEARANCE_HID_KEYBOARD);
BlinkLed AdvLed(BLE_ADV_LED_PIN);
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34

void cannot_connect_callback()
{
  MatrixScan.stop_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void matrix_scan_callback(const Set &ids)
{
  HidEngine.applyToKeymap(ids);
}

void setup()
{
  // Bluefruit
  Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_DEFAULT, BLE_GAP_EVENT_LENGTH_DEFAULT, 2, BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
  Bluefruit.begin(1, 0);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  Bluefruit.setTxPower(8);
<<<<<<< HEAD
  Bluefruit.setName(DEVICE_NAME);
=======
  Bluefruit.setName(BLE_DEVICE_NAME);
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34

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

  // MatrixScan
  MatrixScan.setCallback(matrix_scan_callback);
  MatrixScan.setMatrix(matrix, out_pins, in_pins);
  MatrixScan.start();

  // HidEngine
  HidEngine.setKeymap(keymap);
  HidEngine.setHidReporter(hid_reporter);
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
