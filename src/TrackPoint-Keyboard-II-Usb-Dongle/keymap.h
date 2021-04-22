
#include "HidCore.h"
#include "HidEngine.h"
#include "KC.h"

using namespace hidpg;

class ScrollOrTap : public Command
{
public:
  ScrollOrTap(Command *tap_command) : _tap_command(tap_command)
  {
    _tap_command->setParent(this);
  }

  uint8_t onRelease() override
  {
    if (_isScroll == false)
    {
      _tap_command->press();
      _tap_command->release();
    }
    else
    {
      xSemaphoreTake(_mutex, portMAX_DELAY);
      _isScroll = false;
      xSemaphoreGive(_mutex);
    }
    return 1;
  }

  static void init()
  {
    _mutex = xSemaphoreCreateMutex();
  }

  static void notifyScroll()
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _isScroll = true;
    xSemaphoreGive(_mutex);
  }

private:
  Command *_tap_command;
  static SemaphoreHandle_t _mutex;
  static bool _isScroll;
};

bool ScrollOrTap::_isScroll = false;
SemaphoreHandle_t ScrollOrTap::_mutex = nullptr;

Key keymap[] = {
  { 0 /*  LeftButton    */, MS_CLK(LeftButton) },
  { 1 /*  RightButton   */, MS_CLK(RightButton) },
  { 2 /*  MiddleButton  */, new ScrollOrTap(MS_CLK(MiddleButton)) },

  { 4 /*  a             */, LY1({ NK(A), NK(F21) }) },
  { 5 /*  b             */, NK(B) },
  { 6 /*  c             */, LY1({ NK(C), NK(KeypadEqual) }) },
  { 7 /*  d             */, LY1({ NK(D), NK(F23) }) },
  { 8 /*  e             */, LY1({ NK(E), NK(F19) }) },
  { 9 /*  f             */, LY1({ NK(F), NK(F24) }) },
  { 10 /* g             */, LY1({ NK(G), NK(Pause) }) },
  { 11 /* h             */, LY1({ NK(H), NK(ArrowLeft) }) },
  { 12 /* i             */, LY1({ NK(I), NK(Home) }) },
  { 13 /* j             */, LY1({ NK(J), NK(ArrowDown) }) },
  { 14 /* k             */, LY1({ NK(K), NK(ArrowUp) }) },
  { 15 /* l             */, LY1({ NK(L), NK(ArrowRight) }) },
  { 16 /* m             */, LY1({ NK(M), NK(Application) }) },
  { 17 /* n             */, NK(N) },
  { 18 /* o             */, LY1({ NK(O), NK(End) }) },
  { 19 /* p             */, LY1({ NK(P), NK(PageUp) }) },
  { 20 /* q             */, LY1({ NK(Q), NK(F17) }) },
  { 21 /* r             */, LY1({ NK(R), NK(F20) }) },
  { 22 /* s             */, LY1({ NK(S), NK(F22) }) },
  { 23 /* t             */, NK(T) },
  { 24 /* u             */, NK(U) },
  { 25 /* v             */, LY1({ NK(V), NK(ScrollLock) }) },
  { 26 /* w             */, LY1({ NK(W), NK(F18) }) },
  { 27 /* x             */, LY1({ NK(X), NK(KeypadComma) }) },
  { 28 /* y             */, NK(Y) },
  { 29 /* z             */, LY1({ NK(Z), NK(Int6) }) },
  { 30 /* 1             */, LY1({ NK(_1), NK(F13) }) },
  { 31 /* 2             */, LY1({ NK(_2), NK(F14) }) },
  { 32 /* 3             */, LY1({ NK(_3), NK(F15) }) },
  { 33 /* 4             */, LY1({ NK(_4), NK(F16) }) },
  { 34 /* 5             */, NK(_5) },
  { 35 /* 6             */, NK(_6) },
  { 36 /* 7             */, NK(_7) },
  { 37 /* 8             */, NK(_8) },
  { 38 /* 9             */, NK(_9) },
  { 39 /* 0             */, NK(_0) },
  { 40 /* Enter         */, NK(Enter) },
  { 41 /* Escape        */, NK(Escape) },
  { 42 /* Backspace     */, NK(Backspace) },
  { 43 /* Tab           */, NK(Tab) },
  { 44 /* Space         */, NK(Space) },
  { 45 /* -             */, NK(Minus) },
  { 46 /* ^             */, NK(Equal) },
  { 47 /* @             */, NK(BracketLeft) },
  { 48 /* [             */, NK(BracketRight) },
  { 50 /* ]             */, NK(NonUsNumberSign) },
  { 51 /* ;             */, LY1({ NK(Semicolon), NK(PageDown) }) },
  { 52 /* :             */, NK(Quote) },
  { 53 /* Han/Zen       */, NK(Escape) },
  { 54 /* ,             */, NK(Comma) },
  { 55 /* .             */, NK(Period) },
  { 56 /* /             */, NK(Slash) },
  { 57 /* CapsLock      */, LY2({ TD({ { MO(Ctrl), MO(Ctrl) },
                                       { TAP(MO(Ctrl), 2), MO(Ctrl + Shift) },
                                       { TAP(MO(Ctrl), 3), MO(Ctrl + Shift + Alt) } }),
                                  MO(Ctrl) }) },
  { 58 /* F1            */, NK(F1) },
  { 59 /* F2            */, NK(F2) },
  { 60 /* F3            */, NK(F3) },
  { 61 /* F4            */, NK(F4) },
  { 62 /* F5            */, NK(F5) },
  { 63 /* F6            */, NK(F6) },
  { 64 /* F7            */, NK(F7) },
  { 65 /* F8            */, NK(F8) },
  { 66 /* F9            */, NK(F9) },
  { 67 /* F10           */, NK(F10) },
  { 68 /* F11           */, NK(F11) },
  { 69 /* F12           */, NK(F12) },
  { 70 /* PrtSc         */, NK(PrintScreen) },

  { 73 /* Insert        */, NK(Insert) },
  { 74 /* Home          */, NK(Home) },
  { 75 /* PgUp          */, NK(PageUp) },
  { 76 /* Delete        */, NK(Delete) },
  { 77 /* End           */, NK(End) },
  { 78 /* PgDn          */, NK(PageDown) },
  { 79 /* ArrowRight    */, NK(ArrowRight) },
  { 80 /* ArrowLeft     */, NK(ArrowLeft) },
  { 81 /* ArrowDown     */, NK(ArrowDown) },
  { 82 /* ArrowUp       */, NK(ArrowUp) },

  { 135 /* \(yen)       */, NK(Int1) },
  { 136 /* Kana         */, LY1({ NK(Backspace), NK(Delete) }) },
  { 137 /* \(BackSlash) */, NK(Int3) },
  { 138 /* Henkan       */, TD({ { DBL(NK(Int4), NK(Lang1)), SL1(1) },
                                 { MS_CLK(ForwardButton), MS_CLK(ForwardButton) } }) },
  { 139 /* Muhenkan     */, TD({ { DBL(NK(Int5), NK(Lang2)), SL1(1) },
                                 { MS_CLK(BackwardButton), MS_CLK(BackwardButton) } }) },

  { 200 /* LeftCtrl     */, NOP() },
  { 201 /* LeftShift    */, LY2({ TD({ { MO(Shift), MO(Shift) },
                                       { CK(Shift, CapsLock), CK(Shift, CapsLock) } }),
                                  MO(Shift) }) },
  { 202 /* LeftAlt      */, TD({ { MO(Alt), MO(Alt) },
                                 { TAP(MO(Alt), 2), MO(Alt + Shift) },
                                 { TAP(MO(Alt), 3), MO(Alt + Ctrl) } }) },
  { 203 /* LeftGui      */, MO(Gui) },
  { 204 /* RightCtrl    */, TL2(1) },
  { 205 /* RightShift   */, TD({ { MO(RightShift), MO(RightShift) },
                                 { CK(RightShift, CapsLock), CK(RightShift, CapsLock) } }) },
  { 206 /* RightAlt     */, TD({ { MO(RightAlt), MO(RightAlt) },
                                 { MO(Alt + Shift), MO(Alt + Shift) },
                                 { MO(Alt + Ctrl), MO(Alt + Ctrl) } }) },
};

Encoder encoderMap[] = {
  { 0, MS_SCR(-1, 0), MS_SCR(1, 0) },
};