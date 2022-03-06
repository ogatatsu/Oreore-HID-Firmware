#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
constexpr uint8_t OUT0 = 4;  // ROW0
constexpr uint8_t OUT1 = 11; // ROW1
constexpr uint8_t OUT2 = 12; // ROW2
constexpr uint8_t OUT3 = 14; // ROW3
constexpr uint8_t OUT4 = 16; // ROW4
constexpr uint8_t out_pins[] = { OUT0, OUT1, OUT2, OUT3, OUT4 };

constexpr uint8_t IN0 = 13; // COL0
constexpr uint8_t IN1 = 15; // COL1
constexpr uint8_t IN2 = 31; // COL2
constexpr uint8_t IN3 = 30; // COL3
constexpr uint8_t IN4 = 29; // COL4
constexpr uint8_t IN5 = 28; // COL5
constexpr uint8_t IN6 = 27; // COL6
constexpr uint8_t in_pins[] = { IN0, IN1, IN2, IN3, IN4, IN5, IN6 };

constexpr uint8_t out_len = sizeof(out_pins) / sizeof(out_pins[0]);
constexpr uint8_t in_len = sizeof(in_pins) / sizeof(in_pins[0]);

// マトリックスの定義
Switch *matrix[out_len][in_len] = {
  { SW(12), SW(11), SW(10), SW(9), SW(8), SW(7), nullptr },
  { SW(24), SW(23), SW(22), SW(21), SW(20), SW(19), nullptr },
  { SW(36), SW(35), SW(34), SW(33), SW(32), SW(31), nullptr },
  { SW(50), SW(49), SW(48), SW(47), SW(46), SW(45), SW(44) },
  { SW(64), SW(63), SW(62), SW(61), SW(60), SW(59), SW(58) },
};
