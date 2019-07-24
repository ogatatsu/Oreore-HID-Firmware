
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define DEVICE_NAME "4x5 keypad"

// 割り込みにSense signalを使用するか
// nRF52でなおかつcustom version frameworkでのみ使用可能
#define USE_SENSE_INTERRUPT true
