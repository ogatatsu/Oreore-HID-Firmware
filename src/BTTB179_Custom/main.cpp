
#include "Arduino.h"
#include "BTTB179BleHost.h"
#include "HidEngine.h"
#include "Set.h"
#include "UsbHid.h"
#include "bluefruit.h"
#include "keymap.h"

using namespace hidpg;

BTTB179BleHost bttb179;
SemaphoreHandle_t mov_mutex;
SemaphoreHandle_t whl_mutex;
int16_t delta_x_sum = 0;
int16_t delta_y_sum = 0;
int16_t wheel_sum = 0;
int16_t pan_sum = 0;
bool is_mouse_move_called = false;
bool is_rotate_encoder_called = false;
Set ids;

void scan_callback(ble_gap_evt_adv_report_t *report)
{
  uint8_t buffer[32] = {};

  if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    if (strcmp((char *)buffer, "SANWA BTTB179BK") == 0)
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

  BLEConnection *conn = Bluefruit.Connection(conn_handle);

  if (bttb179.discover(conn_handle))
  {
    Serial.println("Found it");

    // HID device mostly require pairing/bonding
    conn->requestPairing();

    Serial.println("Ready to receive from peripheral");
  }
  else
  {
    Serial.println("Found NONE");

    // disconnect since we couldn't find blehid service
    conn->disconnect();
  }
}

void connection_secured_callback(uint16_t conn_handle)
{
  BLEConnection *conn = Bluefruit.Connection(conn_handle);

  if (!conn->secured())
  {
    // It is possible that connection is still not secured by this time.
    // This happens (central only) when we try to encrypt connection using stored bond keys
    // but peer reject it (probably it remove its stored key).
    // Therefore we will request an pairing again --> callback again when encrypted
    conn->requestPairing();
  }
  else
  {
    Serial.println("Secured");

    bttb179.enableTrackball();

    Serial.println("Ready to receive from peripheral");
  }
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void)conn_handle;
  (void)reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
}

void trackball_report_callback(bttb179_trackball_report_t *report)
{
  // Serial.printf("[Trackball] buttons = %2d, x = %4d, y = %4d, wheel = %2d, pan = %2d\n", report->buttons, report->x, report->y, report->wheel, report->pan);

  // buttons
  ids.update(0, bitRead(report->buttons, 0)); // left button
  ids.update(1, bitRead(report->buttons, 1)); // right button
  ids.update(2, bitRead(report->buttons, 2)); // middle button
  ids.update(3, bitRead(report->buttons, 3)); // backward button
  ids.update(4, bitRead(report->buttons, 4)); // forward button
  HidEngine.applyToKeymap(ids);

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
    xSemaphoreTake(whl_mutex, portMAX_DELAY);
    wheel_sum += report->wheel;
    if (is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(0);
      is_rotate_encoder_called = true;
    }
    xSemaphoreGive(whl_mutex);
  }

  // pan
  if (report->pan != 0)
  {
    xSemaphoreTake(whl_mutex, portMAX_DELAY);
    pan_sum += report->pan;
    if (is_rotate_encoder_called == false)
    {
      HidEngine.rotateEncoder(1);
      is_rotate_encoder_called = true;
    }
    xSemaphoreGive(whl_mutex);
  }
}

void read_mouse_delta_callback(int16_t &delta_x, int16_t &delta_y)
{
  xSemaphoreTake(mov_mutex, portMAX_DELAY);
  delta_x = delta_x_sum;
  delta_y = delta_y_sum;
  delta_x_sum = delta_y_sum = 0;
  is_mouse_move_called = false;
  xSemaphoreGive(mov_mutex);
}

void read_encoder_step_callback(uint8_t encoder_id, int32_t &step)
{
  xSemaphoreTake(whl_mutex, portMAX_DELAY);
  if (encoder_id == 0)
  {
    step = wheel_sum;
    wheel_sum = 0;
  }
  else if (encoder_id == 1)
  {
    step = pan_sum;
    pan_sum = 0;
  }
  is_rotate_encoder_called = false;
  xSemaphoreGive(whl_mutex);
}

void setup()
{
  Serial.begin(115200);

  mov_mutex = xSemaphoreCreateMutex();
  whl_mutex = xSemaphoreCreateMutex();

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  Bluefruit.begin(0, 1);
  Bluefruit.setName("BTTB179 Custom");
  Bluefruit.autoConnLed(false);

  // Init BTTB179 Host
  bttb179.begin();
  bttb179.setTrackballReportCallback(trackball_report_callback);

  // Init Usb HID
  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();

  // Init HidEngine
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.setReadEncoderStepCallback(read_encoder_step_callback);
  HidEngine.setKeymap(keymap);
  HidEngine.setEncoderMap(encoderMap);
  HidEngine.setTrackMap(trackMap);
  HidEngine.start();

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  Bluefruit.Security.setSecuredCallback(connection_secured_callback);

  // Start Central Scanning
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);   // in unit of 0.625 ms
  Bluefruit.Scanner.filterService(bttb179); // only report HID service
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds

  vTaskDelete(xTaskGetCurrentTaskHandle());
  // suspendLoop();
}

void loop()
{
  // not used
}
