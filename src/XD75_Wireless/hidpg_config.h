
#pragma once

<<<<<<< HEAD
// スイッチがHIGHとLOWどちらでONになるか
#define MATRIX_SCAN_ACTIVE_STATE HIGH
=======
// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define BLE_DEVICE_NAME "Helix Wireless"

// 左手側のアドレス
#define BLE_PEER_ADDR {0x36, 0x9E, 0x59, 0xB6, 0xF4, 0xE0}
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34

// 割り込みにSense signalを使用するか
// nRF52でのみ使用可能
#define MATRIX_SCAN_USE_NRF52_SENSE_INTERRUPT true
<<<<<<< HEAD
=======

// 電池の最大電圧
#define MAX_BATTERY_VOLTAGE 3.0
// 電池の最小電圧
#define MIN_BATTERY_VOLTAGE 2.0
// 電池の電圧を読み取る間隔
#define READ_BATTERY_VOLTAGE_INTERVAL_MS (1000 * 60)
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34
