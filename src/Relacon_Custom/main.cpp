
#include "Arduino.h"
#include "BleClientRelacon.h"
#include "HidEngine.h"
#include "Set.h"
#include "UsbHid.h"
#include "bluefruit.h"
#include "keymap.h"

using namespace hidpg;

BleClientRelacon relacon;
SemaphoreHandle_t mov_mutex;
SemaphoreHandle_t whl_mutex;
int16_t delta_x_sum = 0;
int16_t delta_y_sum = 0;
int16_t wheel_sum = 0;
bool is_mouse_move_called = false;
bool is_rotate_encoder_called = false;
Set ids;

void scan_callback(ble_gap_evt_adv_report_t *report)
{
  uint8_t buffer[32] = {};

  if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    if (strcmp((char *)buffer, "ELECOM Relacon") == 0)
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

  if (relacon.discover(conn_handle))
  {
    Serial.println("Found it");

    // HID device mostly require pairing/bonding
    conn->requestPairing();
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

    relacon.enableTrackball();
    relacon.enableConsumer();

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

void trackball_report_callback(relacon_trackball_report_t *report)
{
  // Serial.printf("[Trackball] buttons = %2d, x = %4d, y = %4d, wheel = %2d\n", report->buttons, report->x, report->y, report->wheel);

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
      HidEngine.mouseMove(MOUSE_ID);
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
}

void consumer_report_callback(relacon_consumer_report_t *report)
{
  // Serial.printf("[Consumer] usage_code = %d\n", report->usage_code);

  ids.update(10, (report->usage_code == 181) ? true : false); // next
  ids.update(11, (report->usage_code == 182) ? true : false); // prev
  ids.update(12, (report->usage_code == 205) ? true : false); // play pause
  ids.update(13, (report->usage_code == 233) ? true : false); // volume up
  ids.update(14, (report->usage_code == 234) ? true : false); // volume down
  HidEngine.applyToKeymap(ids);
}

void read_mouse_delta_callback(uint8_t mouse_id, int16_t &delta_x, int16_t &delta_y)
{
  if (mouse_id == MOUSE_ID)
  {
    xSemaphoreTake(mov_mutex, portMAX_DELAY);
    delta_x = delta_x_sum;
    delta_y = delta_y_sum;
    delta_x_sum = delta_y_sum = 0;
    is_mouse_move_called = false;
    xSemaphoreGive(mov_mutex);
  }
}

void read_encoder_step_callback(uint8_t encoder_id, int32_t &step)
{
  xSemaphoreTake(whl_mutex, portMAX_DELAY);
  step = wheel_sum;
  wheel_sum = 0;
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
  Bluefruit.setName("Relacon Custom");
  Bluefruit.autoConnLed(false);

  // Init Realacon Host
  relacon.begin();
  relacon.setTrackballReportCallback(trackball_report_callback);
  relacon.setConsumerReportCallback(consumer_report_callback);

  // Init Usb HID
  UsbHid.begin();
  HidReporter *hid_reporter = UsbHid.getHidReporter();

  // Init HidEngine
  HidEngine.setHidReporter(hid_reporter);
  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.setReadEncoderStepCallback(read_encoder_step_callback);
  HidEngine.setKeymap(keymap);
  HidEngine.setEncoderMap(encoderMap);
  HidEngine.setGestureMap(gestureMap);
  HidEngine.start();

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  Bluefruit.Security.setSecuredCallback(connection_secured_callback);

  // Start Central Scanning
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);   // in unit of 0.625 ms
  Bluefruit.Scanner.filterService(relacon); // only report HID service
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds

  vTaskDelete(xTaskGetCurrentTaskHandle());
  // suspendLoop();
}

void loop()
{
  // not used
}
