
#include "Arduino.h"
#include "CommandDsl.h"
#include "DebounceIn.h"
#include "HidEngine.h"
#include "KC.h"
#include "PMW3360DM.h"
#include "UsbHid.h"

using namespace hidpg;

constexpr uint8_t MOUSE_ID = 0;

PMW3360DM pmw3360dm = PMW3360DM::create<MOUSE_ID>(ThreadSafeSPI, A1, A0);

static Key keymap[] = {
  { 0, MS_CLK(LeftButton) },
  { 7, MS_CLK(RightButton) },
};

void motion_callback()
{
  HidEngine.movePointer(MOUSE_ID);
}

void read_mouse_delta_callback(uint8_t mouse_id, int16_t &delta_x, int16_t &delta_y)
{
  if (mouse_id == MOUSE_ID)
  {
    pmw3360dm.readDelta(&delta_x, &delta_y);
  }
}

void debounce_in_callback(uint8_t pin, bool state)
{
  static Set key_ids;
  key_ids.update(pin, !state);
  HidEngine.applyToKeymap(key_ids);
}

void setup()
{
  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();

  DebounceIn.addPin(0, INPUT_PULLUP);
  DebounceIn.addPin(7, INPUT_PULLUP);
  DebounceIn.setCallback(debounce_in_callback);
  DebounceIn.start();

  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setKeymap(keymap);
  HidEngine.start();

  pmw3360dm.setCallback(motion_callback);
  pmw3360dm.start();
  pmw3360dm.changeCpi(PMW3360DM::Cpi::_1000);
}

void loop()
{
}
