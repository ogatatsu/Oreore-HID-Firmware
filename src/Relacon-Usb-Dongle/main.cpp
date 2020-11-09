
#include "HidEngine.h"
#include "RelaconBleHost.h"
#include "UsbHid.h"
#include <bluefruit.h>

using namespace hidpg;

RelaconBleHost relacon;
SemaphoreHandle_t mutex;
int16_t delta_x_sum = 0;
int16_t delta_y_sum = 0;
bool is_mouse_move_called = false;

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

  if (relacon.discover(conn_handle))
  {
    Serial.println("Found it");

    // HID device mostly require pairing/bonding
    if (!Bluefruit.requestPairing(conn_handle))
    {
      Serial.print("Failed to paired");
      return;
    }

    relacon.enableTrackball();
    relacon.enableConsumer();

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

void trackball_report_callback(relacon_trackball_report_t *report)
{
  // Serial.printf("[Trackball] buttons = %2d, x = %4d, y = %4d, wheel = %2d\n", report->buttons, report->x, report->y, report->wheel);
  xSemaphoreTake(mutex, portMAX_DELAY);
  delta_x_sum += report->x;
  delta_y_sum += report->y;
  if (is_mouse_move_called == false)
  {
    HidEngine.mouseMove();
    is_mouse_move_called = true;
  }
  xSemaphoreGive(mutex);
}

void consumer_report_callback(relacon_consumer_report_t *report)
{
  Serial.printf("[Consumer] usage_code = %d\n", report->usage_code);
}

void read_mouse_delta_callback(int16_t *delta_x, int16_t *delta_y)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
  *delta_x = delta_x_sum;
  *delta_y = delta_y_sum;
  delta_x_sum = delta_y_sum = 0;
  is_mouse_move_called = false;
  xSemaphoreGive(mutex);
}

void setup()
{
  mutex = xSemaphoreCreateMutex();

  Serial.begin(115200);
  while (!Serial)
    delay(10); // for nrf52840 with native usb

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);

  Bluefruit.setName("Bluefruit52 Central");

  // Init Realacon Host
  relacon.begin();
  relacon.setTrackballReportCallback(trackball_report_callback);
  relacon.setConsumerReportCallback(consumer_report_callback);

  UsbHid.begin();

  HidEngine.setHidReporter(UsbHid.getHidReporter());
  HidEngine.setReadMouseDeltaCallback(read_mouse_delta_callback);
  HidEngine.begin();

  // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.setConnLedInterval(250);

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Filter only accept HID service in advertising
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);   // in unit of 0.625 ms
  Bluefruit.Scanner.filterService(relacon); // only report HID service
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds
}

void loop()
{
  delay(1000);
}
