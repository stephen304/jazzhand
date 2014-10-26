#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <Leap.h>
#include "task_runner.h"
#include "config.h"

#define SWIPE_THRESHOLD .8
#define MOUSE_SENSITIVITY 7

using namespace Leap;

void swipe_action(Vector direction) {
  if (direction.x > SWIPE_THRESHOLD) {
    system(RIGHT_SWIPE);
  }
  if (direction.x < -SWIPE_THRESHOLD) {
    system(LEFT_SWIPE);
  }
  if (direction.y > SWIPE_THRESHOLD) {
    system(UP_SWIPE);
  }
  if (direction.y < -SWIPE_THRESHOLD) {
    system(DOWN_SWIPE);
  }
  if (direction.z > SWIPE_THRESHOLD) {
    system(FORWARD_SWIPE);
  }
  if (direction.z < -SWIPE_THRESHOLD) {
    system(BACK_SWIPE);
  }
}

void circle_action(bool cw) {
  if (cw) {
    system(CIRCLE_CW);
  }
  else {
    system(CIRCLE_CCW);
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

void mouse_click(Vector pos, Vector dir){
    system("xdotool click 1");
}
