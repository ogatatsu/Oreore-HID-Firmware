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

#include "BlinkLed.h"
#include "Bluefruit_ConnectionController.h"
#include "MatrixScan.h"
#include "matrix.h"

using namespace hidpg;

BLEPeripheralProfileUart BLEProfile;
BlinkLed AdvLed(PIN_LED1);

void cannot_connect_callback()
{
  MatrixScan.stop_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void receive_data_callback(uint8_t *data, uint16_t len)
{
  // マスターがsystemOffになったらスレーブもsystemOffにする
  if (len == 1 && data[0] == 0)
  {
    MatrixScan.stop_and_setWakeUpInterrupt();
    sd_power_system_off();
  }
}

void matrix_scan_callback(const Set &ids)
{
  // 配列にして送る
  uint16_t size = ids.count();
  uint8_t buf[size + 1];
  // Setが空でも空という情報は送りたいので１バイト目は適当になにか入れておいて、キーの押し情報は２バイト目以降に詰める
  buf[0] = 0;
  ids.toArray(buf + 1);
  BLEProfile.Uart.write(buf, sizeof(buf));
}

void setup()
{
  // Bluefruit
  Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_DEFAULT, BLE_GAP_EVENT_LENGTH_DEFAULT, 2, BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
  Bluefruit.begin(1, 0);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  ble_gap_addr_t own_addr = {
    .addr_id_peer = 0,
    .addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC,
    .addr = BLE_OWN_ADDR,
  };
  Bluefruit.setAddr(&own_addr);
  Bluefruit.setTxPower(8);

  // Bluefruit_ConnectionController
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

  // MatrixScan
  MatrixScan.setCallback(matrix_scan_callback);
  MatrixScan.setMatrix(matrix, out_pins, in_pins);
  MatrixScan.start();
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
