#include "task_runner.h"
#include <cstdlib>
#include <Leap.h>

#define SWIPE_THRESHOLD .8

using namespace Leap;

void swipe_action(Vector direction) {
  if (direction.x > SWIPE_THRESHOLD) {
    system("bspc desktop -f next");
  }
  if (direction.x < -SWIPE_THRESHOLD) {
    system("bspc desktop -f prev");
  }
  if (direction.y > SWIPE_THRESHOLD) {
    //Up?
  }
  if (direction.y < -SWIPE_THRESHOLD) {
    //Down?
  }
  if (direction.z > SWIPE_THRESHOLD) {
    //Forward?
  }
  if (direction.z < -SWIPE_THRESHOLD) {
    //Back?
  }
}

void circle_action(bool direction) {
  if (direction) {
    system("xdotool click --clearmodifiers 5");
  }
  else {
    system("xdotool click --clearmodifiers 4");
  }
}
