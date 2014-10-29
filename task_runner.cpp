#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <Leap.h>
#include "task_runner.h"
#include "config.h"

#define SWIPE_THRESHOLD .8
#define MOUSE_SENSITIVITY 7
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define VSCREEN_LX -170
#define VSCREEN_UX 170
#define VSCREEN_LY -10
#define VSCREEN_UY 130

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
    system(BACK_SWIPE);
  }
  if (direction.z < -SWIPE_THRESHOLD) {
    system(FORWARD_SWIPE);
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

    //Calculate where finger points on place z=-300
    float distance = 300 + pos.z;
    float dirZ = 0;
    if (dir.z < 0) {
      dirZ = dir.z * -1;
    }
    else {
      dirZ = dir.z;
    }
    float scaleFactor = distance / dirZ;
    float backX = dir.x * scaleFactor + pos.x;
    float backY = dir.y * scaleFactor + pos.y;

    //Don't let the mouse escape!
    if (backY < VSCREEN_LY) {
      backY = VSCREEN_LY;
    }
    if (backX < VSCREEN_LX) {
      backX = VSCREEN_LX;
    }
    if (backX > VSCREEN_UX) {
      backX = VSCREEN_UX;
    }
    if (backY > VSCREEN_UY) {
      backY = VSCREEN_UY;
    }

    //Make bottom-left based back coordinates
    float backPosX = backX - VSCREEN_LX;
    float backPosY = backY - VSCREEN_LY;

    //Convert Y to top based
    backPosY = (VSCREEN_UY - VSCREEN_LY) - backPosY;

    //Convert to real screen coordinates
    int x_pos = (backPosX / (VSCREEN_UX - VSCREEN_LX)) * SCREEN_WIDTH;
    int y_pos = (backPosY / (VSCREEN_UY - VSCREEN_LY)) * SCREEN_HEIGHT;

    char command[64] = "xdotool mousemove ";
    char args[32];

    sprintf(args, "%d %d", x_pos, y_pos);
    strcat(command, args);
    system(command);
}

void mouse_click(){
    system("xdotool click 1");
}

void screen_tap_action(){
  system(TAP_SCREEN);
}

void key_tap_action(){
  system(TAP_ACTION);
}
