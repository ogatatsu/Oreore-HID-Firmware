#include "BleCommand.h"
#include "HidEngine.h"
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
  { 1, LY({ NK(Escape), _______, ToH(NK(Escape), 2000, RESET) }) },
  { 2, LY({ NK(Tab), _______, CK(Gui, Tab) }) },
  { 3, TD({ { MO(Ctrl), MO(Ctrl) },
            { MO(Ctrl + Shift), MO(Ctrl + Shift) },
            { MO(Ctrl + Shift + Alt), MO(Ctrl + Shift + Alt) } }) },
  { 4, DBL(SL(1), TD({ { OSM(Shift), OSM(Shift) },
                       { CK(Shift, CapsLock), CK(Shift, CapsLock) } })) },
  { 5, SEQ_MODE },

  { 6, LY({ NK(_1), _______, ToH(NK(F1), 1500, BT(1)) }) },
  { 7, NK(Q) },
  { 8, LY({ NK(A), _______, NK(Home) }) },
  { 9, NK(Z) },
  { 10, NOP },

  { 11, LY({ NK(_2), _______, ToH(NK(F2), 1500, BT(2)) }) },
  { 12, NK(W) },
  { 13, LY({ NK(S), _______, NK(PageUp) }) },
  { 14, NK(X) },
  { 15, NOP },

  { 16, LY({ NK(_3), _______, ToH(NK(F3), 1500, BT(3)) }) },
  { 17, NK(E) },
  { 18, LY({ NK(D), _______, NK(PageDown) }) },
  { 19, NK(C) },
  { 20, MO(Gui) },

  { 21, LY({ NK(_4), _______, NK(F4) }) },
  { 22, NK(R) },
  { 23, LY({ NK(F), _______, NK(End) }) },
  { 24, NK(V) },
  { 25, TD({ { MO(Alt), MO(Alt) },
             { MO(Alt + Shift), MO(Alt + Shift) },
             { MO(Alt + Ctrl), MO(Alt + Ctrl) } }) },

  { 26, LY({ NK(_5), _______, NK(F5) }) },
  { 27, NK(T) },
  { 28, NK(G) },
  { 29, NK(B) },
  { 30, LT(2, NK(Int5)) },

  { 31, NK(PageUp) },
  { 32, NK(PageDown) },
  { 33, NK(Home) },
  { 34, NK(End) },
  { 35, NK(Space) },

  { 36, NK(PrintScreen) },
  { 37, NK(Insert) },
  { 38, NK(Delete) },
  { 39, NK(Backspace) },
  { 40, NK(Enter) },

  { 41, LY({ NK(_6), _______, NK(F6) }) },
  { 42, NK(Y) },
  { 43, LY({ NK(H), _______, NK(ArrowLeft) }) },
  { 44, NK(N) },
  { 45, LT(2, NK(Int4)) },

  { 46, LY({ NK(_7), _______, NK(F7) }) },
  { 47, NK(U) },
  { 48, LY({ NK(J), _______, NK(ArrowDown) }) },
  { 49, NK(M) },
  { 50, TD({ { MO(RightCtrl), MO(RightCtrl) },
             { MO(Ctrl + Shift), MO(Ctrl + Shift) },
             { MO(Ctrl + Shift + Alt), MO(Ctrl + Shift + Alt) } }) },

  { 51, LY({ NK(_8), _______, NK(F8) }) },
  { 52, NK(I) },
  { 53, LY({ NK(K), _______, NK(ArrowUp) }) },
  { 54, NK(Comma) },
  { 55, NOP },

  { 56, LY({ NK(_9), _______, NK(F9) }) },
  { 57, NK(O) },
  { 58, LY({ NK(L), _______, NK(ArrowRight) }) },
  { 59, NK(Period) },
  { 60, NK(ArrowUp) },

  { 61, LY({ NK(_0), NK(Int3), NK(F10) }) },
  { 62, NK(P) },
  { 63, NK(Semicolon) },
  { 64, NK(Slash) },
  { 65, NK(ArrowLeft) },

  { 66, LY({ NK(Minus), _______, NK(F11) }) },
  { 67, NK(BracketLeft) },
  { 68, NK(Quote) },
  { 69, LY({ NK(Int3), NK(Int1) }) },
  { 70, NK(ArrowDown) },

  { 71, LY({ NK(Equal), _______, NK(F12) }) },
  { 72, NK(BracketRight) },
  { 73, NK(Backslash) },
  { 74, DBL(SL(1), TD({ { OSM(Shift), OSM(Shift) },
                        { CK(Shift, CapsLock), CK(Shift, CapsLock) } })) },
  { 75, NK(ArrowRight) },
};
