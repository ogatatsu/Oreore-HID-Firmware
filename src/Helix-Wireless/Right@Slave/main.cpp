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

#include "BatteryUtil.h"
#include "BleControllerSlave.h"
#include "MatrixScan.h"
#include "matrix.h"

using namespace hidpg;

QueueHandle_t eventQueue;

void prph_cannot_connect_callback()
{
  MatrixScan::stopTask_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void receive_data_callback(uint8_t *data, uint16_t len)
{
  // マスターがsystemOffになったらスレーブもsystemOffにする
  if (len == 1 && data[0] == 0)
  {
    MatrixScan::stopTask_and_setWakeUpInterrupt();
    sd_power_system_off();
  }
}

void keyscan_callback(const Set &ids)
{
  // 配列にして送る
  uint16_t size = ids.count();
  uint8_t buf[size + 1];
  // Setが空でも空という情報は送りたいので１バイト目は適当になにか入れておいて、キーの押し情報は２バイト目以降に詰める
  buf[0] = 0;
  ids.toArray(buf + 1);
  BleControllerSlave::sendData(buf, sizeof(buf));
}

void setup()
{
  // シリアルをオンにすると消費電流が増えるのでデバッグ時以外はオフにする
  // Serial.begin(115200);

  BleControllerSlave::setPrphCannnotConnectCallback(prph_cannot_connect_callback);
  BleControllerSlave::setReceiveDataCallback(receive_data_callback);
  BleControllerSlave::init();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleControllerSlave::startPrphConnection();

  MatrixScan::setKeyscanCallback(keyscan_callback);
  MatrixScan::setMatrix(matrix, outPins, inPins);
  MatrixScan::init();
  MatrixScan::startTask();
}

void loop()
{
  BleControllerSlave::setBatteryLevel(BatteryUtil::readBatteryLevel());
  delay(300000); //5 minites
}
