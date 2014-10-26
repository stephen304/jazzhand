#include "task_runner.h"
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <Leap.h>

#define SWIPE_THRESHOLD .8
#define MOUSE_SENSITIVITY 7

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



void move_mouse(Vector pos, Vector dir){

    int x_offset = 800;
    int y_offset = 1500;

    char command[64] = "xdotool mousemove ";
    char args[32];

    int x_pos = (pos.x * MOUSE_SENSITIVITY) + x_offset;
    if(x_pos < 0) x_pos = 0;

    int y_pos = -1 * (pos.y * MOUSE_SENSITIVITY) + y_offset;
    if(y_pos < 0 ) y_pos = 900;

    sprintf(args, "%d %d", x_pos, y_pos);
    strcat(command, args);
    system(command);
}
