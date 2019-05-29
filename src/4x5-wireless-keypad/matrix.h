#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
constexpr uint8_t OUT0 = 13; // ROW0
constexpr uint8_t OUT1 = 2;  // ROW1
constexpr uint8_t OUT2 = 14; // ROW2
constexpr uint8_t OUT3 = 16; // ROW3
constexpr uint8_t OUT4 = 18; // ROW4
constexpr uint8_t outPins[] = { OUT0, OUT1, OUT2, OUT3, OUT4 };

constexpr uint8_t IN0 = 3; // COL0
constexpr uint8_t IN1 = 4; // COL1
constexpr uint8_t IN2 = 5; // COL2
constexpr uint8_t IN3 = 7; // COL3
constexpr uint8_t inPins[] = { IN0, IN1, IN2, IN3 };

constexpr uint8_t outLength = sizeof(outPins) / sizeof(outPins[0]);
constexpr uint8_t inLength = sizeof(inPins) / sizeof(inPins[0]);

// スイッチとマトリックスの定義
// IN0
Switch sw1(1);
Switch sw2(2);
Switch sw3(3);
Switch sw4(4);
Switch sw5(5);
// IN1
Switch sw6(6);
Switch sw7(7);
Switch sw8(8);
Switch sw9(9);
Switch sw10(10);
// IN2
Switch sw11(11);
Switch sw12(12);
Switch sw13(13);
Switch sw14(14);
Switch sw15(15);
// IN3
Switch sw16(16);
Switch sw17(17);
Switch sw18(18);
Switch sw19(19);
Switch sw20(20);

Switch *matrix[outLength][inLength] = {
  { &sw1, &sw6, &sw11, &sw16 },
  { &sw2, &sw7, &sw12, &sw17 },
  { &sw3, &sw8, &sw13, &sw18 },
  { &sw4, &sw9, &sw14, &sw19 },
  { &sw5, &sw10, &sw15, &sw20 },
};
