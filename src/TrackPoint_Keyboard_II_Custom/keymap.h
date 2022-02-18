
#include "CommandDsl.h"
#include "CommandTapper.h"
#include "HidEngine.h"
#include "KC.h"
#include "UsbCommandDsl.h"

using namespace hidpg;

constexpr uint8_t MOUSE_ID = 0;
constexpr uint8_t GESTURE_ID_TAB = 0;
constexpr uint8_t GESTURE_ID_ARROW = 1;
constexpr uint8_t GESTURE_ID_DESKTOP = 2;
constexpr uint8_t ENCODER_ID = 0;

//------------------------------------------------------------------+
// CustomCommand
//------------------------------------------------------------------+

class EncoderAndLayer1OrTap : public Command, public BdrcpEventListener
{
public:
  EncoderAndLayer1OrTap(Command *tap_command)
      : BdrcpEventListener(this), _tap_command(tap_command), _hasDifferentCommandPressed(false)
  {
    _tap_command->setParent(this);
  }

  void onPress(uint8_t n_times) override
  {
    Layer1.on(1);
    startListen_BeforeDifferentRootCommandPress();
    _hasDifferentCommandPressed = false;
  }

  uint8_t onRelease() override
  {
    Layer1.off(1);
    stopListen_BeforeDifferentRootCommandPress();

    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (_hasScrolled == false && _hasDifferentCommandPressed == false)
    {
      CommandTapper.tap(_tap_command);
    }
    else
    {
      _hasScrolled = false;
    }
    xSemaphoreGive(_mutex);

    return 1;
  }

  void onBeforeDifferentRootCommandPress()
  {
    _hasDifferentCommandPressed = true;
  }

  static void begin()
  {
    _mutex = xSemaphoreCreateMutexStatic(&_mutex_buffer);
  }

  static void notifyScroll()
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _hasScrolled = true;
    xSemaphoreGive(_mutex);
  }

private:
  Command *_tap_command;
  bool _hasDifferentCommandPressed;
  static SemaphoreHandle_t _mutex;
  static StaticSemaphore_t _mutex_buffer;
  static bool _hasScrolled;
};

bool EncoderAndLayer1OrTap::_hasScrolled = false;
SemaphoreHandle_t EncoderAndLayer1OrTap::_mutex = nullptr;
StaticSemaphore_t EncoderAndLayer1OrTap::_mutex_buffer;

EncoderAndLayer1OrTap EncoderAndLayer1OrMiddleButton_(MS_CLK(MiddleButton));
Command *EncoderAndLayer1OrMiddleButton = &EncoderAndLayer1OrMiddleButton_;

//------------------------------------------------------------------+
// Keymap
//------------------------------------------------------------------+

Key keymap[] = {
  { 0 /*  LeftButton    */, LY({ MS_CLK(LeftButton), MS_CLK(BackwardButton) }) },
  { 1 /*  RightButton   */, LY({ MS_CLK(RightButton), MS_CLK(ForwardButton) }) },
  { 2 /*  MiddleButton  */, EncoderAndLayer1OrMiddleButton },

  { 4 /*  a             */, NK(A) },
  { 5 /*  b             */, NK(B) },
  { 6 /*  c             */, NK(C) },
  { 7 /*  d             */, NK(D) },
  { 8 /*  e             */, NK(E) },
  { 9 /*  f             */, NK(F) },
  { 10 /* g             */, NK(G) },
  { 11 /* h             */, LY1({ LY({ NK(H), NK(Left) }), CC(PrevTrack) }) },
  { 12 /* i             */, LY({ NK(I), NK(Backspace) }) },
  { 13 /* j             */, LY1({ LY({ NK(J), NK(Down) }), CC(VolumeDown) }) },
  { 14 /* k             */, LY1({ LY({ NK(K), NK(Up) }), CC(VolumeUp) }) },
  { 15 /* l             */, LY1({ LY({ NK(L), NK(Right) }), CC(NextTrack) }) },
  { 16 /* m             */, LY1({ LY({ NK(M), NK(Application) }), CC(Mute) }) },
  { 17 /* n             */, NK(N) },
  { 18 /* o             */, LY({ NK(O), NK(Delete) }) },
  { 19 /* p             */, LY1({ LY({ NK(P), NK(PageUp) }), CC(LightUp) }) },
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
  { 30 /* 1             */, LY({ NK(_1), NK(F1) }) },
  { 31 /* 2             */, LY({ NK(_2), NK(F2) }) },
  { 32 /* 3             */, LY({ NK(_3), NK(F3) }) },
  { 33 /* 4             */, LY({ NK(_4), NK(F4) }) },
  { 34 /* 5             */, LY({ NK(_5), NK(F5) }) },
  { 35 /* 6             */, LY({ NK(_6), NK(F6) }) },
  { 36 /* 7             */, LY({ NK(_7), NK(F7) }) },
  { 37 /* 8             */, LY({ NK(_8), NK(F8) }) },
  { 38 /* 9             */, LY({ NK(_9), NK(F9) }) },
  { 39 /* 0             */, LY({ NK(_0), NK(F10) }) },
  { 40 /* Enter         */, MLT({ NK(Enter), WAKE_UP() }) },
  { 41 /* Escape        */, NK(Escape) },
  { 42 /* Backspace     */, NK(Backspace) },
  { 43 /* Tab           */, NK(Tab) },
  { 44 /* Space         */, LY1({ LY({ NK(Space), NK(Enter) }), CC(PlayPause) }) },
  { 45 /* -             */, LY({ NK(Minus), NK(F11) }) },
  { 46 /* ^             */, LY({ NK(Equal), NK(F12) }) },
  { 47 /* @             */, NK(LeftBracket) },
  { 48 /* [             */, NK(RightBracket) },
  { 50 /* ]             */, NK(NonUsNumberSign) },
  { 51 /* ;             */, LY1({ LY({ NK(Semicolon), NK(PageDown) }), CC(LightDown) }) },
  { 52 /* :             */, NK(Quote) },
  { 53 /* Han/Zen       */, NK(Grave) },
  { 54 /* ,             */, LY({ NK(Comma), NK(Home) }) },
  { 55 /* .             */, LY({ NK(Period), NK(End) }) },
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
  { 79 /* Right         */, NK(Right) },
  { 80 /* Left          */, NK(Left) },
  { 81 /* Down          */, NK(Down) },
  { 82 /* Up            */, NK(Up) },

  { 135 /* \(yen)       */, NK(Int1) },
  { 136 /* Kana         */, TDDM({ { MO(RightGui), MLT({ MO(RightGui), GST(GESTURE_ID_ARROW) }) },
                                   { NOP(), MLT({ MO(RightGui + Shift), GST(GESTURE_ID_ARROW) }) } },
                                 { MOUSE_ID }) },
  { 137 /* \(BackSlash) */, NK(Int3) },
  { 138 /* Henkan       */, TDDM({ { MLT({ NK(Int4), NK(Lang1) }), MLT({ SL(1), NK(F24), GST(GESTURE_ID_TAB) }) },
                                   { NOP(), MLT({ SL(1), MO(Alt), TAP(NK(Tab)), GST(GESTURE_ID_ARROW) }) },
                                   { CK(Gui, Tab), MLT({ SL(1), TAP(CK(Gui, Tab)), GST(GESTURE_ID_DESKTOP), TAP_R(NK(Escape)) }) } },
                                 { MOUSE_ID }) },
  { 139 /* Muhenkan     */, TDDM({ { MLT({ NK(Int5), NK(Lang2) }), MLT({ SL(1), NK(F24), GST(GESTURE_ID_TAB) }) },
                                   { NOP(), MLT({ SL(1), MO(Alt), TAP(CK(Shift, Tab)), GST(GESTURE_ID_ARROW) }) },
                                   { CK(Gui, Tab), MLT({ SL(1), TAP(CK(Gui, Tab)), GST(GESTURE_ID_DESKTOP), TAP_R(NK(Escape)) }) } },
                                 { MOUSE_ID }) },

  { 200 /* LeftCtrl     */, TD({ { NOP(), MO(Ctrl + Alt) },
                                 { NOP(), MO(Ctrl + Alt + Shift) } }) },
  { 201 /* LeftShift    */, TD({ { MO(Shift), MO(Shift) },
                                 { CK(Shift, CapsLock), CK(Shift, CapsLock) } }) },
  { 202 /* LeftAlt      */, TD({ { MO(Alt), MO(Alt) },
                                 { TAP(MO(Alt), 2), MO(Alt + Shift) } }) },
  { 203 /* LeftGui      */, TDDM({ { MO(Gui), MLT({ MO(Gui), GST(GESTURE_ID_ARROW) }) },
                                   { NOP(), MLT({ MO(Gui + Shift), GST(GESTURE_ID_ARROW) }) } },
                                 { MOUSE_ID }) },
  { 204 /* RightCtrl    */, TD({ { MO(RightCtrl), MO(RightCtrl) },
                                 { TAP(MO(RightCtrl), 2), MO(RightCtrl + Shift) } }) },
  { 205 /* RightShift   */, TD({ { MO(RightShift), MO(RightShift) },
                                 { CK(RightShift, CapsLock), CK(RightShift, CapsLock) } }) },
  { 206 /* RightAlt     */, TD({ { MO(RightAlt), MO(RightAlt) },
                                 { TAP(MO(RightAlt), 2), MO(RightAlt + Shift) } }) },

  { 255 /* Dongle Button */, NOP() },
};

Gesture gestureMap[] = {
  // { gesture_id, mouse_id, distance, angle_snap, up_command, down_command, left_command, right_command }
  { GESTURE_ID_TAB, MOUSE_ID, 15, AngleSnap::Enable, _______, _______, TS(250, CK(Ctrl + Shift, Tab)), TS(250, CK(Ctrl, Tab)) },
  { GESTURE_ID_ARROW, MOUSE_ID, 15, AngleSnap::Enable, TS(250, NK(Up)), TS(250, NK(Down)), TS(250, NK(Left)), TS(250, NK(Right)) },
  { GESTURE_ID_DESKTOP, MOUSE_ID, 15, AngleSnap::Enable, _______, _______, TS(250, CK(Gui + Ctrl, Left)), TS(250, CK(Gui + Ctrl, Right)) },
};

Encoder encoderMap[] = {
  // { encoder_id, counterclockwise_command, clockwise_command }
  { ENCODER_ID, LY({ MS_SCR(-1, 0), MS_SCR(0, 1) }), LY({ MS_SCR(1, 0), MS_SCR(0, -1) }) },
};
