#include "MatrixScan.h"
#include "variant.h"

using namespace hidpg;

// マトリックス回路で使うピンの定義
static constexpr uint8_t OUT0 = 6; // X
static constexpr uint8_t OUT1 = 5; // Y
static constexpr uint8_t OUT2 = 4; // Z
static constexpr uint8_t out_pins[] = { OUT0, OUT1, OUT2 };

static constexpr uint8_t IN0 = 3; // A
static constexpr uint8_t IN1 = 2; // B
static constexpr uint8_t IN2 = 1; // C
static constexpr uint8_t IN3 = 0; // D
static constexpr uint8_t in_pins[] = { IN0, IN1, IN2, IN3 };

constexpr uint8_t outLength = sizeof(out_pins) / sizeof(out_pins[0]);
constexpr uint8_t inLength = sizeof(in_pins) / sizeof(in_pins[0]);

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
