
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define DEVICE_NAME "Helix Wireless"

// アドバタイジング時に点滅させるLEDのピン番号
#define ADV_LED_PIN PIN_LED2

// スレーブ側のアドレスのリスト、このリストでフィルタして他の機器と接続しないようにする
#define SLAVE_ADDR_LIST {                 \
    {0x36, 0x9E, 0x59, 0xB6, 0xF4, 0xE0}, \
}

// スレーブがある場合のみ使用される
// スキャン時に点滅させるLEDのピン番号
#define SCAN_LED_PIN PIN_LED1

// 割り込みにSense signalを使用するか
// nRF52でなおかつcustom version frameworkでのみ使用可能
#define USE_SENSE_INTERRUPT true
