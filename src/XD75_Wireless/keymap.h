#include "BleCommandDsl.h"
#include "CommandDsl.h"
#include "KC.h"

using namespace hidpg;

/* ID
 * ,--------------------------------------------------------------------------------------------------------.
 * |   1  |   6  |  11  |  16  |  21  |  26  |  31  |  36  |  41  |  46  |  51  |  56  |  61  |  66  |  71  |
 * |------+------+------+------+------+------+------+------+------+------+------+------+------+------+------|
 * |   2  |   7  |  12  |  17  |  22  |  27  |  32  |  37  |  42  |  47  |  52  |  57  |  62  |  67  |  72  |
 * |------+------+------+------+------+------+------+------+------+------+------+------+------+------+------|
 * |   3  |   8  |  13  |  18  |  23  |  28  |  33  |  38  |  43  |  48  |  53  |  58  |  63  |  68  |  73  |
 * |------+------+------+------+------+------+------+------+------+------+------+------+------+------+------|
 * |   4  |   9  |  14  |  19  |  24  |  29  |  34  |  39  |  44  |  49  |  54  |  59  |  64  |  69  |  74  |
 * |------+------+------+------+------+------+------+------+------+------+------+------+------+------+------|
 * |   5  |  10  |  15  |  20  |  25  |  30  |  35  |  40  |  45  |  50  |  55  |  60  |  65  |  70  |  75  |
 * `--------------------------------------------------------------------------------------------------------.
 */

Key keymap[] = {
<<<<<<< HEAD
    {1, LY({KC(Escape), _______, HT(RESET(), KC(Escape), 2000)})},
    {2, LY({KC(Tab), _______, KC(Gui + Tab)})},
    {3, TD({KC(Ctrl)},
           {KC(Ctrl + Shift)},
           {KC(Ctrl + Shift + Alt)})},
    {4, MLT(SL(1), TD({KC(Shift)},
                      {KC(Shift + CapsLock)}))},
    {5, NOP()},

    {6, KC(_1)},
    {7, KC(Q)},
    {8, LY({KC(A), _______, KC(Home)})},
    {9, KC(Z)},
    {10, NOP()},

    {11, KC(_2)},
    {12, KC(W)},
    {13, LY({KC(S), _______, KC(PageUp)})},
    {14, KC(X)},
    {15, NOP()},

    {16, KC(_3)},
    {17, KC(E)},
    {18, LY({KC(D), _______, KC(PageDown)})},
    {19, KC(C)},
    {20, KC(Gui)},
=======
  { 1, LY({ NK(Escape), _______, ToH(NK(Escape), 2000, RESET()) }) },
  { 2, LY({ NK(Tab), _______, CK(Gui, Tab) }) },
  { 3, TD({ { MO(Ctrl), MO(Ctrl) },
            { MO(Ctrl + Shift), MO(Ctrl + Shift) },
            { MO(Ctrl + Shift + Alt), MO(Ctrl + Shift + Alt) } }) },
  { 4, MLT({ SL(1), TD({ { MO(Shift), MO(Shift) },
                         { CK(Shift, CapsLock), CK(Shift, CapsLock) } }) }) },
  { 5, SEQ_MODE() },

  { 6, NK(_1) },
  { 7, NK(Q) },
  { 8, LY({ NK(A), _______, NK(Home) }) },
  { 9, NK(Z) },
  { 10, NOP() },

  { 11, NK(_2) },
  { 12, NK(W) },
  { 13, LY({ NK(S), _______, NK(PageUp) }) },
  { 14, NK(X) },
  { 15, NOP() },

  { 16, NK(_3) },
  { 17, NK(E) },
  { 18, LY({ NK(D), _______, NK(PageDown) }) },
  { 19, NK(C) },
  { 20, MO(Gui) },
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34

    {21, LY({KC(_4), _______, KC(F4)})},
    {22, KC(R)},
    {23, LY({KC(F), _______, KC(End)})},
    {24, KC(V)},
    {25, TD({KC(Alt)},
            {KC(Alt + Shift)},
            {KC(Alt + Ctrl)})},

    {26, LY({KC(_5), _______, KC(F5)})},
    {27, KC(T)},
    {28, KC(G)},
    {29, KC(B)},
    {30, HT(SL(2), KC(Int5))},

    {31, KC(PageUp)},
    {32, KC(PageDown)},
    {33, KC(Home)},
    {34, KC(End)},
    {35, KC(Space)},

    {36, KC(PrintScreen)},
    {37, KC(Insert)},
    {38, KC(Delete)},
    {39, KC(Backspace)},
    {40, KC(Enter)},

    {41, LY({KC(_6), _______, KC(F6)})},
    {42, KC(Y)},
    {43, LY({KC(H), _______, KC(Left)})},
    {44, KC(N)},
    {45, HT(SL(2), KC(Int4))},

    {46, LY({KC(_7), _______, KC(F7)})},
    {47, KC(U)},
    {48, LY({KC(J), _______, KC(Down)})},
    {49, KC(M)},
    {50, TD({KC(RightCtrl)},
            {KC(Ctrl + Shift)},
            {KC(Ctrl + Shift + Alt)})},

    {51, LY({KC(_8), _______, KC(F8)})},
    {52, KC(I)},
    {53, LY({KC(K), _______, KC(Up)})},
    {54, KC(Comma)},
    {55, NOP()},

    {56, LY({KC(_9), _______, KC(F9)})},
    {57, KC(O)},
    {58, LY({KC(L), _______, KC(Right)})},
    {59, KC(Period)},
    {60, KC(Up)},

    {61, LY({KC(_0), KC(Int3), KC(F10)})},
    {62, KC(P)},
    {63, KC(Semicolon)},
    {64, KC(Slash)},
    {65, KC(Left)},

    {66, LY({KC(Minus), _______, KC(F11)})},
    {67, KC(LeftBracket)},
    {68, KC(Quote)},
    {69, LY({KC(Int3), KC(Int1)})},
    {70, KC(Down)},

<<<<<<< HEAD
    {71, LY({KC(Equal), _______, KC(F12)})},
    {72, KC(RightBracket)},
    {73, KC(Backslash)},
    {74, MLT(SL(1), TD({KC(Shift)},
                       {KC(Shift + CapsLock)}))},
    {75, KC(Right)},
=======
  { 71, LY({ NK(Equal), _______, NK(F12) }) },
  { 72, NK(RightBracket) },
  { 73, NK(Backslash) },
  { 74, MLT({ SL(1), TD({ { MO(Shift), MO(Shift) },
                          { CK(Shift, CapsLock), CK(Shift, CapsLock) } }) }) },
  { 75, NK(Right) },
>>>>>>> 5177ef91c514989a45e6342f9e170515e3b67f34
};
