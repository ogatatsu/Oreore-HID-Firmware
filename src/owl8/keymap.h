
/* ID
 * ,---------------------------.
 * |   1  |   2  |   3  |   4  |
 * |------+------+------+------+
 * |   5  |   6  |   7  |   8  |
 * `---------------------------.
 */

#include "BleCommandDsl.h"
#include "CommandDsl.h"
#include "KC.h"

using namespace hidpg;

constexpr uint8_t ENCODER_LEFT_ID = 0;
constexpr uint8_t ENCODER_RIGHT_ID = 1;

Key keymap[] = {
  { 1, NK(A) },
  { 2, NK(B) },
  { 3, NK(C) },
  { 4, NK(D) },
  { 5, NK(E) },
  { 6, NK(F) },
  { 7, NK(G) },
  { 8, NK(H) },
};

Encoder encoderMap[] = {
  // { id, counterclockwise_command, clockwise_command }
  { ENCODER_LEFT_ID, CC(VolumeDown), CC(VolumeUp) },
  { ENCODER_RIGHT_ID, MS_SCR(1, 0), MS_SCR(-1, 0) },
};
