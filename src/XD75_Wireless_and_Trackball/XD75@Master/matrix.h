#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
constexpr uint8_t OUT0 = 3;   // COL0
constexpr uint8_t OUT1 = 2;   // COL1
constexpr uint8_t OUT2 = 4;   // COL2
constexpr uint8_t OUT3 = 12;  // COL3
constexpr uint8_t OUT4 = 11;  // COL4
constexpr uint8_t OUT5 = 18;  // COL5
constexpr uint8_t OUT6 = 24;  // COL6
constexpr uint8_t OUT7 = 23;  // COL7
constexpr uint8_t OUT8 = 7;   // COL8
constexpr uint8_t OUT9 = 26;  // COL9
constexpr uint8_t OUT10 = 25; // COL10
constexpr uint8_t OUT11 = 27; // COL11
constexpr uint8_t OUT12 = 30; // COL12
constexpr uint8_t OUT13 = 29; // COL13
constexpr uint8_t OUT14 = 28; // COL14
constexpr uint8_t out_pins[] = { OUT0, OUT1, OUT2, OUT3, OUT4, OUT5, OUT6, OUT7, OUT8, OUT9, OUT10, OUT11, OUT12, OUT13, OUT14 };

constexpr uint8_t IN0 = 16; // ROW0
constexpr uint8_t IN1 = 31; // ROW1
constexpr uint8_t IN2 = 13; // ROW2
constexpr uint8_t IN3 = 15; // ROW3
constexpr uint8_t IN4 = 5;  // ROW4
constexpr uint8_t in_pins[] = { IN0, IN1, IN2, IN3, IN4 };

constexpr uint8_t out_len = sizeof(out_pins) / sizeof(out_pins[0]);
constexpr uint8_t in_len = sizeof(in_pins) / sizeof(in_pins[0]);

// マトリックスの定義
Switch *matrix[out_len][in_len] = {
  { SW(1), SW(2), SW(3), SW(4), SW(5) },
  { SW(6), SW(7), SW(8), SW(9), SW(10) },
  { SW(11), SW(12), SW(13), SW(14), SW(15) },
  { SW(16), SW(17), SW(18), SW(19), SW(20) },
  { SW(21), SW(22), SW(23), SW(24), SW(25) },
  { SW(26), SW(27), SW(28), SW(29), SW(30) },
  { SW(31), SW(32), SW(33), SW(34), SW(35) },
  { SW(36), SW(37), SW(38), SW(39), SW(40) },
  { SW(41), SW(42), SW(43), SW(44), SW(45) },
  { SW(46), SW(47), SW(48), SW(49), SW(50) },
  { SW(51), SW(52), SW(53), SW(54), SW(55) },
  { SW(56), SW(57), SW(58), SW(59), SW(60) },
  { SW(61), SW(62), SW(63), SW(64), SW(65) },
  { SW(66), SW(67), SW(68), SW(69), SW(70) },
  { SW(71), SW(72), SW(73), SW(74), SW(75) },
};
