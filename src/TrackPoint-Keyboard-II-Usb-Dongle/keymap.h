
#include "CommandTapper.h"
#include "HidCore.h"
#include "HidEngine.h"
#include "KC.h"

using namespace hidpg;

//------------------------------------------------------------------+
// CustomCommand
//------------------------------------------------------------------+

class ScrollOrTap : public Command
{
public:
  ScrollOrTap(Command *tap_command) : _tap_command(tap_command)
  {
    _tap_command->setParent(this);
  }

  uint8_t onRelease() override
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (_isScroll == false)
    {
      CommandTapper.tap(_tap_command);
    }
    else
    {
      _isScroll = false;
    }
    xSemaphoreGive(_mutex);

    return 1;
  }

  static void init()
  {
    _mutex = xSemaphoreCreateMutexStatic(&_mutex_buffer);
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
  static StaticSemaphore_t _mutex_buffer;
  static bool _isScroll;
};

bool ScrollOrTap::_isScroll = false;
SemaphoreHandle_t ScrollOrTap::_mutex = nullptr;
StaticSemaphore_t ScrollOrTap::_mutex_buffer;

ScrollOrTap ScrollOrMiddleButton(MS_CLK(MiddleButton));

//------------------------------------------------------------------+
// Keymap
//------------------------------------------------------------------+

Key keymap[] = {
  { 0 /*  LeftButton    */, MS_CLK(LeftButton) },
  { 1 /*  RightButton   */, MS_CLK(RightButton) },
  { 2 /*  MiddleButton  */, &ScrollOrMiddleButton },

  { 4 /*  a             */, NK(A) },
  { 5 /*  b             */, NK(B) },
  { 6 /*  c             */, NK(C) },
  { 7 /*  d             */, NK(D) },
  { 8 /*  e             */, NK(E) },
  { 9 /*  f             */, NK(F) },
  { 10 /* g             */, NK(G) },
  { 11 /* h             */, LY1({ LY({ NK(H), NK(ArrowLeft), NK(Backspace) }),
                                  CC(PrevTrack) }) },
  { 12 /* i             */, LY({ NK(I), NK(Home) }) },
  { 13 /* j             */, LY1({ LY({ NK(J), NK(ArrowDown) }),
                                  CC(VolumeDown) }) },
  { 14 /* k             */, LY1({ LY({ NK(K), NK(ArrowUp) }),
                                  CC(VolumeUp) }) },
  { 15 /* l             */, LY1({ LY({ NK(L), NK(ArrowRight), NK(Delete) }),
                                  CC(NextTrack) }) },
  { 16 /* m             */, LY1({ LY({ NK(M), NK(Application) }),
                                  CC(Mute) }) },
  { 17 /* n             */, NK(N) },
  { 18 /* o             */, LY({ NK(O), NK(End) }) },
  { 19 /* p             */, LY({ NK(P), NK(PageUp) }) },
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
  { 41 /* Escape        */, MLT({ TRC(2), RD_CLK() }) },
  { 42 /* Backspace     */, NK(Backspace) },
  { 43 /* Tab           */, NK(Tab) },
  { 44 /* Space         */, LY1({ NK(Space), CC(PlayPause) }) },
  { 45 /* -             */, NK(Minus) },
  { 46 /* ^             */, NK(Equal) },
  { 47 /* @             */, NK(BracketLeft) },
  { 48 /* [             */, NK(BracketRight) },
  { 50 /* ]             */, NK(NonUsNumberSign) },
  { 51 /* ;             */, LY({ NK(Semicolon), NK(PageDown) }) },
  { 52 /* :             */, NK(Quote) },
  { 53 /* Han/Zen       */, TRC(2) },
  { 54 /* ,             */, LY1({ NK(Comma), CC(Rewind) }) },
  { 55 /* .             */, LY1({ NK(Period), CC(FastForward) }) },
  { 56 /* /             */, NK(Slash) },
  { 57 /* CapsLock      */, TD({ { NK(Escape), MO(Ctrl) },
                                 { TAP(MO(Ctrl), 2), MO(Ctrl + Shift) } }) },
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
  { 70 /* PrtSc         */, TD({ { NK(PrintScreen), NK(PrintScreen) },
                                 { CK(Gui + Shift, S), CK(Gui + Shift, S) } }) },
  { 73 /* Insert        */, NK(Insert) },
  { 74 /* Home          */, NK(Home) },
  { 75 /* PgUp          */, NK(PageUp) },
  { 76 /* Delete        */, LY({ NK(Delete), SC(SystemSleep) }) },
  { 77 /* End           */, NK(End) },
  { 78 /* PgDn          */, NK(PageDown) },
  { 79 /* ArrowRight    */, NK(ArrowRight) },
  { 80 /* ArrowLeft     */, NK(ArrowLeft) },
  { 81 /* ArrowDown     */, NK(ArrowDown) },
  { 82 /* ArrowUp       */, NK(ArrowUp) },

  { 135 /* \(yen)       */, NK(Int1) },
  { 136 /* Kana         */, TD({ { CC(LaunchApp1), CC(LaunchApp1) },
                                 { CC(LaunchApp2), CC(LaunchApp2) },
                                 { CC(LaunchMail), CC(LaunchMail) },
                                 { CC(LaunchMedia), CC(LaunchMedia) } }) },
  { 137 /* \(BackSlash) */, NK(Int3) },
  { 138 /* Henkan       */, TD({ { MLT({ NK(Int4), NK(Lang1) }), MLT({ SL(1), TRC(0) }) },
                                 { MS_CLK(ForwardButton), MLT({ SL(2), TRC(1) }) } },
                               true) },
  { 139 /* Muhenkan     */, TD({ { MLT({ NK(Int5), NK(Lang2) }), MLT({ SL(1), TRC(0) }) },
                                 { MS_CLK(BackwardButton), MLT({ SL(2), TRC(1) }) } },
                               true) },

  { 200 /* LeftCtrl     */, MLT({ SL1(1), TRC(3) }) },
  { 201 /* LeftShift    */, TD({ { MO(Shift), MO(Shift) },
                                 { CK(Shift, CapsLock), CK(Shift, CapsLock) } }) },
  { 202 /* LeftAlt      */, TD({ { MO(Alt), MO(Alt) },
                                 { TAP(MO(Alt), 2), MO(Alt + Shift) } }) },
  { 203 /* LeftGui      */, MO(Gui) },
  { 204 /* RightCtrl    */, TD({ { MO(RightCtrl), MO(RightCtrl) },
                                 { TAP(MO(RightCtrl), 2), MO(RightCtrl + Shift) } }) },
  { 205 /* RightShift   */, TD({ { MO(RightShift), MO(RightShift) },
                                 { CK(RightShift, CapsLock), CK(RightShift, CapsLock) } }) },
  { 206 /* RightAlt     */, TD({ { MO(RightAlt), MO(RightAlt) },
                                 { TAP(MO(RightAlt), 2), MO(RightAlt + Shift) } }) },

  { 255 /* Dongle Button */, NOP() },
};

Encoder encoderMap[] = {
  { 0, MS_SCR(-1, 0), MS_SCR(1, 0) },
};

Track trackMap[] = {
  { 0, 100, AngleSnap::Enable, _______, _______, NK(Backspace), NK(Delete) },
  { 1, 100, AngleSnap::Enable, NK(ArrowUp), NK(ArrowDown), NK(ArrowLeft), NK(ArrowRight) },
  { 2, 1, AngleSnap::Enable, RD_ROT(-1), RD_ROT(1), RD_ROT(-1), RD_ROT(1) },
  { 3, 100, AngleSnap::Enable, CC(VolumeUp), CC(VolumeDown), _______, _______ }
};

SimulKey simulKeymap[] = {
  { { 138, 139 }, MLT({ SL(2), TRC(1) }) }
};

void layer_change_state_callback(layer_bitmap_t prev_state, layer_bitmap_t state)
{
  static bool f13_state = false;

  if (bitRead(prev_state, 2) == 0 && bitRead(state, 2) == 1) // Layer2がonになった
  {
    // F13が入力されている場合解除してからF14を入力
    if (f13_state == true)
    {
      Hid.unsetKey(F13);
      f13_state = false;
    }
    Hid.setKey(F14);
    Hid.sendKeyReport(false);
  }
  else if (bitRead(prev_state, 2) == 1 && bitRead(state, 2) == 0) // Layer2がoffになった
  {
    // F14を解除
    Hid.unsetKey(F14);

    // Layer1がonだったらF13を入力
    if (bitRead(state, 1) == 1)
    {
      Hid.setKey(F13);
      f13_state = true;
    }
    Hid.sendKeyReport(false);
  }

  if (bitRead(prev_state, 1) == 0 && bitRead(state, 1) == 1) // Layer1がonになった
  {
    // Layer2がoffだったらF13を入力
    if (bitRead(state, 2) == 0)
    {
      Hid.setKey(F13);
      Hid.sendKeyReport(false);
      f13_state = true;
    }
  }
  else if (bitRead(prev_state, 1) == 1 && bitRead(state, 1) == 0) // Layer1がoffになった
  {
    // F13が入力されている場合解除する
    if (f13_state == true)
    {
      Hid.unsetKey(F13);
      Hid.sendKeyReport(false);
      f13_state = false;
    }
  }
}
