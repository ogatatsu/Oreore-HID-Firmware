#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
constexpr uint8_t OUT0 = 4;  // ROW0
constexpr uint8_t OUT1 = 11; // ROW1
constexpr uint8_t OUT2 = 12; // ROW2
constexpr uint8_t OUT3 = 14; // ROW3
constexpr uint8_t OUT4 = 16; // ROW4
constexpr uint8_t outPins[] = {OUT0, OUT1, OUT2, OUT3, OUT4};

constexpr uint8_t IN0 = 13; // COL0
constexpr uint8_t IN1 = 15; // COL1
constexpr uint8_t IN2 = 31; // COL2
constexpr uint8_t IN3 = 30; // COL3
constexpr uint8_t IN4 = 29; // COL4
constexpr uint8_t IN5 = 28; // COL5
constexpr uint8_t IN6 = 27; // COL6
constexpr uint8_t inPins[] = {IN0, IN1, IN2, IN3, IN4, IN5, IN6};

constexpr uint8_t outLength = sizeof(outPins) / sizeof(outPins[0]);
constexpr uint8_t inLength = sizeof(inPins) / sizeof(inPins[0]);

// スイッチとマトリックスの定義
// IN0
Switch sw1(1);
Switch sw7(13);
Switch sw13(25);
Switch sw19(37);
Switch sw26(51);
// IN1
Switch sw2(2);
Switch sw8(14);
Switch sw14(26);
Switch sw20(38);
Switch sw27(52);
// IN2
Switch sw3(3);
Switch sw9(15);
Switch sw15(27);
Switch sw21(39);
Switch sw28(53);
// IN3
Switch sw4(4);
Switch sw10(16);
Switch sw16(28);
Switch sw22(40);
Switch sw29(54);
// IN4
Switch sw5(5);
Switch sw11(17);
Switch sw17(29);
Switch sw23(41);
Switch sw30(55);
// IN5
Switch sw6(6);
Switch sw12(18);
Switch sw18(30);
Switch sw24(42);
Switch sw31(56);
// IN6
Switch sw25(43);
Switch sw32(57);

Switch *matrix[outLength][inLength] = {
    {&sw1, &sw2, &sw3, &sw4, &sw5, &sw6, nullptr},
    {&sw7, &sw8, &sw9, &sw10, &sw11, &sw12, nullptr},
    {&sw13, &sw14, &sw15, &sw16, &sw17, &sw18, nullptr},
    {&sw19, &sw20, &sw21, &sw22, &sw23, &sw24, &sw25},
    {&sw26, &sw27, &sw28, &sw29, &sw30, &sw31, &sw32},
};
