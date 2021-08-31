#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
constexpr uint8_t OUT0 = 13; // ROW0
constexpr uint8_t OUT1 = 2;  // ROW1
constexpr uint8_t OUT2 = 14; // ROW2
constexpr uint8_t OUT3 = 16; // ROW3
constexpr uint8_t OUT4 = 18; // ROW4
constexpr uint8_t out_pins[] = { OUT0, OUT1, OUT2, OUT3, OUT4 };

constexpr uint8_t IN0 = 3; // COL0
constexpr uint8_t IN1 = 4; // COL1
constexpr uint8_t IN2 = 5; // COL2
constexpr uint8_t IN3 = 7; // COL3
constexpr uint8_t in_pins[] = { IN0, IN1, IN2, IN3 };

constexpr uint8_t out_len = sizeof(out_pins) / sizeof(out_pins[0]);
constexpr uint8_t in_len = sizeof(in_pins) / sizeof(in_pins[0]);

// マトリックスの定義
Switch *matrix[out_len][in_len] = {
  { SW(1), SW(6), SW(11), SW(16) },
  { SW(2), SW(7), SW(12), SW(17) },
  { SW(3), SW(8), SW(13), SW(18) },
  { SW(4), SW(9), SW(14), SW(19) },
  { SW(5), SW(10), SW(15), SW(20) },
};
