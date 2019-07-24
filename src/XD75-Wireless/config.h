
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define DEVICE_NAME "XD75 Wireless"

// アドバタイジング時に点滅させるLEDのピン番号
#define ADV_LED_PIN 14

// LEDをHighDriveで光らせるかどうか
#define IS_HIGH_DRIVE true

// スイッチがHIGHとLOWどちらでONになるか
#define ACTIVE_STATE HIGH

// 割り込みにSense signalを使用するか
// nRF52でなおかつcustom version frameworkでのみ使用可能
#define USE_SENSE_INTERRUPT true
