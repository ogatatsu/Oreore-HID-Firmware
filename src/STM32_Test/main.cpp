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

#include "PMW3360.h"
#include "STM32Hid.h"
#include "keymap.h"
#include "matrix.h"

#define PMW3360_NCS_PIN PB13
#define PMW3360_INTERRUPT_PIN PB12

PMW3360 pmw3360 = PMW3360::create<0>(ThreadSafeSPI, PMW3360_NCS_PIN, PMW3360_INTERRUPT_PIN);

void keyscan_callback(const Set &ids)
{
  HidEngine::applyToKeymap(ids);
}

void motion_callback(int8_t deltaX, int8_t deltaY, uint8_t id)
{
  // トラックボールはセンサーを逆向きに取り付けるのでdeltaXを-にする
  HidEngine::mouseMove(-deltaX, deltaY);
}

void setup()
{
  MatrixScan::setKeyscanCallback(keyscan_callback);
  MatrixScan::init(matrix, outPins, inPins);
  MatrixScan::startTask();

  pmw3360.setMotionCallback(motion_callback);
  pmw3360.startTask();

  STM32Hid::init();

  HidEngine::setKeymap(keymap);
  //HidEngine::setSimulKeymap(simulKeymap);
  //HidEngine::setSeqKeymap(seqKeymap);
  HidEngine::init(STM32Hid::getHidReporter());
  HidEngine::startTask();

  vTaskStartScheduler();

  while (1)
    ;
}

void loop()
{
  // Not used.
}
