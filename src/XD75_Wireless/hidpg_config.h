
#pragma once

// スイッチがHIGHとLOWどちらでONになるか
#define MATRIX_SCAN_ACTIVE_STATE HIGH

// 割り込みにSense signalを使用するか
// nRF52でのみ使用可能
#define MATRIX_SCAN_USE_NRF52_SENSE_INTERRUPT true
