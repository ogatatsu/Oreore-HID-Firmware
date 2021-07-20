/*
  The MIT License (MIT)

  Copyright (c) 2020 ogatatsu.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "TPKBD2BleHost.h"
#include "bluefruit.h"

TPKBD2BleHost::TPKBD2BleHost(void)
    : BLEClientService(UUID16_SVC_HUMAN_INTERFACE_DEVICE),
      _keyboard_cb(nullptr),
      _trackpoint_cb(nullptr),
      _consumer_cb(nullptr),
      _keyboard_input(UUID16_CHR_REPORT),
      _trackpoint_input(UUID16_CHR_REPORT),
      _consumer_input(UUID16_CHR_REPORT)
{
}

bool TPKBD2BleHost::begin()
{
  // Invoke base class begin()
  BLEClientService::begin();

  _keyboard_input.begin(this);
  _trackpoint_input.begin(this);
  _consumer_input.begin(this);

  // set notify callback
  _keyboard_input.setNotifyCallback(keyboard_client_notify_cb);
  _trackpoint_input.setNotifyCallback(trackpoint_client_notify_cb);
  _consumer_input.setNotifyCallback(consumer_client_notify_cb);

  return true;
}

void TPKBD2BleHost::setKeyboardReportCallback(keyboard_callback_t fp)
{
  _keyboard_cb = fp;
}

void TPKBD2BleHost::setTrackpointReportCallback(trackpoint_callback_t fp)
{
  _trackpoint_cb = fp;
}

void TPKBD2BleHost::setConsumerReportCallback(consumer_callback_t fp)
{
  _consumer_cb = fp;
}

bool TPKBD2BleHost::discover(uint16_t conn_handle)
{
  // Call Base class discover
  VERIFY(BLEClientService::discover(conn_handle));
  _conn_hdl = BLE_CONN_HANDLE_INVALID; // make as invalid until we found all chars

  // Discover all characteristics
  Bluefruit.Discovery.discoverCharacteristic(conn_handle, _keyboard_input, _trackpoint_input, _consumer_input);

  VERIFY(_keyboard_input.discovered() && _trackpoint_input.discovered(), _consumer_input.discovered());

  _conn_hdl = conn_handle;
  return true;
}

//------------------------------------------------------------------+
// Keyboard
//------------------------------------------------------------------+
bool TPKBD2BleHost::enableKeyboard()
{
  return _keyboard_input.enableNotify();
}

bool TPKBD2BleHost::disableKeyboard()
{
  return _keyboard_input.disableNotify();
}

void TPKBD2BleHost::_handle_keyboard_input(uint8_t *data, uint16_t len)
{
  if (_keyboard_cb && (len == sizeof(tpkbd2_keyboard_report_t)))
  {
    _keyboard_cb((tpkbd2_keyboard_report_t *)data);
  }
}

void TPKBD2BleHost::keyboard_client_notify_cb(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
  TPKBD2BleHost &svc = (TPKBD2BleHost &)chr->parentService();
  svc._handle_keyboard_input(data, len);
}

//------------------------------------------------------------------+
// Trackpoint
//------------------------------------------------------------------+
bool TPKBD2BleHost::enableTrackpoint()
{
  return _trackpoint_input.enableNotify();
}

bool TPKBD2BleHost::disableTrackpoint()
{
  return _trackpoint_input.disableNotify();
}

void TPKBD2BleHost::_handle_trackpoint_input(uint8_t *data, uint16_t len)
{
  if (_trackpoint_cb && (len == sizeof(tpkbd2_trackpoint_report_t)))
  {
    _trackpoint_cb((tpkbd2_trackpoint_report_t *)data);
  }
}

void TPKBD2BleHost::trackpoint_client_notify_cb(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
  TPKBD2BleHost &svc = (TPKBD2BleHost &)chr->parentService();
  svc._handle_trackpoint_input(data, len);
}

//------------------------------------------------------------------+
// Consumer
//------------------------------------------------------------------+
bool TPKBD2BleHost::enableConsumer()
{
  return _consumer_input.enableNotify();
}

bool TPKBD2BleHost::disableConsumer()
{
  return _consumer_input.disableNotify();
}

void TPKBD2BleHost::_handle_consumer_input(uint8_t *data, uint16_t len)
{
  if (_consumer_cb && (len == sizeof(tpkbd2_consumer_report_t)))
  {
    _consumer_cb((tpkbd2_consumer_report_t *)data);
  }
}

void TPKBD2BleHost::consumer_client_notify_cb(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
  TPKBD2BleHost &svc = (TPKBD2BleHost &)chr->parentService();
  svc._handle_consumer_input(data, len);
}
