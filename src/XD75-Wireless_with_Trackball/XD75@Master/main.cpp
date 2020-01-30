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

#include "BatteryUtil.h"
#include "BleController.h"
#include "keymap.h"
#include "matrix.h"

using namespace hidpg;

void prph_cannot_connect_callback()
{
  // スレーブにsystemOffにすることを通知する
  // なんでもよいけどとりあえず0を送っておく
  uint8_t data = 0;
  BleController.Central.sendData(0, &data, 1);
  delay(1000);

  MatrixScan.stopTask_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void matrix_scan_callback(const Set &ids)
{
  HidEngine.applyToKeymap(ids);
}

void cent_receive_data_callback(uint8_t index, uint8_t *data, uint16_t len)
{
  struct Buf
  {
    int16_t delta_x;
    int16_t delta_y;
  };

  Buf *buf = reinterpret_cast<Buf *>(data);
  HidEngine.mouseMove(buf->delta_x, buf->delta_y);
}

void setup()
{
  // シリアルをオンにすると消費電流が増えるのでデバッグ時以外はオフにする
  // Serial.begin(115200);

  BleController.Periph.setCannnotConnectCallback(prph_cannot_connect_callback);
  BleController.Central.setReceiveDataCallback(cent_receive_data_callback);
  BleController.init();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleController.Periph.startConnection();
  BleController.Central.startConnection();

  MatrixScan.setCallback(matrix_scan_callback);
  MatrixScan.setMatrix(matrix, out_pins, in_pins);
  MatrixScan.init();
  MatrixScan.startTask();

  HidEngine.setKeymap(keymap);
  HidEngine.setTrackmap(trackmap);
  HidEngine.setHidReporter(BleController.Periph.getHidReporter());
  HidEngine.init();
  HidEngine.startTask();
}

void loop()
{
  BleController.Periph.setBatteryLevel(BatteryUtil.readBatteryLevel());
  delay(300000); //5 minites
}
