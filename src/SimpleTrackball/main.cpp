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
#include "BleController.h"
#include "HidEngine.h"
#include "PMW3360.h"

#define PMW3360_NCS_PIN 25
#define PMW3360_INTERRUPT_PIN 23

using namespace hidpg;

PMW3360 pmw3360 = PMW3360::create<0>(ThreadSafeSPI, PMW3360_NCS_PIN, PMW3360_INTERRUPT_PIN);

void prph_cannot_connect_callback()
{
  pmw3360.stopTask_and_setWakeUpInterrupt();
  sd_power_system_off();
}

void motion_callback(int16_t deltaX, int16_t deltaY)
{
  // トラックボールはセンサーを逆向きに取り付けるのでdeltaXを-にする
  HidEngine::mouseMove(-deltaX, deltaY);
}

void setup()
{
  // シリアルをオンにすると消費電流が増えるのでデバッグ時以外はオフにする
  // Serial.begin(115200);

  BleController::setPrphCannnotConnectCallback(prph_cannot_connect_callback);
  BleController::init();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  BleController::startPrphConnection();

  pmw3360.setCallback(motion_callback);
  pmw3360.init();
  pmw3360.startTask();

  HidEngine::setHidReporter(BleController::getHidReporter());
  HidEngine::init();
  HidEngine::startTask();
}

void loop()
{
  BleController::setBatteryLevel(BatteryUtil::readBatteryLevel());
  delay(300000); //5 minites
}
