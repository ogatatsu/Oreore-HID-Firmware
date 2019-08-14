#include "BleCommand.h"
#include "HidEngine.h"

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

ID_and_Command keymap[] = {
  { 1, LY({ NK(_Escape), _______, ToH(NK(_Escape), 2000, RESET) }) },
  { 2, LY({ NK(_Tab), _______, CK(_Gui, _Tab) }) },
  { 3, TD({ { MO(_Ctrl), MO(_Ctrl) },
            { MO(_Ctrl | _Shift), MO(_Ctrl | _Shift) },
            { MO(_Ctrl | _Shift | _Alt), MO(_Ctrl | _Shift | _Alt) } }) },
  { 4, DBL(SL(1), TD({ { MO(_Shift), MO(_Shift) },
                       { CK(_Shift, _CapsLock), CK(_Shift, _CapsLock) } })) },
  { 5, SEQ_MODE },

  { 6, LY({ NK(_1), _______, ToH(NK(_F1), 1500, BT(1)) }) },
  { 7, NK(_Q) },
  { 8, LY({ NK(_A), _______, MS_CLK(_LeftButton) }) },
  { 9, NK(_Z) },
  { 10, NOP },

  { 11, LY({ NK(_2), _______, ToH(NK(_F2), 1500, BT(2)) }) },
  { 12, NK(_W) },
  { 13, LY({ NK(_S), _______, TRT(1, MS_CLK(_MiddleButton)) }) },
  { 14, NK(_X) },
  { 15, NOP },

  { 16, LY({ NK(_3), _______, ToH(NK(_F3), 1500, BT(3)) }) },
  { 17, NK(_E) },
  { 18, LY({ NK(_D), _______, MS_CLK(_RightButton) }) },
  { 19, NK(_C) },
  { 20, MO(_Gui) },

  { 21, LY({ NK(_4), _______, NK(_F4) }) },
  { 22, NK(_R) },
  { 23, NK(_F) },
  { 24, NK(_V) },
  { 25, TD({ { MO(_Alt), MO(_Alt) },
             { MO(_Alt | _Shift), MO(_Alt | _Shift) },
             { MO(_Alt | _Ctrl), MO(_Alt | _Ctrl) } }) },

  { 26, LY({ NK(_5), _______, NK(_F5) }) },
  { 27, NK(_T) },
  { 28, NK(_G) },
  { 29, NK(_B) },
  { 30, LT(2, NK(_Int5)) },

  { 31, NK(_PageUp) },
  { 32, NK(_PageDown) },
  { 33, NK(_Home) },
  { 34, NK(_End) },
  { 35, NK(_Space) },

  { 36, NK(_PrintScreen) },
  { 37, NK(_Insert) },
  { 38, NK(_Delete) },
  { 39, NK(_Backspace) },
  { 40, NK(_Enter) },

  { 41, LY({ NK(_6), _______, NK(_F6) }) },
  { 42, NK(_Y) },
  { 43, LY({ NK(_H), _______, NK(_ArrowLeft) }) },
  { 44, NK(_N) },
  { 45, LT(2, NK(_Int4)) },

  { 46, LY({ NK(_7), _______, NK(_F7) }) },
  { 47, NK(_U) },
  { 48, LY({ NK(_J), _______, NK(_ArrowDown) }) },
  { 49, NK(_M) },
  { 50, TD({ { MO(_Ctrl), MO(_Ctrl) },
             { MO(_Ctrl | _Shift), MO(_Ctrl | _Shift) },
             { MO(_Ctrl | _Shift | _Alt), MO(_Ctrl | _Shift | _Alt) } }) },

  { 51, LY({ NK(_8), _______, NK(_F8) }) },
  { 52, NK(_I) },
  { 53, LY({ NK(_K), _______, NK(_ArrowUp) }) },
  { 54, NK(_Comma) },
  { 55, NOP },

  { 56, LY({ NK(_9), _______, NK(_F9) }) },
  { 57, NK(_O) },
  { 58, LY({ NK(_L), _______, NK(_ArrowRight) }) },
  { 59, NK(_Period) },
  { 60, NK(_ArrowUp) },

  { 61, LY({ NK(_0), NK(_Int3), NK(_F10) }) },
  { 62, NK(_P) },
  { 63, NK(_Semicolon) },
  { 64, NK(_Slash) },
  { 65, NK(_ArrowUp) },

  { 66, LY({ NK(_Minus), _______, NK(_F11) }) },
  { 67, NK(_BracketLeft) },
  { 68, NK(_Quote) },
  { 69, NK(_Int1) },
  { 70, NK(_ArrowDown) },

  { 71, LY({ NK(_Equal), _______, NK(_F12) }) },
  { 72, NK(_BracketRight) },
  { 73, NK(_Backslash) },
  { 74, DBL(SL(1), TD({ { MO(_Shift), MO(_Shift) },
                        { CK(_Shift, _CapsLock), CK(_Shift, _CapsLock) } })) },
  { 75, NK(_ArrowRight) },
};

TrackID_and_Command trackmap[] = {
  { 1, 50, MS_SCR(1, 0), MS_SCR(-1, 0) },
};
