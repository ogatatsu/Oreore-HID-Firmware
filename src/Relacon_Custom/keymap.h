
#include "CommandDsl.h"
#include "KC.h"

using namespace hidpg;

Key keymap[] = {
  { 0, MS_CLK(LeftButton) },
  { 1, TD({ { MS_CLK(RightButton), MS_CLK(RightButton) },
            { TAP(MS_CLK(RightButton), 2, 30), TRC(0) } },
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

Track trackMap[] = {
  { 0, 40, AngleSnap::Disable, MS_SCR(1, 0), MS_SCR(-1, 0), MS_SCR(0, -1), MS_SCR(0, 1) }
};
