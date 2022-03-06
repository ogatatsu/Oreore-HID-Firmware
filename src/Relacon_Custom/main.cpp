
#include "Arduino.h"
#include "Bluefruit_ConnectionController.h"
#include "DebounceIn.h"
#include "HidEngine.h"
#include "UsbHid.h"
#include "keymap.h"

using namespace hidpg;

constexpr const char *DEVICE_NAME = "Relacon Custom";

BLECentralProfileRelacon Relacon;
BlinkLed ScanLed(PIN_LED1, LOW);
SemaphoreHandle_t callback_mutex;
int16_t delta_x_sum = 0;
int16_t delta_y_sum = 0;
int16_t wheel_sum = 0;
bool is_mouse_move_called = false;
bool is_rotate_encoder_called = false;
Set ids;

//------------------------------------------------------------------+
// tinyusb
//------------------------------------------------------------------+

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
// Trackball
//------------------------------------------------------------------+

void trackball_report_callback(uint8_t buttons, int16_t x, int16_t y, int8_t wheel)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  // buttons
  bool is_update = false;
  is_update |= ids.update(0, bitRead(buttons, 0)); // left button
  is_update |= ids.update(1, bitRead(buttons, 1)); // right button
  is_update |= ids.update(2, bitRead(buttons, 2)); // middle button
  is_update |= ids.update(3, bitRead(buttons, 3)); // backward button
  is_update |= ids.update(4, bitRead(buttons, 4)); // forward button
  if (is_update)
  {
    HidEngine.applyToKeymap(ids);
  }

  // mouse move
  if (x != 0 || y != 0)
  {
    delta_x_sum += x;
    delta_y_sum += y;
    if (is_mouse_move_called == false)
    {
      HidEngine.movePointer(PdTrackball);
      is_mouse_move_called = true;
    }
  }

  // wheel
  if (wheel != 0)
  {
    wheel_sum += wheel;
    if (is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(EncWheel);
      is_rotate_encoder_called = true;
    }
  }
  xSemaphoreGive(callback_mutex);
}

void consumer_report_callback(uint16_t usage_code)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  ids.update(10, usage_code == 181); // next
  ids.update(11, usage_code == 182); // prev
  ids.update(12, usage_code == 205); // play pause
  ids.update(13, usage_code == 233); // volume up
  ids.update(14, usage_code == 234); // volume down
  HidEngine.applyToKeymap(ids);
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
    ids.update(255, !state);
    HidEngine.applyToKeymap(ids);
  }
  xSemaphoreGive(callback_mutex);
}

//------------------------------------------------------------------+
// HidEngine
//------------------------------------------------------------------+

void read_pointer_delta_callback(PointingDeviceId pointing_device_id, int16_t &delta_x, int16_t &delta_y)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  if (pointing_device_id == PdTrackball)
  {
    delta_x = delta_x_sum;
    delta_y = delta_y_sum;
    delta_x_sum = delta_y_sum = 0;
    is_mouse_move_called = false;
  }
  xSemaphoreGive(callback_mutex);
}

void read_encoder_step_callback(EncoderId encoder_id, int16_t &step)
{
  xSemaphoreTake(callback_mutex, portMAX_DELAY);
  if (encoder_id == EncWheel)
  {
    step = wheel_sum;
    wheel_sum = 0;
    is_rotate_encoder_called = false;
  }
  xSemaphoreGive(callback_mutex);
}

//------------------------------------------------------------------+
// main
//------------------------------------------------------------------+

void setup()
{
  static StaticSemaphore_t mutex_buffer;
  callback_mutex = xSemaphoreCreateMutexStatic(&mutex_buffer);

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  Bluefruit.begin(0, 1);
  Bluefruit.setTxPower(8);
  Bluefruit.setName(DEVICE_NAME);

  // Relacon Host
  Relacon.begin();
  Relacon.Hid.setTrackballReportCallback(trackball_report_callback);
  Relacon.Hid.setConsumerReportCallback(consumer_report_callback);

  // ConnectionController
  ScanLed.begin();
  Bluefruit_ConnectionController.begin();
  Bluefruit_ConnectionController.Central.setProfile(&Relacon);
  Bluefruit_ConnectionController.Central.setScanLed(&ScanLed);
  Bluefruit_ConnectionController.Central.start();

  // Usb
  TinyUSBDevice.setProductDescriptor(DEVICE_NAME);
  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();

  // Dongle Button
  DebounceIn.addPin(PIN_BUTTON1, INPUT_PULLUP, 10);
  DebounceIn.setCallback(debounce_in_callback);
  DebounceIn.start();

  // HidEngine
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setReadPointerDeltaCallback(read_pointer_delta_callback);
  HidEngine.setReadEncoderStepCallback(read_encoder_step_callback);
  HidEngine.setKeymap(keymap);
  HidEngine.setEncoderMap(encoderMap);
  HidEngine.setGestureMap(gestureMap);
  // HidEngine.setComboMap(comboMap);
  HidEngine.start();
}

void loop()
{
  suspendLoop();
}
