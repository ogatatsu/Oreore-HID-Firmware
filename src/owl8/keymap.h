
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

constexpr EncoderId EncLeft{0};
constexpr EncoderId EncRight{1};

uint8_t MOUSE_ID = 0;

Key keymap[] = {
    {1, KC(A)},
    {2, KC(B)},
    {3, KC(C)},
    {4, KC(D)},
    {5, KC(E)},
    {6, KC(F)},
    {7, KC(G)},
    {8, KC(H)},
};

Encoder encoderMap[] = {
    // { encoder_id, step,
    //   counterclockwise_command, clockwise_command }

    {EncLeft, 1,
     MS_SCR(-2, 0), MS_SCR(2, 0)},

    {EncRight, 1,
     MS_SCR(0, -2), MS_SCR(0, 2)},
};
