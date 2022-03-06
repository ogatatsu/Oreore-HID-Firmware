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
  { SW(1), SW(2), SW(3), SW(4), SW(5), SW(6), nullptr },
  { SW(13), SW(14), SW(15), SW(16), SW(17), SW(18), nullptr },
  { SW(25), SW(26), SW(27), SW(28), SW(29), SW(30), nullptr },
  { SW(37), SW(38), SW(39), SW(40), SW(41), SW(42), SW(43) },
  { SW(51), SW(52), SW(53), SW(54), SW(55), SW(56), SW(57) },
};
