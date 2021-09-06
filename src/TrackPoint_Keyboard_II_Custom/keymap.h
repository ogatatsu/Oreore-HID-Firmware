
#include "CommandDsl.h"
#include "CommandTapper.h"
#include "HidCore.h"
#include "HidEngine.h"
#include "KC.h"
#include "UsbCommandDsl.h"

using namespace hidpg;

constexpr uint8_t ENCODER_ID = 0;
constexpr uint8_t TRACK_ID_DELETE = 0;
constexpr uint8_t TRACK_ID_ARROW = 1;
constexpr uint8_t TRACK_ID_MEDIA = 2;

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

  static void begin()
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
  { 41 /* Escape        */, NK(Escape) },
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
  { 53 /* Han/Zen       */, NK(Grave) },
  { 54 /* ,             */, NK(Comma) },
  { 55 /* .             */, NK(Period) },
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
  { 76 /* Delete        */, LY({ NK(Delete), MLT({ SC(SystemSleep), WAKE_UP() }) }) },
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
  { 138 /* Henkan       */, TD({ { MLT({ NK(Int4), NK(Lang1) }), SL(1) },
                                 { MS_CLK(ForwardButton), SL(2) } },
                               true) },
  { 139 /* Muhenkan     */, TD({ { MLT({ NK(Int5), NK(Lang2) }), SL(1) },
                                 { MS_CLK(BackwardButton), SL(2) } },
                               true) },

  { 200 /* LeftCtrl     */, MLT({ SL1(1), TRC(TRACK_ID_MEDIA) }) },
  { 201 /* LeftShift    */, TD({ { NOP(), MO(Shift) },
                                 { CK(Shift, CapsLock), CK(Shift, CapsLock) } }) },
  { 202 /* LeftAlt      */, TD({ { MO(Alt), MO(Alt) },
                                 { TAP(MO(Alt), 2), MO(Alt + Shift) } }) },
  { 203 /* LeftGui      */, MO(Gui) },
  { 204 /* RightCtrl    */, TD({ { NOP(), MO(RightCtrl) },
                                 { TAP(MO(RightCtrl), 2), MO(RightCtrl + Shift) } }) },
  { 205 /* RightShift   */, TD({ { NOP(), MO(RightShift) },
                                 { CK(RightShift, CapsLock), CK(RightShift, CapsLock) } }) },
  { 206 /* RightAlt     */, TD({ { MO(RightAlt), MO(RightAlt) },
                                 { TAP(MO(RightAlt), 2), MO(RightAlt + Shift) } }) },

  { 255 /* Dongle Button */, NOP() },
};

SimulKey simulKeymap[] = {
  { { 138, 139 } /* Henkan + Muhenkan */, SL(2) }
};

Encoder encoderMap[] = {
  // { id, counterclockwise_command, clockwise_command }
  { ENCODER_ID, MS_SCR(-1, 0), MS_SCR(1, 0) },
};

Track trackMap[] = {
  // { id, threshold_distance, angle_snap, up_command, down_command, left_command, right_command }
  { TRACK_ID_DELETE, 100, AngleSnap::Enable, _______, _______, NK(Backspace), NK(Delete) },
  { TRACK_ID_ARROW, 100, AngleSnap::Enable, NK(ArrowUp), NK(ArrowDown), NK(ArrowLeft), NK(ArrowRight) },
  { TRACK_ID_MEDIA, 100, AngleSnap::Enable, CC(VolumeUp), CC(VolumeDown), _______, _______ }
};

void process_f13_f14_according_to_layer_change(layer_bitmap_t prev_state, layer_bitmap_t state)
{
  static bool f13_state = false;

  if (bitRead(prev_state, 2) == 0 && bitRead(state, 2) == 1) // 2がon
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
  else if (bitRead(prev_state, 2) == 1 && bitRead(state, 2) == 0) // 2がoff
  {
    // F14を解除
    Hid.unsetKey(F14);

    // 1がonだったらF13を入力
    if (bitRead(state, 1) == 1 && f13_state == false)
    {
      Hid.setKey(F13);
      f13_state = true;
    }
    Hid.sendKeyReport(false);
  }

  if (bitRead(prev_state, 1) == 0 && bitRead(state, 1) == 1) // 1がon
  {
    // 2がoffだったらF13を入力
    if (bitRead(state, 2) == 0 && f13_state == false)
    {
      Hid.setKey(F13);
      Hid.sendKeyReport(false);
      f13_state = true;
    }
  }
  else if (bitRead(prev_state, 1) == 1 && bitRead(state, 1) == 0) // 1がoff
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

void process_trc_according_to_layer_change(layer_bitmap_t prev_state, layer_bitmap_t state)
{
  static TrackID trc_del(TRACK_ID_DELETE);
  static TrackID trc_arrow(TRACK_ID_ARROW);

  static bool trc_del_state = false;

  if (bitRead(prev_state, 2) == 0 && bitRead(state, 2) == 1) // 2がon
  {
    HidEngine.startTracking(trc_arrow);
  }
  else if (bitRead(prev_state, 2) == 1 && bitRead(state, 2) == 0) // 2がoff
  {
    HidEngine.stopTracking(trc_arrow);

    // 1がonだったらtrc_delをON
    if (bitRead(state, 1) == 1 && trc_del_state == false)
    {
      HidEngine.startTracking(trc_del);
      trc_del_state = true;
    }
  }

  if (bitRead(prev_state, 1) == 0 && bitRead(state, 1) == 1) // 1がon
  {
    // Layer2がoffだったらtrc_delをON
    if (bitRead(state, 2) == 0 && trc_del_state == false)
    {
      HidEngine.startTracking(trc_del);
      trc_del_state = true;
    }
  }
  else if (bitRead(prev_state, 1) == 1 && bitRead(state, 1) == 0) // 1がoff
  {
    // trc_delがONの場合解除する
    if (trc_del_state == true)
    {
      HidEngine.stopTracking(trc_del);
      trc_del_state = false;
    }
  }
}

void layer_change_state_callback(layer_bitmap_t prev_state, layer_bitmap_t state)
{
  process_f13_f14_according_to_layer_change(prev_state, state);
  process_trc_according_to_layer_change(prev_state, state);
}
