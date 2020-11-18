
#include "HidEngine.h"
#include "KC.h"

using namespace hidpg;

Key keymap[] = {
  { 0, MS_CLK(LeftButton) },
  { 1, TD({ { MS_CLK(RightButton), MS_CLK(RightButton) },
            { TRC(0), TRC(0) } }) },
  { 2, MS_CLK(MiddleButton) },
  { 3, MS_CLK(BackwardButton) },
  { 4, MS_CLK(ForwardButton) },
  { 10, CC(Next) },
  { 11, CC(Prev) },
  { 12, CC(PlayPause) },
  { 13, CC(VolumeUp) },
  { 14, CC(VolumeDown) },
  { 15, NOP() },
};

Encoder encoderMap[] = {
  { 0, MS_SCR(-1, 0), MS_SCR(1, 0) },
};

Track trackMap[] = {
  { 0, 20, MS_SCR(1, 0), MS_SCR(-1, 0) }
};
