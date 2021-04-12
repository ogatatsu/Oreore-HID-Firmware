
#include "HidCore.h"
#include "HidEngine.h"
#include "KC.h"

using namespace hidpg;

class ScrollOrTap : public Command
{
public:
  ScrollOrTap(Command *cmd) : _cmd(cmd)
  {
    _cmd->setParent(this);
  }

  uint8_t onRelease() override
  {
    if (_isScroll == false)
    {
      _cmd->press();
      _cmd->release();
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
  Command *_cmd;
  static SemaphoreHandle_t _mutex;
  static bool _isScroll;
};

bool ScrollOrTap::_isScroll = false;
SemaphoreHandle_t ScrollOrTap::_mutex = nullptr;


Key keymap[] = {
  { 0 /*  LeftButton    */, MS_CLK(LeftButton) },
  { 1 /*  RightButton   */, MS_CLK(RightButton) },
  { 2 /*  MiddleButton  */, new ScrollOrTap(MS_CLK(MiddleButton)) },

  { 4 /*  a             */, NK(A) },
  { 5 /*  b             */, NK(B) },
  { 6 /*  c             */, NK(C) },
  { 7 /*  d             */, NK(D) },
  { 8 /*  e             */, NK(E) },
  { 9 /*  f             */, NK(F) },
  { 10 /* g             */, NK(G) },
  { 11 /* h             */, LY({ NK(H), NK(ArrowLeft) }) },
  { 12 /* i             */, NK(I) },
  { 13 /* j             */, LY({ NK(J), NK(ArrowDown) }) },
  { 14 /* k             */, LY({ NK(K), NK(ArrowUp) }) },
  { 15 /* l             */, LY({ NK(L), NK(ArrowRight) }) },
  { 16 /* m             */, NK(M) },
  { 17 /* n             */, NK(N) },
  { 18 /* o             */, NK(O) },
  { 19 /* p             */, NK(P) },
  { 20 /* q             */, NK(Q) },
  { 21 /* r             */, NK(R) },
  { 22 /* s             */, NK(S) },
  { 23 /* t             */, NK(T) },
  { 24 /* u             */, NK(U) },
  { 25 /* v             */, NK(V) },
  { 26 /* w             */, NK(W) },
  { 27 /* x             */, NK(X) },
  { 28 /* y             */, NK(Y) },
  { 29 /* z             */, NK(Z) },
  { 30 /* 1             */, NK(_1) },
  { 31 /* 2             */, NK(_2) },
  { 32 /* 3             */, NK(_3) },
  { 33 /* 4             */, NK(_4) },
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
  { 50 /* [             */, NK(NonUsNumberSign) },
  { 51 /* ;             */, NK(Semicolon) },
  { 52 /* :             */, NK(Quote) },
  { 53 /* Han/Zen       */, NK(Grave) },
  { 54 /* ,             */, NK(Comma) },
  { 55 /* .             */, NK(Period) },
  { 56 /* /             */, NK(Slash) },
  { 57 /* CapsLock      */, TD({ { MO(Ctrl), MO(Ctrl) },
                                 { MO(Ctrl + Shift), MO(Ctrl + Shift) },
                                 { MO(Ctrl + Shift + Alt), MO(Ctrl + Shift + Alt) } }) },
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
  { 136 /* Kana         */, TD({ { MO(RightCtrl), MO(RightCtrl) },
                                 { MO(Ctrl + Shift), MO(Ctrl + Shift) },
                                 { MO(Ctrl + Shift + Alt), MO(Ctrl + Shift + Alt) } }) },
  { 137 /* \(BackSlash) */, NK(Int3) },
  { 138 /* Henkan       */, LT(1, DBL(NK(Int4), NK(Lang1))) },
  { 139 /* Muhenkan     */, LT(1, DBL(NK(Int5), NK(Lang2))) },

  { 200 /* LeftCtrl     */, NK(F13) },
  { 201 /* LeftShift    */, TD({ { MO(Shift), MO(Shift) },
                                 { CK(Shift, CapsLock), CK(Shift, CapsLock) } }) },
  { 202 /* LeftAlt      */, TD({ { MO(Alt), MO(Alt) },
                                 { MO(Alt + Shift), MO(Alt + Shift) },
                                 { MO(Alt + Ctrl), MO(Alt + Ctrl) } }) },
  { 203 /* LeftGui      */, MO(Gui) },
  { 204 /* RightCtrl    */, NK(F14) },
  { 205 /* RightShift   */, TD({ { MO(RightShift), MO(RightShift) },
                                 { CK(RightShift, CapsLock), CK(RightShift, CapsLock) } }) },
  { 206 /* RightAlt     */, TD({ { MO(RightAlt), MO(RightAlt) },
                                 { MO(Alt + Shift), MO(Alt + Shift) },
                                 { MO(Alt + Ctrl), MO(Alt + Ctrl) } }) },
};

Encoder encoderMap[] = {
  { 0, MS_SCR(-1, 0), MS_SCR(1, 0) },
};
