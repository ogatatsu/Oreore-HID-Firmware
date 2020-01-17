
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

// スイッチとマトリックスの定義
// IN0
Switch sw1(12);
Switch sw7(24);
Switch sw13(36);
Switch sw19(50);
Switch sw26(64);
// IN1
Switch sw2(11);
Switch sw8(23);
Switch sw14(35);
Switch sw20(49);
Switch sw27(63);
// IN2
Switch sw3(10);
Switch sw9(22);
Switch sw15(34);
Switch sw21(48);
Switch sw28(62);
// IN3
Switch sw4(9);
Switch sw10(21);
Switch sw16(33);
Switch sw22(47);
Switch sw29(61);
// IN4
Switch sw5(8);
Switch sw11(20);
Switch sw17(32);
Switch sw23(46);
Switch sw30(60);
// IN5
Switch sw6(7);
Switch sw12(19);
Switch sw18(31);
Switch sw24(45);
Switch sw31(59);
// IN6
Switch sw25(44);
Switch sw32(58);

Switch *matrix[out_len][in_len] = {
  { &sw1, &sw2, &sw3, &sw4, &sw5, &sw6, nullptr },
  { &sw7, &sw8, &sw9, &sw10, &sw11, &sw12, nullptr },
  { &sw13, &sw14, &sw15, &sw16, &sw17, &sw18, nullptr },
  { &sw19, &sw20, &sw21, &sw22, &sw23, &sw24, &sw25 },
  { &sw26, &sw27, &sw28, &sw29, &sw30, &sw31, &sw32 },
};
