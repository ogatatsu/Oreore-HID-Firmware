#include "BleCommand.h"
#include "HidEngine.h"

using namespace hidpg;

/* ID
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |   1  |   2  |   3  |   4  |   5  |   6  |             |   7  |   8  |   9  |  10  |  11  |  12  |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |  13  |  14  |  15  |  16  |  17  |  18  |             |  19  |  20  |  21  |  22  |  23  |  24  |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |  25  |  26  |  27  |  28  |  29  |  30  |             |  31  |  32  |  33  |  34  |  35  |  36  |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |  37  |  38  |  39  |  40  |  41  |  42  |  43  |  44  |  45  |  46  |  47  |  48  |  49  |  50  |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |  51  |  52  |  53  |  54  |  55  |  56  |  57  |  58  |  59  |  60  |  61  |  62  |  63  |  64  |
   * `-------------------------------------------------------------------------------------------------'
   */

/* Layer 0 (qwerty)
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |   `  |   1  |   2  |   3  |   4  |   5  |             |   6  |   7  |   8  |   9  |   0  | Del  |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * | Tab  |   Q  |   W  |   E  |   R  |   T  |             |   Y  |   U  |   I  |   O  |   P  | Bksp |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * | Ctrl |   A  |   S  |   D  |   F  |   G  |             |   H  |   J  |   K  |   L  |   ;  |  '   |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * | Shift|   Z  |   X  |   C  |   V  |   B  |   [  |   ]  |   N  |   M  |   ,  |   .  |   /  |Enter |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |  NOP | Esc  | Alt  | GUI  | EISU |Lower |Space |Space |Raise | KANA | Left | Down |  Up  |Right |
   * `-------------------------------------------------------------------------------------------------'
   */

/* Layer 1 (Lower)
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |   ~  |   !  |   @  |   #  |   $  |   %  |             |   ^  |   &  |   *  |   (  |   )  |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |   ~  |   !  |   @  |   #  |   $  |   %  |             |   ^  |   &  |   *  |   (  |   )  |      |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |  F1  |  F2  |  F3  |  F4  |  F5  |             |  F6  |   _  |   +  |   {  |   }  |  |   |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  (   |   )  |  F12 |      |      | Home | End  |      |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |      |      |      |      |      |      |      |      |      |      | Next | Vol- | Vol+ | Play |
   * `-------------------------------------------------------------------------------------------------'
   */

/* Layer 2 (Raise)
   * ,-----------------------------------------.             ,-----------------------------------------.
   * |   `  |   1  |   2  |   3  |   4  |   5  |             |   6  |   7  |   8  |   9  |   0  | Bksp |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |   `  |   1  |   2  |   3  |   4  |   5  |             |   6  |   7  |   8  |   9  |   0  | Del  |
   * |------+------+------+------+------+------|             |------+------+------+------+------+------|
   * |      |  F1  |  F2  |  F3  |  F4  |  F5  |             |  F6  |   -  |   =  |   [  |   ]  |  \   |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |      |  F7  |  F8  |  F9  |  F10 |  F11 |      |      |  F12 |      |      |PageDn|PageUp|      |
   * |------+------+------+------+------+------+------+------+------+------+------+------+------+------|
   * |      |      |      |      |      |      |      |      |      |      | Next | Vol- | Vol+ | Play |
   * `-------------------------------------------------------------------------------------------------'
   */

constexpr int LOWER = 1;
constexpr int RAISE = 2;

ID_and_Command keymap[] = {
  { 1, LY({ NK(_Grave), CK(_Shift, _Grave), NK(_Grave) }) },
  { 2, LY({ NK(_1), ToH(CK(_Shift, _1), 1500, BT(1)), NK(_1) }) },
  { 3, LY({ NK(_2), ToH(CK(_Shift, _2), 1500, BT(2)), NK(_2) }) },
  { 4, LY({ NK(_3), ToH(CK(_Shift, _3), 1500, BT(3)), NK(_3) }) },
  { 5, LY({ NK(_4), CK(_Shift, _4), NK(_4) }) },
  { 6, LY({ NK(_5), CK(_Shift, _5), NK(_5) }) },
  { 7, LY({ NK(_6), CK(_Shift, _6), NK(_6) }) },
  { 8, LY({ NK(_7), CK(_Shift, _7), NK(_7) }) },
  { 9, LY({ NK(_8), CK(_Shift, _8), NK(_8) }) },
  { 10, LY({ NK(_9), CK(_Shift, _9), NK(_9) }) },
  { 11, LY({ NK(_0), CK(_Shift, _0), NK(_0) }) },
  { 12, LY({ NK(_Delete), _______, NK(_Backspace) }) },

  { 13, LY({ NK(_Tab), CK(_Shift, _Grave), NK(_Grave) }) },
  { 14, LY({ NK(_Q), CK(_Shift, _1), NK(_1) }) },
  { 15, LY({ NK(_W), CK(_Shift, _2), NK(_2) }) },
  { 16, LY({ NK(_E), CK(_Shift, _3), NK(_3) }) },
  { 17, LY({ NK(_R), CK(_Shift, _4), NK(_4) }) },
  { 18, LY({ NK(_T), CK(_Shift, _5), NK(_5) }) },
  { 19, LY({ NK(_Y), CK(_Shift, _6), NK(_6) }) },
  { 20, LY({ NK(_U), CK(_Shift, _7), NK(_7) }) },
  { 21, LY({ NK(_I), CK(_Shift, _8), NK(_8) }) },
  { 22, LY({ NK(_O), CK(_Shift, _9), NK(_9) }) },
  { 23, LY({ NK(_P), CK(_Shift, _0), NK(_0) }) },
  { 24, LY({ NK(_Backspace), _______, NK(_Delete) }) },

  { 25, LY({ MO(_Ctrl), _______, _______ }) },
  { 26, LY({ NK(_A), NK(_F1), NK(_F1) }) },
  { 27, LY({ NK(_S), NK(_F2), NK(_F2) }) },
  { 28, LY({ NK(_D), NK(_F3), NK(_F3) }) },
  { 29, LY({ NK(_F), NK(_F4), NK(_F4) }) },
  { 30, LY({ NK(_G), NK(_F5), NK(_F5) }) },
  { 31, LY({ NK(_H), NK(_F6), NK(_F6) }) },
  { 32, LY({ NK(_J), CK(_Shift, _Minus), NK(_Minus) }) },
  { 33, LY({ NK(_K), CK(_Shift, _Equal), NK(_Equal) }) },
  { 34, LY({ NK(_L), CK(_Shift, _BracketLeft), NK(_BracketLeft) }) },
  { 35, LY({ NK(_Semicolon), CK(_Shift, _BracketRight), NK(_BracketRight) }) },
  { 36, LY({ NK(_Quote), CK(_Shift, _Backslash), NK(_Backslash) }) },

  { 37, LY({ MO(_Shift), _______, _______ }) },
  { 38, LY({ NK(_Z), NK(_F7), NK(_F7) }) },
  { 39, LY({ NK(_X), NK(_F8), NK(_F8) }) },
  { 40, LY({ NK(_C), NK(_F9), NK(_F9) }) },
  { 41, LY({ NK(_V), NK(_F10), NK(_F10) }) },
  { 42, LY({ NK(_B), NK(_F11), NK(_F11) }) },
  { 43, LY({ NK(_BracketLeft), CK(_Shift, _9), _______ }) },
  { 44, LY({ NK(_BracketRight), CK(_Shift, _0), _______ }) },
  { 45, LY({ NK(_N), NK(_F12), NK(_F12) }) },
  { 46, LY({ NK(_M), _______, _______ }) },
  { 47, LY({ NK(_Comma), _______, _______ }) },
  { 48, LY({ NK(_Period), NK(_Home), NK(_PageDown) }) },
  { 49, LY({ NK(_Slash), NK(_End), NK(_PageUp) }) },
  { 50, LY({ NK(_Enter), _______, _______ }) },

  { 51, NOP },
  { 52, LY({ NK(_Escape), _______, _______ }) },
  { 53, LY({ MO(_Alt), _______, _______ }) },
  { 54, LY({ MO(_Gui), _______, _______ }) },
  { 55, LY({ NK(_Int5), _______, _______ }) },
  { 56, SL({ LOWER }) },
  { 57, LY({ NK(_Space), _______, _______ }) },
  { 58, LY({ NK(_Space), _______, _______ }) },
  { 59, SL({ RAISE }) },
  { 60, LY({ NK(_Int4), _______ }) },
  { 61, LY({ NK(_ArrowLeft), CC(_Next), CC(_Next) }) },
  { 62, LY({ NK(_ArrowDown), CC(_VolumeUp), CC(_VolumeUp) }) },
  { 63, LY({ NK(_ArrowUp), CC(_VolumeDown), CC(_VolumeDown) }) },
  { 64, LY({ NK(_ArrowRight), CC(_PlayPause), CC(_PlayPause) }) },
};

SimulIDs_and_Command simulKeymap[] = {
  { { 51, 56, 59 }, ToH(NOP, 2000, RESET) },
};
