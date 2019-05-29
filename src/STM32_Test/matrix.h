#include "MatrixScan.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
static constexpr uint8_t OUT0 = PB9; // X
static constexpr uint8_t OUT1 = PB8; // Y
static constexpr uint8_t OUT2 = PB7; // Z
static constexpr uint8_t outPins[] = { OUT0, OUT1, OUT2 };

static constexpr uint8_t IN0 = PB6; // A
static constexpr uint8_t IN1 = PB5; // B
static constexpr uint8_t IN2 = PB4; // C
static constexpr uint8_t IN3 = PB3; // D
static constexpr uint8_t inPins[] = { IN0, IN1, IN2, IN3 };

constexpr uint8_t outLength = sizeof(outPins) / sizeof(outPins[0]);
constexpr uint8_t inLength = sizeof(inPins) / sizeof(inPins[0]);

// スイッチとマトリックスの定義
// IN0
static Switch sw10(10); //sw*
static Switch sw11(11); //sw0
static Switch sw12(12); //sw#
// IN1
static Switch sw7(7);
static Switch sw8(8);
static Switch sw9(9);
// IN2
static Switch sw4(4);
static Switch sw5(5);
static Switch sw6(6);
// IN3
static Switch sw1(1);
static Switch sw2(2);
static Switch sw3(3);

static Switch *matrix[outLength][inLength] = {
  { &sw10, &sw7, &sw4, &sw1 },
  { &sw11, &sw8, &sw5, &sw2 },
  { &sw12, &sw9, &sw6, &sw3 },
};
