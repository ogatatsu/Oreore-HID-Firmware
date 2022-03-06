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

#include "Bluefruit_ConnectionController.h"
#include "keymap.h"
#include "matrix.h"

using namespace hidpg;

BLECentralProfileUart RightSide(BLE_PEER_ADDR);
BLEPeripheralProfileHid BLEProfile(BLE_APPEARANCE_HID_KEYBOARD);
BlinkLed AdvLed(PIN_LED1);
BlinkLed ScanLed(PIN_LED2);
Set left_ids, right_ids;
SemaphoreHandle_t callback_mutex;
StaticSemaphore_t mutex_buffer;

void prph_cannot_connect_callback()
{
  // スレーブにsystemOffにすることを通知する
  // なんでもよいけどとりあえず0を送っておく
  uint8_t data = 0;
  RightSide.Uart.write(&data, 1);
  delay(1000);

  Bluefruit_ConnectionController.Central.stop();
  MatrixScan.stop_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void uart_rx_callback(uint8_t *data, uint16_t len)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  right_ids.clear();
  // 1バイト目は飛ばす
  right_ids.addAll(data + 1, len - 1);
  HidEngine.applyToKeymap(left_ids | right_ids);
  xSemaphoreGive(callback_mutex);
}

void cent_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  // 切断されたらキーが押しっぱなしにならないように空のデータを送る
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  right_ids.clear();
  HidEngine.applyToKeymap(left_ids | right_ids);
  xSemaphoreGive(callback_mutex);
}

void matrix_scan_callback(const Set &ids)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  left_ids = ids;
  HidEngine.applyToKeymap(left_ids | right_ids);
  xSemaphoreGive(callback_mutex);
}

void setup()
{
  Serial.begin(115200);

  callback_mutex = xSemaphoreCreateMutexStatic(&mutex_buffer);

  // Bluefruit
  Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_DEFAULT, BLE_GAP_EVENT_LENGTH_DEFAULT, 2, BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
  Bluefruit.begin(1, 1);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  Bluefruit.setTxPower(8);
  Bluefruit.setName(BLE_DEVICE_NAME);

  // Central
  RightSide.begin();
  RightSide.Uart.setRxCallback(uart_rx_callback);
  ScanLed.begin();

  Bluefruit_ConnectionController.begin();
  Bluefruit_ConnectionController.Central.setProfile(&RightSide);
  Bluefruit_ConnectionController.Central.setScanLed(&ScanLed);
  Bluefruit_ConnectionController.Central.setDisconnectCallback(cent_disconnect_callback);
  Bluefruit_ConnectionController.Central.start();

  // Peripheral
  BLEProfile.begin();
  AdvLed.begin();

  Bluefruit_ConnectionController.Periph.setProfile(&BLEProfile);
  Bluefruit_ConnectionController.Periph.setAdvLed(&AdvLed);
  Bluefruit_ConnectionController.Periph.setCannnotConnectCallback(prph_cannot_connect_callback);
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
