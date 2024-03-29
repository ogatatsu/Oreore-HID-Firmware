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

// マトリックスの定義
static Switch *matrix[outLength][inLength] = {
  { SW(10), SW(7), SW(4), SW(1) },
  { SW(11), SW(8), SW(5), SW(2) },
  { SW(12), SW(9), SW(6), SW(3) },
};
