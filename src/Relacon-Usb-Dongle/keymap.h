
#include "HidEngine.h"
#include "KC.h"

using namespace hidpg;

Key keymap[] = {
  { 0, MS_CLK(LeftButton) },
  { 1, MS_CLK(RightButton) },
  { 2, MS_CLK(MiddleButton) },
  { 3, MS_CLK(BackwardButton) },
  { 4, MS_CLK(ForwardButton) },
  { 10, CC(Next) },
  { 11, CC(Prev) },
  { 12, CC(PlayPause) },
  { 13, CC(VolumeUp) },
  { 14, CC(VolumeDown) },
};

Encoder encoderMap[] = {
  { 0, MS_SCR(-1, 0), MS_SCR(1, 0) },
};
