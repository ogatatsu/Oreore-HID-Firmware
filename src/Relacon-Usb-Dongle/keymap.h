
#include "HidEngine.h"
#include "KC.h"
#include "HidCore.h"

using namespace hidpg;

class DblRightClick : public Command
{
  void onPress(uint8_t accumulation)
  {
    Hid.mouseButtonPress(RightButton);
    delay(20);
    Hid.mouseButtonRelease(RightButton);
    delay(20);
    Hid.mouseButtonPress(RightButton);
    delay(20);
    Hid.mouseButtonRelease(RightButton);
  }
};

Key keymap[] = {
  { 0, MS_CLK(LeftButton) },
  { 1, TD({ { MS_CLK(RightButton), MS_CLK(RightButton) },
            { new DblRightClick, TRC(0) } }) },
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

Track trackMap[] = {
  { 0, 25, MS_SCR(1, 0), MS_SCR(-1, 0) }
};
