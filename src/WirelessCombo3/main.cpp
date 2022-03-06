
#include "Arduino.h"
#include "Bluefruit_ConnectionController.h"
#include "DebounceIn.h"
#include "HidEngine.h"
#include "UsbHid.h"
#include "keymap.h"

using namespace hidpg;

constexpr const char *DEVICE_NAME = "WirelessCombo3";

BLECentralProfileLift Lift;
BLECentralProfileTrackpointKeyboard2 TrackPointKeyboard2;
BLECentralProfileRelacon Relacon;

BlinkLed ScanLed(PIN_LED1, LOW);
SemaphoreHandle_t callback_mutex;

struct
{
  int16_t delta_x_sum = 0;
  int16_t delta_y_sum = 0;
  bool is_move_pointer_called = false;

  int16_t wheel_sum = 0;
  bool is_rotate_encoder_called = false;
} TrackPointData, RelaconData;

struct
{
  int16_t delta_x_sum = 0;
  int16_t delta_y_sum = 0;
  bool is_move_pointer_called = false;

  int16_t wheel_sum = 0;
  bool is_wheel_rotate_encoder_called = false;

  int16_t pan_sum = 0;
  bool is_pan_rotate_encoder_called = false;
} LiftData;

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
    TrackPointData.delta_x_sum += x;
    TrackPointData.delta_y_sum += y;
    if (TrackPointData.is_move_pointer_called == false)
    {
      HidEngine.movePointer(PdTrackpoint);
      TrackPointData.is_move_pointer_called = true;
    }
  }

  // mouse wheel
  if (wheel != 0)
  {
    EncoderAnd_A_IfNotRunning_B::notifyScroll();

    TrackPointData.wheel_sum += wheel;
    if (TrackPointData.is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(EncTrackpoint);
      TrackPointData.is_rotate_encoder_called = true;
    }
  }
  xSemaphoreGive(callback_mutex);
}

void tpkbd2_consumer_report_callback(uint16_t usage_code)
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
// Lift
//------------------------------------------------------------------+

void mouse_report_callback(uint8_t buttons, int16_t x, int16_t y, int8_t wheel, int8_t pan)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  // mouse buttons
  bool is_update = false;
  is_update |= other_ids.update(231, bitRead(buttons, 0)); // left button
  is_update |= other_ids.update(232, bitRead(buttons, 1)); // right button
  is_update |= other_ids.update(233, bitRead(buttons, 2)); // middle button
  is_update |= other_ids.update(234, bitRead(buttons, 3)); // middle button
  is_update |= other_ids.update(235, bitRead(buttons, 4)); // middle button
  // マウスを動かすたびにapplyToKeymapを呼び出すのを防ぐ
  if (is_update)
  {
    HidEngine.applyToKeymap(key6_ids | other_ids);
  }

  // mouse move
  if (x != 0 || y != 0)
  {
    LiftData.delta_x_sum += x;
    LiftData.delta_y_sum += y;
    if (LiftData.is_move_pointer_called == false)
    {
      HidEngine.movePointer(PdLift);
      LiftData.is_move_pointer_called = true;
    }
  }

  // wheel
  if (wheel != 0)
  {
    LiftData.wheel_sum += wheel;
    if (LiftData.is_wheel_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(EncLiftWheel);
      LiftData.is_wheel_rotate_encoder_called = true;
    }
  }

  // pan
  if (pan != 0)
  {
    LiftData.pan_sum += pan;
    if (LiftData.is_pan_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(EncLiftPan);
      LiftData.is_pan_rotate_encoder_called = true;
    }
  }

  xSemaphoreGive(callback_mutex);
}

//------------------------------------------------------------------+
// Relacon
//------------------------------------------------------------------+

void trackball_report_callback(uint8_t buttons, int16_t x, int16_t y, int8_t wheel)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  // buttons
  bool is_update = false;
  is_update |= other_ids.update(241, bitRead(buttons, 0)); // left button
  is_update |= other_ids.update(242, bitRead(buttons, 1)); // right button
  is_update |= other_ids.update(243, bitRead(buttons, 2)); // middle button
  is_update |= other_ids.update(244, bitRead(buttons, 3)); // backward button
  is_update |= other_ids.update(245, bitRead(buttons, 4)); // forward button
  if (is_update)
  {
    HidEngine.applyToKeymap(key6_ids | other_ids);
  }

  // mouse move
  if (x != 0 || y != 0)
  {
    RelaconData.delta_x_sum += x;
    RelaconData.delta_y_sum += y;
    if (RelaconData.is_move_pointer_called == false)
    {
      HidEngine.movePointer(PdRelacon);
      RelaconData.is_move_pointer_called = true;
    }
  }

  // wheel
  if (wheel != 0)
  {
    RelaconData.wheel_sum += wheel;
    if (RelaconData.is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(EncRelaconWheel);
      RelaconData.is_rotate_encoder_called = true;
    }
  }
  xSemaphoreGive(callback_mutex);
}

void relacon_consumer_report_callback(uint16_t usage_code)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  other_ids.update(246, usage_code == 181); // next
  other_ids.update(247, usage_code == 182); // prev
  other_ids.update(248, usage_code == 205); // play pause
  other_ids.update(249, usage_code == 233); // volume up
  other_ids.update(250, usage_code == 234); // volume down
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
    delta_x = round(TrackPointData.delta_x_sum * TrackpointSpeedMagnification);
    delta_y = round(TrackPointData.delta_y_sum * TrackpointSpeedMagnification);
    TrackPointData.delta_x_sum = TrackPointData.delta_y_sum = 0;
    TrackPointData.is_move_pointer_called = false;
  }
  else if (pointing_device_id == PdLift)
  {
    delta_x = LiftData.delta_x_sum;
    delta_y = LiftData.delta_y_sum;
    LiftData.delta_x_sum = LiftData.delta_y_sum = 0;
    LiftData.is_move_pointer_called = false;
  }
  else if (pointing_device_id == PdRelacon)
  {
    delta_x = RelaconData.delta_x_sum;
    delta_y = RelaconData.delta_y_sum;
    RelaconData.delta_x_sum = RelaconData.delta_y_sum = 0;
    RelaconData.is_move_pointer_called = false;
  }
  xSemaphoreGive(callback_mutex);
}

void read_encoder_step_callback(EncoderId encoder_id, int16_t &step)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  if (encoder_id == EncTrackpoint)
  {
    step = TrackPointData.wheel_sum;
    TrackPointData.wheel_sum = 0;
    TrackPointData.is_rotate_encoder_called = false;
  }
  else if (encoder_id == EncLiftWheel)
  {
    step = LiftData.wheel_sum;
    LiftData.wheel_sum = 0;
    LiftData.is_wheel_rotate_encoder_called = false;
  }
  else if (encoder_id == EncLiftPan)
  {
    step = LiftData.pan_sum;
    LiftData.pan_sum = 0;
    LiftData.is_pan_rotate_encoder_called = false;
  }
  if (encoder_id == EncRelaconWheel)
  {
    step = RelaconData.wheel_sum;
    RelaconData.wheel_sum = 0;
    RelaconData.is_rotate_encoder_called = false;
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

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 2
  Bluefruit.begin(0, 3);
  Bluefruit.setTxPower(8);
  Bluefruit.setName(DEVICE_NAME);

  // Lift Host
  Lift.begin();
  Lift.Hid.setMouseReportCallback(mouse_report_callback);

  // Relacon Host
  Relacon.begin();
  Relacon.Hid.setTrackballReportCallback(trackball_report_callback);
  Relacon.Hid.setConsumerReportCallback(relacon_consumer_report_callback);

  // TrackPoint Keyboard II Host
  TrackPointKeyboard2.begin();
  TrackPointKeyboard2.Hid.setKeyboardReportCallback(keyboard_report_callback);
  TrackPointKeyboard2.Hid.setTrackpointReportCallback(trackpoint_report_callback);
  TrackPointKeyboard2.Hid.setConsumerReportCallback(tpkbd2_consumer_report_callback);
  TrackPointKeyboard2.Hid.setVendorReportCallback(vendor_report_callback);

  // ConnectionController
  ScanLed.begin();
  Bluefruit_ConnectionController.begin();
  Bluefruit_ConnectionController.Central.setProfile(&Lift, &TrackPointKeyboard2, &Relacon);
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
