
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define BLE_DEVICE_NAME "SimpleTrackball"

// ペアリングする時にセントラル側の設定画面に現れる見た目
#define BLE_APPEARANCE BLE_APPEARANCE_HID_MOUSE

// 電池の最大電圧
#define MAX_BATTERY_VOLTAGE 3.0
// 電池の最小電圧
#define MIN_BATTERY_VOLTAGE 2.0
// 電池の電圧を読み取る間隔
#define READ_BATTERY_VOLTAGE_INTERVAL_MS 60000

#define PMW3360DM_NCS_PIN 30
#define PMW3360DM_INTERRUPT_PIN 27
