
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define BLE_DEVICE_NAME "Helix Wireless"

// アドバタイジング時に点滅させるLEDのピン番号
#define BLE_ADV_LED_PIN PIN_LED2

// スレーブ側のアドレスのリスト、このリストでフィルタして他の機器と接続しないようにする
#define BLE_SLAVE_ADDR_LIST {                 \
    {0x36, 0x9E, 0x59, 0xB6, 0xF4, 0xE0}, \
}

// スレーブがある場合のみ使用される
// スキャン時に点滅させるLEDのピン番号
#define BLE_SCAN_LED_PIN PIN_LED1

// 割り込みにSense signalを使用するか
// nRF52でのみ使用可能
#define MATRIX_SCAN_USE_SENSE_INTERRUPT true

// 電池の最大電圧
#define MAX_BATTERY_VOLTAGE 3.0
// 電池の最小電圧
#define MIN_BATTERY_VOLTAGE 2.0
// 電池の電圧を読み取る間隔
#define READ_BATTERY_VOLTAGE_INTERVAL_MS 60000
