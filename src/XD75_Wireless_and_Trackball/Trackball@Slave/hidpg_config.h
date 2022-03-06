
#pragma once

#define BLE_OWN_ADDR {0x36, 0x9E, 0x59, 0xB6, 0xF4, 0xE0}

#define PMW3360DM_Run_Downshift 50   // 500ms
#define PMW3360DM_Rest1_Rate 9       // 10ms
#define PMW3360DM_Rest1_Downshift 1  // 3.2s
#define PMW3360DM_Rest2_Rate 99      // 100ms
#define PMW3360DM_Rest2_Downshift 57 // 3.04min
#define PMW3360DM_Rest3_Rate 499     // 500ms

#define PMW3360DM_NCS_PIN 25
#define PMW3360DM_INTERRUPT_PIN 23

// 電池の最大電圧
#define MAX_BATTERY_VOLTAGE 3.0
// 電池の最小電圧
#define MIN_BATTERY_VOLTAGE 2.0
// 電池の電圧を読み取る間隔
#define READ_BATTERY_VOLTAGE_INTERVAL_MS 60000
