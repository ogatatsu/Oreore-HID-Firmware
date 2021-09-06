
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define BLE_DEVICE_NAME "4x5 keypad"

// 割り込みにSense signalを使用するか
// nRF52でのみ使用可能
#define MATRIX_SCAN_USE_SENSE_INTERRUPT true

// 電池の最大電圧
#define MAX_BATTERY_VOLTAGE 3.0
// 電池の最小電圧
#define MIN_BATTERY_VOLTAGE 2.0
// 電池の電圧を読み取る間隔
#define READ_BATTERY_VOLTAGE_INTERVAL_MS 60000
