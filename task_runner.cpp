#include "task_runner.h"
#include <cstdlib>

void swipe_left(){

}
void swipe_right(){

}
void swipe_up(){

}
void swipe_down(){

}
void circle_action(bool direction) {
  if (direction) {
    system("xdotool click --clearmodifiers 5");
  }
  else {
    system("xdotool click --clearmodifiers 4");
  }
}
