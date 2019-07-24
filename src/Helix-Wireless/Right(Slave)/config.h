
#pragma once

// デバイスの名前、ペアリングする時にPCやスマホ側に出てくる名前
#define DEVICE_NAME "Helix Wireless Slave"

// 自身のアドレスを変更したい場合は定義する(random static address)
#define OWN_ADDR {0x36, 0x9E, 0x59, 0xB6, 0xF4, 0xE0}

// 割り込みにSense signalを使用するか
// nRF52でなおかつcustom version frameworkでのみ使用可能
#define USE_SENSE_INTERRUPT true
