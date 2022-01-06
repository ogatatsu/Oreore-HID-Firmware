
#include "CommandDsl.h"
#include "KC.h"

using namespace hidpg;

constexpr uint8_t MOUSE_ID = 0;
constexpr uint8_t GESTURE_ID_TRC = 0;

Key keymap[] = {
  { 0, MS_CLK(LeftButton) },
  { 1, TD({ { MS_CLK(RightButton), MS_CLK(RightButton) },
            { TAP(MS_CLK(RightButton), 2, 30), GST(GESTURE_ID_TRC) } },
          true) },
  { 2, MS_CLK(MiddleButton) },
  { 3, MS_CLK(BackwardButton) },
  { 4, MS_CLK(ForwardButton) },
  { 10, CC(NextTrack) },
  { 11, CC(PrevTrack) },
  { 12, CC(PlayPause) },
  { 13, CC(VolumeUp) },
  { 14, CC(VolumeDown) },
};

Encoder encoderMap[] = {
  { 0, MS_SCR(-1, 0), MS_SCR(1, 0) },
};

Gesture gestureMap[] = {
  // { gesture_id, mouse_id, distance, angle_snap, up_command, down_command, left_command, right_command }
  { GESTURE_ID_TRC, MOUSE_ID, 40, AngleSnap::Disable, MS_SCR(1, 0), MS_SCR(-1, 0), MS_SCR(0, -1), MS_SCR(0, 1) }
};
