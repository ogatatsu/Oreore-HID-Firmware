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
    {1, L({NK(_Grave), CK(_Shift, _Grave), NK(_Grave)})},
    {2, L({NK(_1), ToH(CK(_Shift, _1), 1500, BT(1)), NK(_1)})},
    {3, L({NK(_2), ToH(CK(_Shift, _2), 1500, BT(2)), NK(_2)})},
    {4, L({NK(_3), ToH(CK(_Shift, _3), 1500, BT(3)), NK(_3)})},
    {5, L({NK(_4), CK(_Shift, _4), NK(_4)})},
    {6, L({NK(_5), CK(_Shift, _5), NK(_5)})},
    {7, L({NK(_6), CK(_Shift, _6), NK(_6)})},
    {8, L({NK(_7), CK(_Shift, _7), NK(_7)})},
    {9, L({NK(_8), CK(_Shift, _8), NK(_8)})},
    {10, L({NK(_9), CK(_Shift, _9), NK(_9)})},
    {11, L({NK(_0), CK(_Shift, _0), NK(_0)})},
    {12, L({NK(_Delete), _______, NK(_Backspace)})},

    {13, L({NK(_Tab), CK(_Shift, _Grave), NK(_Grave)})},
    {14, L({NK(_Q), CK(_Shift, _1), NK(_1)})},
    {15, L({NK(_W), CK(_Shift, _2), NK(_2)})},
    {16, L({NK(_E), CK(_Shift, _3), NK(_3)})},
    {17, L({NK(_R), CK(_Shift, _4), NK(_4)})},
    {18, L({NK(_T), CK(_Shift, _5), NK(_5)})},
    {19, L({NK(_Y), CK(_Shift, _6), NK(_6)})},
    {20, L({NK(_U), CK(_Shift, _7), NK(_7)})},
    {21, L({NK(_I), CK(_Shift, _8), NK(_8)})},
    {22, L({NK(_O), CK(_Shift, _9), NK(_9)})},
    {23, L({NK(_P), CK(_Shift, _0), NK(_0)})},
    {24, L({NK(_Backspace), _______, NK(_Delete)})},

    {25, L({MO(_Ctrl), _______, _______})},
    {26, L({NK(_A), NK(_F1), NK(_F1)})},
    {27, L({NK(_S), NK(_F2), NK(_F2)})},
    {28, L({NK(_D), NK(_F3), NK(_F3)})},
    {29, L({NK(_F), NK(_F4), NK(_F4)})},
    {30, L({NK(_G), NK(_F5), NK(_F5)})},
    {31, L({NK(_H), NK(_F6), NK(_F6)})},
    {32, L({NK(_J), CK(_Shift, _Minus), NK(_Minus)})},
    {33, L({NK(_K), CK(_Shift, _Equal), NK(_Equal)})},
    {34, L({NK(_L), CK(_Shift, _BracketLeft), NK(_BracketLeft)})},
    {35, L({NK(_Semicolon), CK(_Shift, _BracketRight), NK(_BracketRight)})},
    {36, L({NK(_Quote), CK(_Shift, _Backslash), NK(_Backslash)})},

    {37, L({MO(_Shift), _______, _______})},
    {38, L({NK(_Z), NK(_F7), NK(_F7)})},
    {39, L({NK(_X), NK(_F8), NK(_F8)})},
    {40, L({NK(_C), NK(_F9), NK(_F9)})},
    {41, L({NK(_V), NK(_F10), NK(_F10)})},
    {42, L({NK(_B), NK(_F11), NK(_F11)})},
    {43, L({NK(_BracketLeft), CK(_Shift, _9), _______})},
    {44, L({NK(_BracketRight), CK(_Shift, _0), _______})},
    {45, L({NK(_N), NK(_F12), NK(_F12)})},
    {46, L({NK(_M), _______, _______})},
    {47, L({NK(_Comma), _______, _______})},
    {48, L({NK(_Period), NK(_Home), NK(_PageDown)})},
    {49, L({NK(_Slash), NK(_End), NK(_PageUp)})},
    {50, L({NK(_Enter), _______, _______})},

    {51, NOP},
    {52, L({NK(_Escape), _______, _______})},
    {53, L({MO(_Alt), _______, _______})},
    {54, L({MO(_Gui), _______, _______})},
    {55, L({NK(_Int5), _______, _______})},
    {56, SL({LOWER})},
    {57, L({NK(_Space), _______, _______})},
    {58, L({NK(_Space), _______, _______})},
    {59, SL({RAISE})},
    {60, L({NK(_Int4), _______})},
    {61, L({NK(_ArrowLeft), CC(_Next), CC(_Next)})},
    {62, L({NK(_ArrowDown), CC(_VolumeUp), CC(_VolumeUp)})},
    {63, L({NK(_ArrowUp), CC(_VolumeDown), CC(_VolumeDown)})},
    {64, L({NK(_ArrowRight), CC(_PlayPause), CC(_PlayPause)})},
};

SimulIDs_and_Command simulKeymap[] = {
    {{51, 56, 59}, ToH(NOP, 2000, RESET)},
};
