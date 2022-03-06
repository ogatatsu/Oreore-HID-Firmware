
#include "BleCommandDsl.h"
#include "CommandDsl.h"
#include "KC.h"

using namespace hidpg;

constexpr PointingDeviceId PdTrackball{0};

constexpr EncoderId EncWheel{0};

constexpr GestureId GestureScroll{0};

Key keymap[] = {
    {0, MS_CLK(LeftButton)},
    {1, TD_DPM({MS_CLK(RightButton), MS_CLK(RightButton)},
               {TAP(MS_CLK(RightButton), 2, 30), SFT(GestureScroll)},
               {PdTrackball})},
    {2, MS_CLK(MiddleButton)},
    {3, MS_CLK(BackwardButton)},
    {4, MS_CLK(ForwardButton)},
    {10, KC(NextTrack)},
    {11, KC(PrevTrack)},
    {12, KC(PlayPause)},
    {13, KC(VolumeUp)},
    {14, KC(VolumeDown)},

    {255, HT(RESET(), NOP(), 2000)},
};

Encoder encoderMap[] = {
    // { encoder_id, step,
    //   counterclockwise_command, clockwise_command }

    {EncWheel, 1,
     MS_SCR(-1, 0), MS_SCR(1, 0)},
};

Gesture gestureMap[] = {
    // { gesture_id, mouse_id, distance, angle_snap,
    //   up_command, down_command, left_command, right_command,
    //   [PreCommand{ command, timing }] }

    {GestureScroll, PdTrackball, 30, AngleSnap::Enable,
     MS_SCR(1, 0), MS_SCR(-1, 0), MS_SCR(0, -1), MS_SCR(0, 1)},
};
