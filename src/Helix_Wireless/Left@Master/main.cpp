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
#include "keymap.h"
#include "matrix.h"

using namespace hidpg;

Set scan_ids, slave_ids;
// callbackはそれぞれ別タスクなので共通で使う変数に触る時はmutexで囲む
SemaphoreHandle_t mutex;

void prph_cannot_connect_callback()
{
  // スレーブにsystemOffにすることを通知する
  // なんでもよいけどとりあえず0を送っておく
  uint8_t data = 0;
  BleController.Central.sendData(0, &data, 1);
  delay(1000);

  BleController.Central.stopConnection();
  MatrixScan.stopTask_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void cent_receive_data_callback(uint8_t index, uint8_t *data, uint16_t len)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  slave_ids.clear();
  // 1バイト目は飛ばす
  slave_ids.addAll(data + 1, len - 1);
  HidEngine.applyToKeymap(scan_ids | slave_ids);
  xSemaphoreGive(mutex);
}

void cent_disconnect_callback(uint8_t index, uint8_t reason)
{
  // 切断されたらキーが押しっぱなしにならないように空のデータを送る
  xSemaphoreTake(mutex, portMAX_DELAY);
  slave_ids.clear();
  HidEngine.applyToKeymap(scan_ids | slave_ids);
  xSemaphoreGive(mutex);
}

void matrix_scan_callback(const Set &ids)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  scan_ids = ids;
  HidEngine.applyToKeymap(scan_ids | slave_ids);
  xSemaphoreGive(mutex);
}

void setup()
{
  mutex = xSemaphoreCreateMutex();

  BleController.Periph.setCannnotConnectCallback(prph_cannot_connect_callback);
  BleController.Central.setReceiveDataCallback(cent_receive_data_callback);
  BleController.Central.setDisconnectCallback(cent_disconnect_callback);
  BleController.begin();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleController.Periph.startConnection();
  BleController.Central.startConnection();

  MatrixScan.setCallback(matrix_scan_callback);
  MatrixScan.setMatrix(matrix, out_pins, in_pins);
  MatrixScan.begin();

  HidEngine.setKeymap(keymap);
  HidEngine.setSimulKeymap(simul_keymap);
  HidEngine.setHidReporter(BleController.Periph.getHidReporter());
  HidEngine.begin();
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
