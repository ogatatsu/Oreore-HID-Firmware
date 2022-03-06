
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
  MatrixScan.start();

  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();

  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setKeymap(keymap);
  HidEngine.start();
}

void loop()
{
}
