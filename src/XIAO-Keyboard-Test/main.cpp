
#include "Arduino.h"
#include "HidEngine.h"
#include "UsbHid.h"
#include "keymap.h"
#include "matrix.h"

using namespace hidpg;


void matrix_scan_callback(const Set &ids)
{
  HidEngine.applyToKeymap(ids);
}

void setup()
{
  MatrixScan.setCallback(matrix_scan_callback);
  MatrixScan.setMatrix(matrix, out_pins, in_pins);
  MatrixScan.begin();

  UsbHid.begin();

  HidEngine.setHidReporter(UsbHid.getHidReporter());
  HidEngine.setKeymap(keymap);
  HidEngine.begin();

  vTaskStartScheduler();
}

void loop()
{
}
