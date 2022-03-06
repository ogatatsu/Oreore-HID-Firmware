
#include "Arduino.h"
#include "BleCommandDsl.h"
#include "CommandDsl.h"
#include "CommandTapper.h"
#include "HidCore.h"
#include "HidEngine.h"
#include "KC.h"
#include "UsbCommandDsl.h"

using namespace hidpg;

constexpr PointingDeviceId PdTrackpoint{0};
constexpr PointingDeviceId PdLift{1};

constexpr EncoderId EncTrackpoint{0};
constexpr EncoderId EncLiftWheel{1};
constexpr EncoderId EncLiftPan{2};

constexpr KeyShiftId KeyShiftRaise{0};
constexpr KeyShiftId KeyShiftLower{1};
constexpr KeyShiftId KeyShiftTaskSwitch{2};

constexpr GestureId GstTabSwitch{0};
constexpr GestureId GstArrow{1};
constexpr GestureId GstMiddleButton{2};

constexpr EncoderShiftId EncShiftPan{0};
constexpr EncoderShiftId EncShiftVolume{1};
constexpr EncoderShiftId EncShiftTaskSwitch{2};
constexpr EncoderShiftId EncShiftTabSwitch{3};

constexpr double TrackpointSpeedMagnification = 2.5;

constexpr uint16_t GestureDistance = round(15 * TrackpointSpeedMagnification);

//------------------------------------------------------------------+
// CustomCommand
//------------------------------------------------------------------+

class EncoderAnd_A_IfNotRunning_B : public Command, public BeforeOtherKeyPressEventListener
{
public:
  EncoderAnd_A_IfNotRunning_B(NotNullCommandPtr a_command, NotNullCommandPtr b_command)
      : BeforeOtherKeyPressEventListener(this), _a_command(a_command), _b_command(b_command), _other_key_pressed(false)
  {
  }

  void setKeyId(uint8_t key_id) override
  {
    Command::setKeyId(key_id);
    _a_command->setKeyId(key_id);
    _b_command->setKeyId(key_id);
  }

  void onPress() override
  {
    _a_command->press();
    startListenBeforeOtherKeyPress();
    _other_key_pressed = false;
  }

  void onRelease() override
  {
    _a_command->release();
    stopListenBeforeOtherKeyPress();

    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (_scrolled == false && _other_key_pressed == false)
    {
      CommandTapper.tap(_b_command);
    }
    else
    {
      _scrolled = false;
    }
    xSemaphoreGive(_mutex);
  }

  void onBeforeOtherKeyPress(uint8_t key_id)
  {
    _other_key_pressed = true;
  }

  static void begin()
  {
    _mutex = xSemaphoreCreateMutexStatic(&_mutex_buffer);
  }

  static void notifyScroll()
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _scrolled = true;
    xSemaphoreGive(_mutex);
  }

private:
  const NotNullCommandPtr _a_command;
  const NotNullCommandPtr _b_command;
  bool _other_key_pressed;
  static SemaphoreHandle_t _mutex;
  static StaticSemaphore_t _mutex_buffer;
  static bool _scrolled;
};

bool EncoderAnd_A_IfNotRunning_B::_scrolled = false;
SemaphoreHandle_t EncoderAnd_A_IfNotRunning_B::_mutex = nullptr;
StaticSemaphore_t EncoderAnd_A_IfNotRunning_B::_mutex_buffer;

class RightButtonOrEncorderShift
    : public Command,
      public BeforeMovePointerEventListener,
      public BeforeRotateEncoderEventListener
{
public:
  RightButtonOrEncorderShift(EncoderShiftId encoder_shift_id)
      : BeforeMovePointerEventListener(),
        BeforeRotateEncoderEventListener(),
        _encoder_shift_id(encoder_shift_id.value)
  {
  }

  void onPress() override
  {
    _state = State::Pressed;
    startListenBeforeMovePointer();
    startListenBeforeRotateEncoder();
    _delta_x_sum = 0;
    _delta_y_sum = 0;
  }

  void onRelease() override
  {
    if (_state == State::Pressed)
    {
      Hid.mouseButtonsPress(RightButton);
      Hid.mouseButtonsRelease(RightButton);
    }
    else if (_state == State::MoveAndClicked)
    {
      Hid.mouseButtonsRelease(RightButton);
    }
    else if (_state == State::EncoderShifted)
    {
      HidEngine.stopEncoderShift(_encoder_shift_id);
    }
    _state = State::Unexecuted;
    stopListenBeforeMovePointer();
    stopListenBeforeRotateEncoder();
  }

  void onBeforeMovePointer(PointingDeviceId pointing_device_id, int16_t delta_x, int16_t delta_y) override
  {
    if (_state == State::Pressed)
    {
      _delta_x_sum = etl::clamp(_delta_x_sum + delta_x, INT16_MIN, INT16_MAX);
      _delta_y_sum = etl::clamp(_delta_y_sum + delta_y, INT16_MIN, INT16_MAX);
      if (abs(_delta_x_sum) >= 20 || abs(_delta_y_sum) >= 20)
      {
        Hid.mouseMove(-_delta_x_sum, -_delta_y_sum);
        Hid.mouseButtonsPress(RightButton);
        delay(10);
        Hid.mouseMove(_delta_x_sum, _delta_y_sum);
        _state = State::MoveAndClicked;
      }
    }
  }

  void onBeforeRotateEncoder(EncoderId encoder_id, int16_t step) override
  {
    if (_state == State::Pressed)
    {
      _state = State::EncoderShifted;
      HidEngine.startEncoderShift(_encoder_shift_id);
    }
  }

private:
  enum class State
  {
    Unexecuted,
    Pressed,
    MoveAndClicked,
    EncoderShifted,
  };

  EncoderShiftIdLink _encoder_shift_id;
  State _state;
  int16_t _delta_x_sum;
  int16_t _delta_y_sum;
};

//------------------------------------------------------------------+
// Keymap
//------------------------------------------------------------------+

Key keymap[] = {
    {0 /*  LeftButton          */, MS_CLK(LeftButton)},
    {1 /*  RightButton         */, MS_CLK(RightButton)},
    {2 /*  MiddleButton        */, new EncoderAnd_A_IfNotRunning_B(SFT(KeyShiftLower, EncShiftVolume, GstMiddleButton),
                                                                   MS_CLK(MiddleButton))},

    {4 /*  a                   */, KC(A)},
    {5 /*  b                   */, KC(B)},
    {6 /*  c                   */, KC(C)},
    {7 /*  d                   */, KC(D)},
    {8 /*  e                   */, KC(E)},
    {9 /*  f                   */, KC(F)},
    {10 /* g                   */, KC(G)},
    {11 /* h                   */, KC(H)},
    {12 /* i                   */, KC(I)},
    {13 /* j                   */, KC(J)},
    {14 /* k                   */, KC(K)},
    {15 /* l                   */, KC(L)},
    {16 /* m                   */, KC(M)},
    {17 /* n                   */, KC(N)},
    {18 /* o                   */, KC(O)},
    {19 /* p                   */, KC(P)},
    {20 /* q                   */, KC(Q)},
    {21 /* r                   */, KC(R)},
    {22 /* s                   */, KC(S)},
    {23 /* t                   */, KC(T)},
    {24 /* u                   */, KC(U)},
    {25 /* v                   */, KC(V)},
    {26 /* w                   */, KC(W)},
    {27 /* x                   */, KC(X)},
    {28 /* y                   */, KC(Y)},
    {29 /* z                   */, KC(Z)},
    {30 /* 1                   */, KC(_1)},
    {31 /* 2                   */, KC(_2)},
    {32 /* 3                   */, KC(_3)},
    {33 /* 4                   */, KC(_4)},
    {34 /* 5                   */, KC(_5)},
    {35 /* 6                   */, KC(_6)},
    {36 /* 7                   */, KC(_7)},
    {37 /* 8                   */, KC(_8)},
    {38 /* 9                   */, KC(_9)},
    {39 /* 0                   */, KC(_0)},
    {40 /* Enter               */, MLT(KC(Enter), WAKE_UP())},
    {41 /* Escape              */, KC(Escape)},
    {42 /* Backspace           */, KC(Backspace)},
    {43 /* Tab                 */, KC(Tab)},
    {44 /* Space               */, KC(Space)},
    {45 /* -                   */, KC(Minus)},
    {46 /* ^                   */, KC(Equal)},
    {47 /* @                   */, KC(LeftBracket)},
    {48 /* [                   */, KC(RightBracket)},
    {50 /* ]                   */, KC(NonUsNumberSign)},
    {51 /* ;                   */, KC(Semicolon)},
    {52 /* :                   */, KC(Quote)},
    {53 /* Han/Zen             */, KC(Grave)},
    {54 /* ,                   */, KC(Comma)},
    {55 /* .                   */, KC(Period)},
    {56 /* /                   */, KC(Slash)},
    {57 /* CapsLock            */, TD({KC(Ctrl), KC(Escape)},
                                      {KC(Ctrl + Shift), TAP(KC(Ctrl), 2)})},
    {58 /* F1                  */, KC(F1)},
    {59 /* F2                  */, KC(F2)},
    {60 /* F3                  */, KC(F3)},
    {61 /* F4                  */, KC(F4)},
    {62 /* F5                  */, KC(F5)},
    {63 /* F6                  */, KC(F6)},
    {64 /* F7                  */, KC(F7)},
    {65 /* F8                  */, KC(F8)},
    {66 /* F9                  */, KC(F9)},
    {67 /* F10                 */, KC(F10)},
    {68 /* F11                 */, KC(F11)},
    {69 /* F12                 */, KC(F12)},
    {70 /* PrtSc               */, TD({KC(PrintScreen)},
                                      {KC(Gui + Shift + S)})},
    {73 /* Insert              */, KC(Insert)},
    {74 /* Home                */, KC(Home)},
    {75 /* PgUp                */, KC(PageUp)},
    {76 /* Delete              */, KC(Delete)},
    {77 /* End                 */, KC(End)},
    {78 /* PgDn                */, KC(PageDown)},
    {79 /* Right               */, KC(Right)},
    {80 /* Left                */, KC(Left)},
    {81 /* Down                */, KC(Down)},
    {82 /* Up                  */, KC(Up)},

    {135 /* \(yen)             */, KC(Int1)},
    {136 /* LeftGui            */, TD_DPM({MLT(KC(RightGui), SFT(GstArrow))},
                                          {MLT(KC(RightGui + Shift), SFT(GstArrow)), KC(Gui + Ctrl + T)},
                                          {PdTrackpoint})},
    {137 /* \(BackSlash)       */, KC(Int3)},
    {138 /* Henkan             */, TD_DPM({MLT(SFT(KeyShiftRaise, EncShiftPan, GstTabSwitch), KC(F24)), MLT(KC(Int4), KC(Lang1))},
                                          {SFT(KeyShiftTaskSwitch, EncShiftTaskSwitch, GstArrow)},
                                          {PdTrackpoint})},
    {139 /* Muhenkan           */, TD_DPM({MLT(SFT(KeyShiftRaise, EncShiftPan, GstTabSwitch), KC(F24)), MLT(KC(Int5), KC(Lang2))},
                                          {SFT(KeyShiftTaskSwitch, EncShiftTaskSwitch, GstArrow)},
                                          {PdTrackpoint})},

    {200 /* LeftCtrl           */, TD({KC(Ctrl + Alt)},
                                      {KC(Ctrl + Alt + Shift)})},
    {201 /* LeftShift          */, TD({KC(Shift)},
                                      {KC(Shift + CapsLock)})},
    {202 /* LeftAlt            */, TD({KC(Alt)},
                                      {KC(Alt + Shift), TAP(KC(Alt), 2)})},
    {203 /* LeftGui            */, TD({KC(Gui)},
                                      {KC(Gui + Shift), KC(Gui + Ctrl + T)})},
    {204 /* RightCtrl          */, TD({KC(RightCtrl)},
                                      {KC(RightCtrl + Shift), TAP(KC(RightCtrl), 2)})},
    {205 /* RightShift         */, TD({KC(RightShift)},
                                      {KC(RightShift + CapsLock)})},
    {206 /* RightAlt           */, TD({KC(RightAlt)},
                                      {KC(RightAlt + Shift), TAP(KC(RightAlt), 2)})},

    {210 /* Mute         (F1)  */, KC(Mute)},
    {211 /* VolumeDown   (F2)  */, KC(VolumeDown)},
    {212 /* VolumeUp     (F3)  */, KC(VolumeUp)},
    {213 /* MicOff       (F4)  */, KC(Ctrl + Shift + M)},
    {214 /* LightDown    (F5)  */, KC(LightDown)},
    {215 /* LightUp      (F6)  */, KC(LightUp)},
    {217 /* Notification (F8)  */, KC(Gui + N)},
    {218 /* Setting      (F9)  */, KC(Gui + A)},
    {219 /* ?            (F10) */, NOP()},
    {220 /* ?            (F11) */, NOP()},
    {221 /* ?            (F12) */, NOP()},

    // Lift

    {231 /* LeftButton         */, MS_CLK(LeftButton)},
    {232 /* RightButton        */, new RightButtonOrEncorderShift(EncShiftTabSwitch)},
    {233 /* MiddleButton       */, MS_CLK(MiddleButton)},
    {234 /* BackwardButton     */, MS_CLK(BackwardButton)},
    {235 /* ForwardButton      */, MS_CLK(ForwardButton)},

    // MDBT50Q Dongle

    {255 /* Dongle Button      */, HT(RESET(), NOP(), 2000)},
};

KeyShift keyShiftMap[] = {
    {
        KeyShiftRaise,
        KeymapOverlay::Enable,
        KEYMAP({
            {11 /* h               */, KC(Left)},
            {13 /* j               */, KC(Down)},
            {14 /* k               */, KC(Up)},
            {15 /* l               */, KC(Right)},

            {44 /* space           */, KC(Enter)},
            {16 /* m               */, KC(Application)},
            {12 /* i               */, KC(Backspace)},
            {18 /* o               */, KC(Delete)},
            {17 /* n               */, KC(PageDown)},
            {19 /* p               */, KC(PageUp)},
            {54 /* ,               */, KC(Home)},
            {55 /* .               */, KC(End)},
            {76 /* Delete          */, KC(SystemSleep)},

            {30 /* 1               */, KC(F1)},
            {31 /* 2               */, KC(F2)},
            {32 /* 3               */, KC(F3)},
            {33 /* 4               */, KC(F4)},
            {34 /* 5               */, KC(F5)},
            {35 /* 6               */, KC(F6)},
            {36 /* 7               */, KC(F7)},
            {37 /* 8               */, KC(F8)},
            {38 /* 9               */, KC(F9)},
            {39 /* 10              */, KC(F10)},
            {45 /* -               */, KC(F11)},
            {46 /* ^               */, KC(F12)},

            {48 /* [               */, KC(Gui + Ctrl + Left)},
            {50 /* ]               */, KC(Gui + Ctrl + Right)},
        }),
    },

    {
        KeyShiftLower,
        KeymapOverlay::Enable,
        KEYMAP({
            {0 /*  LeftButton      */, MS_CLK(BackwardButton)},
            {1 /*  RightButton     */, MS_CLK(ForwardButton)},

            {11 /* h               */, KC(PrevTrack)},
            {13 /* j               */, KC(VolumeDown)},
            {14 /* k               */, KC(VolumeUp)},
            {15 /* l               */, KC(NextTrack)},
            {54 /* ,               */, KC(Rewind)},
            {55 /* .               */, KC(FastForward)},
            {44 /* space           */, KC(PlayPause)},
            {16 /* m               */, KC(Mute)},

            {231 /* LeftButton     */, KC(PlayPause)},
            {233 /* MiddleButton   */, KC(Mute)},
            {234 /* BackwardButton */, KC(PrevTrack)},
            {235 /* ForwardButton  */, KC(NextTrack)},
        }),
    },

    {
        KeyShiftTaskSwitch,
        KeymapOverlay::Enable,
        KEYMAP({
            {11 /* h               */, RPT(KC(Shift + Tab), 300, 150)},
            {13 /* j               */, RPT(KC(Down), 300, 150)},
            {14 /* k               */, RPT(KC(Up), 300, 150)},
            {15 /* l               */, RPT(KC(Tab), 300, 150)},
            {18 /* o               */, TAP(KC(Delete))},

            {26 /* w               */, RPT(KC(Up), 300, 150)},
            {4 /*  a               */, RPT(KC(Shift + Tab), 300, 150)},
            {22 /* s               */, RPT(KC(Down), 300, 150)},
            {7 /*  d               */, RPT(KC(Tab), 300, 150)},
            {57 /* CapsLock        */, TAP(KC(Delete))},
        }),
        PreCommand{MLT(KC(Alt), TAP(KC(Tab)), TAP(KC(Shift + Tab))), Timing::Immediately},
    },
};

Combo comboMap[] = {
    // { first_key_id, second_key_id, command, combo_term_ms, combo_behavior }

    {231, 232, MS_CLK(MiddleButton), 75, ComboBehavior::AnyOrder_SlowRelease},
};

Gesture gestureMap[] = {
    // { gesture_id, mouse_id, distance, angle_snap,
    //   up_command, down_command, left_command, right_command,
    //   [PreCommand{ command, timing }] }

    {GstTabSwitch, PdTrackpoint, GestureDistance, AngleSnap::Enable,
     _______, _______, STEP_SPD(KC(Ctrl + Shift + Tab), 250), STEP_SPD(KC(Ctrl + Tab), 250)},

    {GstArrow, PdTrackpoint, GestureDistance, AngleSnap::Enable,
     STEP_SPD(KC(Up), 250), STEP_SPD(KC(Down), 250), STEP_SPD(KC(Left), 250), STEP_SPD(KC(Right), 250)},

    {GstMiddleButton, PdLift, 1, AngleSnap::Disable,
     MS_MOV(0, -1), MS_MOV(0, 1), MS_MOV(-1, 0), MS_MOV(1, 0),
     PreCommand{MS_CLK(MiddleButton), Timing::JustBeforeFirstAction}},
};

Encoder encoderMap[] = {
    // { encoder_id,
    //   counterclockwise_command, clockwise_command }

    {EncTrackpoint, 1,
     MS_SCR(-1, 0), MS_SCR(1, 0)},

    {EncLiftWheel, 1,
     MS_SCR(-1, 0), MS_SCR(1, 0)},

    {EncLiftPan, 1,
     MS_SCR(0, -1), MS_SCR(0, 1)},
};

EncoderShift encoderShiftMap[] = {
    // { encoder_shift_id, encoder_id,
    //   counterclockwise_command, clockwise_command,
    //   [PreCommand{ command, timing }] }

    {EncShiftPan, EncTrackpoint, 1,
     MS_SCR(0, 1), MS_SCR(0, -1)},

    {EncShiftVolume, EncLiftWheel, 1,
     KC(VolumeUp), KC(VolumeDown)},

    {EncShiftTaskSwitch, EncLiftWheel, 1,
     KC(Tab), KC(Shift + Tab)},

    {EncShiftPan, EncLiftWheel, 1,
     MS_SCR(0, 1), MS_SCR(0, -1)},

    {EncShiftTabSwitch, EncLiftWheel, 1,
     KC(Ctrl + Tab), KC(Ctrl + Shift + Tab)},
};
