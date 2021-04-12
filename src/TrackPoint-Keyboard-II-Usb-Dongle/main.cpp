
#include "Arduino.h"
#include "HidEngine.h"
#include "Set.h"
#include "TPKBD2BleHost.h"
#include "UsbHid.h"
#include "bluefruit.h"
#include "keymap.h"

using namespace hidpg;

TPKBD2BleHost tpkbd2;
SemaphoreHandle_t mov_mutex;
SemaphoreHandle_t whl_mutex;
int16_t delta_x_sum = 0;
int16_t delta_y_sum = 0;
int16_t wheel_sum = 0;
bool is_mouse_move_called = false;
bool is_rotate_encoder_called = false;
Set kbd_ids, btn_ids;

void scan_callback(ble_gap_evt_adv_report_t *report)
{
  uint8_t buffer[32] = {};

  if (report->type.scan_response && Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    if (strcmp((char *)buffer, "TrackPoint Keyboard II") == 0)
    {
      Bluefruit.Central.connect(report);
      return;
    }
  }

  Bluefruit.Scanner.resume();
}

void connect_callback(uint16_t conn_handle)
{
  Serial.println("Connected");
  Serial.print("Discovering HID  Service ... ");

  if (tpkbd2.discover(conn_handle))
  {
    Serial.println("Found it");

    // HID device mostly require pairing/bonding
    if (!Bluefruit.requestPairing(conn_handle))
    {
      Serial.print("Failed to paired");
      return;
    }

    tpkbd2.enableKeyboard();
    tpkbd2.enableTrackpoint();
    tpkbd2.enableConsumer();

    Serial.println("Ready to receive from peripheral");
  }
  else
  {
    Serial.println("Found NONE");

    // disconnect since we couldn't find blehid service
    Bluefruit.disconnect(conn_handle);
  }
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void)conn_handle;
  (void)reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
}

void keyboard_report_callback(tpkbd2_keyboard_report_t *report)
{
  // Serial.print("[keyboard] keycode = ");
  // Serial.printBuffer(report->keycode, 6);
  // Serial.print(" : modifier = ");
  // Serial.println(report->modifier, BIN);

  // 偶にkeycodeに全部1が入ったreportが送られてくる、邪魔なのでフィルタする
  if (report->keycode[0] == 1 &&
      report->keycode[1] == 1 &&
      report->keycode[2] == 1 &&
      report->keycode[3] == 1 &&
      report->keycode[4] == 1 &&
      report->keycode[5] == 1)
  {
    return;
  }

  kbd_ids.clear();
  for (int i = 0; i < 6; i++)
  {
    if (report->keycode[i] != 0)
      kbd_ids.add(report->keycode[i]);
  }
  kbd_ids.update(200, bitRead(report->modifier, 0)); // left ctrl
  kbd_ids.update(201, bitRead(report->modifier, 1)); // left shift
  kbd_ids.update(202, bitRead(report->modifier, 2)); // left alt
  kbd_ids.update(203, bitRead(report->modifier, 3)); // left gui
  kbd_ids.update(204, bitRead(report->modifier, 4)); // right ctrl
  kbd_ids.update(205, bitRead(report->modifier, 5)); // right shift
  kbd_ids.update(206, bitRead(report->modifier, 6)); // right alt
  // このキーボードに right gui は付いて無い

  HidEngine.applyToKeymap(kbd_ids | btn_ids);
}

void trackpoint_report_callback(tpkbd2_trackpoint_report_t *report)
{
  // Serial.printf("[Trackpoint] buttons = %2d, x = %4d, y = %4d, wheel = %2d\n", report->buttons, report->x, report->y, report->wheel);

  // button
  btn_ids.update(0, bitRead(report->buttons, 0)); // left button
  btn_ids.update(1, bitRead(report->buttons, 1)); // right button
  btn_ids.update(2, bitRead(report->buttons, 2)); // middle button

  HidEngine.applyToKeymap(kbd_ids | btn_ids);

  // mouse move
  if (report->x != 0 || report->y != 0)
  {
    xSemaphoreTake(mov_mutex, portMAX_DELAY);
    delta_x_sum += report->x;
    delta_y_sum += report->y;
    if (is_mouse_move_called == false)
    {
      HidEngine.mouseMove();
      is_mouse_move_called = true;
    }
    xSemaphoreGive(mov_mutex);
  }

  // wheel
  if (report->wheel != 0)
  {
    ScrollOrTap::notifyScroll();

    xSemaphoreTake(whl_mutex, portMAX_DELAY);
    wheel_sum += report->wheel;
    if (is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(0);
      is_rotate_encoder_called = true;
    }
    xSemaphoreGive(whl_mutex);
  }
}

void consumer_report_callback(tpkbd2_consumer_report_t *report)
{
  Serial.printf("[Consumer] usage_code = %d\n", report->usage_code);

  // ids.update(210, (report->usage_code == 226) ? true : false); // mute
  // ids.update(211, (report->usage_code == 234) ? true : false); // volume down
  // ids.update(212, (report->usage_code == 233) ? true : false); // volume up
  // ids.update(213, (report->usage_code == 226) ? true : false); //
  // ids.update(214, (report->usage_code == 112) ? true : false); // light down
  // ids.update(215, (report->usage_code == 111) ? true : false); // light up
  // ids.update(216, (report->usage_code == 402) ? true : false); // ?

  // HidEngine.applyToKeymap(ids);
}

void read_mouse_delta_callback(int16_t *delta_x, int16_t *delta_y)
{
  xSemaphoreTake(mov_mutex, portMAX_DELAY);
  *delta_x = delta_x_sum;
  *delta_y = delta_y_sum;
  delta_x_sum = delta_y_sum = 0;
  is_mouse_move_called = false;
  xSemaphoreGive(mov_mutex);
}

void read_encoder_step_callback(uint8_t encoder_id, int32_t *step)
{
  xSemaphoreTake(whl_mutex, portMAX_DELAY);
  if (encoder_id == 0)
  {
    *step = wheel_sum;
    wheel_sum = 0;
  }
  is_rotate_encoder_called = false;
  xSemaphoreGive(whl_mutex);
}

void setup()
{
  Serial.begin(115200);

  mov_mutex = xSemaphoreCreateMutex();
  whl_mutex = xSemaphoreCreateMutex();

  ScrollOrTap::init();

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  Bluefruit.begin(0, 1);
  Bluefruit.setName("TrackPoint-Keyboard-II-Usb-Dongle");

  // Init TrackPoint Keyboard II Host
  tpkbd2.begin();
  tpkbd2.setKeyboardReportCallback(keyboard_report_callback);
  tpkbd2.setTrackpointReportCallback(trackpoint_report_callback);
  tpkbd2.setConsumerReportCallback(consumer_report_callback);

  // Init Usb HID
  UsbHid.begin();

  // Init HidEngine
  HidEngine.setHidReporter(UsbHid.getHidReporter());
  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.setReadEncoderStepCallback(read_encoder_step_callback);
  HidEngine.setKeymap(keymap);
  HidEngine.setEncoderMap(encoderMap);
  HidEngine.begin();

  // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.setConnLedInterval(250);

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);

  // Start Central Scanning
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(true);
  Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds

  suspendLoop();
}

void loop()
{
}
