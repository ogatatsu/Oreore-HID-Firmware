
#include "Arduino.h"
#include "HidEngine.h"
#include "PMW3360DM.h"
#include "UsbHid.h"

using namespace hidpg;

PMW3360DM pmw3360dm = PMW3360DM::create<0>(ThreadSafeSPI, A1, A0);

void motion_callback()
{
  HidEngine.mouseMove();
}

void read_mouse_delta_callback(int16_t *delta_x, int16_t *delta_y)
{
  pmw3360dm.readDelta(delta_x, delta_y);
}

void setup()
{
  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();

  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.start();

  pmw3360dm.setCallback(motion_callback);
  pmw3360dm.start();
  pmw3360dm.changeCpi(PMW3360DM::Cpi::_1000);

  vTaskStartScheduler();
}

void loop()
{
}
