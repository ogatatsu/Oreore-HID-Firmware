
#include "Arduino.h"
#include "Bluefruit_ConnectionController.h"
#include "DebounceIn.h"
#include "HidEngine.h"
#include "UsbHid.h"
#include "keymap.h"

using namespace hidpg;

constexpr const char *DEVICE_NAME = "TrackPoint Keyboard II Custom";

BLECentralProfileTrackpointKeyboard2 TrackPointKeyboard2;
BlinkLed ScanLed(PIN_LED1, LOW);
SemaphoreHandle_t callback_mutex;
int16_t delta_x_sum = 0;
int16_t delta_y_sum = 0;
int16_t wheel_sum = 0;
bool is_mouse_move_called = false;
bool is_rotate_encoder_called = false;
Set key6_ids, other_ids;

//------------------------------------------------------------------+
// TrackPoint Keyboard 2
//------------------------------------------------------------------+

void keyboard_report_callback(uint8_t modifiers, uint8_t key_codes[6])
{
  // 偶にkeycodeに全部1が入ったreportが送られてくる、邪魔なのでフィルタする
  if (key_codes[0] == 1 &&
      key_codes[1] == 1 &&
      key_codes[2] == 1 &&
      key_codes[3] == 1 &&
      key_codes[4] == 1 &&
      key_codes[5] == 1)
  {
    return;
  }

  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  key6_ids.clear();
  for (int i = 0; i < 6; i++)
  {
    if (key_codes[i] != 0)
    {
      key6_ids.add(key_codes[i]);
    }
  }
  other_ids.update(200, bitRead(modifiers, 0)); // left ctrl
  other_ids.update(201, bitRead(modifiers, 1)); // left shift
  other_ids.update(202, bitRead(modifiers, 2)); // left alt
  other_ids.update(203, bitRead(modifiers, 3)); // left gui
  other_ids.update(204, bitRead(modifiers, 4)); // right ctrl
  other_ids.update(205, bitRead(modifiers, 5)); // right shift
  other_ids.update(206, bitRead(modifiers, 6)); // right alt

  HidEngine.applyToKeymap(key6_ids | other_ids);
  xSemaphoreGive(callback_mutex);
}

void trackpoint_report_callback(uint8_t buttons, int8_t x, int8_t y, int8_t wheel)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  // mouse buttons
  bool is_update = false;
  is_update |= other_ids.update(0, bitRead(buttons, 0)); // left button
  is_update |= other_ids.update(1, bitRead(buttons, 1)); // right button
  is_update |= other_ids.update(2, bitRead(buttons, 2)); // middle button
  // トラックポイントを動かすたびにapplyToKeymapを呼び出すのを防ぐ
  if (is_update)
  {
    HidEngine.applyToKeymap(key6_ids | other_ids);
  }

  // mouse move
  if (x != 0 || y != 0)
  {
    delta_x_sum += x;
    delta_y_sum += y;
    if (is_mouse_move_called == false)
    {
      HidEngine.movePointer(PdTrackpoint);
      is_mouse_move_called = true;
    }
  }

  // mouse wheel
  if (wheel != 0)
  {
    EncoderAnd_A_IfNotRunning_B::notifyScroll();

    wheel_sum += wheel;
    if (is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(EncTrackpoint);
      is_rotate_encoder_called = true;
    }
  }
  xSemaphoreGive(callback_mutex);
}

void consumer_report_callback(uint16_t usage_code)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);

  other_ids.update(210, usage_code == 226); // F1(mute)
  other_ids.update(211, usage_code == 234); // F2(volume down)
  other_ids.update(212, usage_code == 233); // F3(volume up)
  other_ids.update(214, usage_code == 112); // F5(light down)
  other_ids.update(215, usage_code == 111); // F6(light up)

  HidEngine.applyToKeymap(key6_ids | other_ids);
  xSemaphoreGive(callback_mutex);
}

void vendor_report_callback(uint16_t usage_code)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);

  other_ids.update(213, usage_code == 187); // F4(mic off)
  other_ids.update(217, usage_code == 193); // F8(?)
  other_ids.update(218, usage_code == 188); // F9(?)
  other_ids.update(219, usage_code == 182); // F10(?)
  other_ids.update(220, usage_code == 439); // F11(?)
  other_ids.update(221, usage_code == 184); // F12(?)

  HidEngine.applyToKeymap(key6_ids | other_ids);
  xSemaphoreGive(callback_mutex);
}

//------------------------------------------------------------------+
// Dongle
//------------------------------------------------------------------+

void debounce_in_callback(uint8_t pin, bool state)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  if (pin == PIN_BUTTON1)
  {
    other_ids.update(255, !state);
    HidEngine.applyToKeymap(key6_ids | other_ids);
  }
  xSemaphoreGive(callback_mutex);
}

//------------------------------------------------------------------+
// HidEngine
//------------------------------------------------------------------+

void read_pointer_delta_callback(PointingDeviceId pointing_device_id, int16_t &delta_x, int16_t &delta_y)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  if (pointing_device_id == PdTrackpoint)
  {
    delta_x = round(delta_x_sum * TrackpointSpeedMagnification);
    delta_y = round(delta_y_sum * TrackpointSpeedMagnification);
    delta_x_sum = delta_y_sum = 0;
    is_mouse_move_called = false;
  }
  xSemaphoreGive(callback_mutex);
}

void read_encoder_step_callback(EncoderId encoder_id, int16_t &step)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  if (encoder_id == EncTrackpoint)
  {
    step = wheel_sum;
    wheel_sum = 0;
    is_rotate_encoder_called = false;
  }
  xSemaphoreGive(callback_mutex);
}

//------------------------------------------------------------------+
// Usb
//------------------------------------------------------------------+

void keyboard_led_callback(uint8_t leds_bitmap)
{
  TrackPointKeyboard2.Hid.setKeyboardLed(leds_bitmap);
}

void tud_mount_cb(void)
{
  ScanLed.resume();
}

void tud_umount_cb(void)
{
  ScanLed.suspend();
}

void tud_suspend_cb(bool remote_wakeup_en)
{
  ScanLed.suspend();
}

void tud_resume_cb(void)
{
  ScanLed.resume();
}

//------------------------------------------------------------------+
// main
//------------------------------------------------------------------+

void setup()
{
  static StaticSemaphore_t mutex_buffer;
  callback_mutex = xSemaphoreCreateMutexStatic(&mutex_buffer);

  EncoderAnd_A_IfNotRunning_B::begin();

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  Bluefruit.begin(0, 1);
  Bluefruit.setTxPower(8);
  Bluefruit.setName(DEVICE_NAME);

  // TrackPoint Keyboard II Host
  TrackPointKeyboard2.begin();
  TrackPointKeyboard2.Hid.setKeyboardReportCallback(keyboard_report_callback);
  TrackPointKeyboard2.Hid.setTrackpointReportCallback(trackpoint_report_callback);
  TrackPointKeyboard2.Hid.setConsumerReportCallback(consumer_report_callback);
  TrackPointKeyboard2.Hid.setVendorReportCallback(vendor_report_callback);

  // ConnectionController
  ScanLed.begin();
  Bluefruit_ConnectionController.begin();
  Bluefruit_ConnectionController.Central.setProfile(&TrackPointKeyboard2);
  Bluefruit_ConnectionController.Central.setScanLed(&ScanLed);
  Bluefruit_ConnectionController.Central.start();

  // Usb
  TinyUSBDevice.setProductDescriptor(DEVICE_NAME);
  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();
  hid_reporter->setKeyboardLedCallback(keyboard_led_callback);

  // Dongle Button
  DebounceIn.addPin(PIN_BUTTON1, INPUT_PULLUP, 10);
  DebounceIn.setCallback(debounce_in_callback);
  DebounceIn.start();

  // HidEngine
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setReadPointerDeltaCallback(read_pointer_delta_callback);
  HidEngine.setReadEncoderStepCallback(read_encoder_step_callback);
  HidEngine.setKeymap(keymap);
  HidEngine.setKeyShiftMap(keyShiftMap);
  HidEngine.setGestureMap(gestureMap);
  HidEngine.setEncoderMap(encoderMap);
  HidEngine.setEncoderShiftMap(encoderShiftMap);
  HidEngine.setComboMap(comboMap);
  HidEngine.start();
}

void loop()
{
  suspendLoop();
}
