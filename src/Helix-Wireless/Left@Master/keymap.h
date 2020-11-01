#include "BleCommand.h"
#include "HidEngine.h"
#include "KC.h"

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

Key keymap[] = {
  { 1, LY({ NK(Grave), CK(Shift, Grave), NK(Grave) }) },
  { 2, LY({ NK(_1), ToH(CK(Shift, _1), 1500, BT(1)), NK(_1) }) },
  { 3, LY({ NK(_2), ToH(CK(Shift, _2), 1500, BT(2)), NK(_2) }) },
  { 4, LY({ NK(_3), ToH(CK(Shift, _3), 1500, BT(3)), NK(_3) }) },
  { 5, LY({ NK(_4), CK(Shift, _4), NK(_4) }) },
  { 6, LY({ NK(_5), CK(Shift, _5), NK(_5) }) },
  { 7, LY({ NK(_6), CK(Shift, _6), NK(_6) }) },
  { 8, LY({ NK(_7), CK(Shift, _7), NK(_7) }) },
  { 9, LY({ NK(_8), CK(Shift, _8), NK(_8) }) },
  { 10, LY({ NK(_9), CK(Shift, _9), NK(_9) }) },
  { 11, LY({ NK(_0), CK(Shift, _0), NK(_0) }) },
  { 12, LY({ NK(Delete), _______, NK(Backspace) }) },

  { 13, LY({ NK(Tab), CK(Shift, Grave), NK(Grave) }) },
  { 14, LY({ NK(Q), CK(Shift, _1), NK(_1) }) },
  { 15, LY({ NK(W), CK(Shift, _2), NK(_2) }) },
  { 16, LY({ NK(E), CK(Shift, _3), NK(_3) }) },
  { 17, LY({ NK(R), CK(Shift, _4), NK(_4) }) },
  { 18, LY({ NK(T), CK(Shift, _5), NK(_5) }) },
  { 19, LY({ NK(Y), CK(Shift, _6), NK(_6) }) },
  { 20, LY({ NK(U), CK(Shift, _7), NK(_7) }) },
  { 21, LY({ NK(I), CK(Shift, _8), NK(_8) }) },
  { 22, LY({ NK(O), CK(Shift, _9), NK(_9) }) },
  { 23, LY({ NK(P), CK(Shift, _0), NK(_0) }) },
  { 24, LY({ NK(Backspace), _______, NK(Delete) }) },

  { 25, LY({ MO(Ctrl), _______, _______ }) },
  { 26, LY({ NK(A), NK(F1), NK(F1) }) },
  { 27, LY({ NK(S), NK(F2), NK(F2) }) },
  { 28, LY({ NK(D), NK(F3), NK(F3) }) },
  { 29, LY({ NK(F), NK(F4), NK(F4) }) },
  { 30, LY({ NK(G), NK(F5), NK(F5) }) },
  { 31, LY({ NK(H), NK(F6), NK(F6) }) },
  { 32, LY({ NK(J), CK(Shift, Minus), NK(Minus) }) },
  { 33, LY({ NK(K), CK(Shift, Equal), NK(Equal) }) },
  { 34, LY({ NK(L), CK(Shift, BracketLeft), NK(BracketLeft) }) },
  { 35, LY({ NK(Semicolon), CK(Shift, BracketRight), NK(BracketRight) }) },
  { 36, LY({ NK(Quote), CK(Shift, Backslash), NK(Backslash) }) },

  { 37, LY({ MO(Shift), _______, _______ }) },
  { 38, LY({ NK(Z), NK(F7), NK(F7) }) },
  { 39, LY({ NK(X), NK(F8), NK(F8) }) },
  { 40, LY({ NK(C), NK(F9), NK(F9) }) },
  { 41, LY({ NK(V), NK(F10), NK(F10) }) },
  { 42, LY({ NK(B), NK(F11), NK(F11) }) },
  { 43, LY({ NK(BracketLeft), CK(Shift, _9), _______ }) },
  { 44, LY({ NK(BracketRight), CK(Shift, _0), _______ }) },
  { 45, LY({ NK(N), NK(F12), NK(F12) }) },
  { 46, LY({ NK(M), _______, _______ }) },
  { 47, LY({ NK(Comma), _______, _______ }) },
  { 48, LY({ NK(Period), NK(Home), NK(PageDown) }) },
  { 49, LY({ NK(Slash), NK(End), NK(PageUp) }) },
  { 50, LY({ NK(Enter), _______, _______ }) },

  { 51, NOP() },
  { 52, LY({ NK(Escape), _______, _______ }) },
  { 53, LY({ MO(Alt), _______, _______ }) },
  { 54, LY({ MO(Gui), _______, _______ }) },
  { 55, LY({ NK(Int5), _______, _______ }) },
  { 56, SL({ LOWER }) },
  { 57, LY({ NK(Space), _______, _______ }) },
  { 58, LY({ NK(Space), _______, _______ }) },
  { 59, SL({ RAISE }) },
  { 60, LY({ NK(Int4), _______ }) },
  { 61, LY({ NK(ArrowLeft), CC(Next), CC(Next) }) },
  { 62, LY({ NK(ArrowDown), CC(VolumeUp), CC(VolumeUp) }) },
  { 63, LY({ NK(ArrowUp), CC(VolumeDown), CC(VolumeDown) }) },
  { 64, LY({ NK(ArrowRight), CC(PlayPause), CC(PlayPause) }) },
};

SimulKey simul_keymap[] = {
  { { 51, 56, 59 }, ToH(NOP(), 2000, RESET()) },
};
