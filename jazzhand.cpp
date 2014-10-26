#include <iostream>
#include <string.h>
#include <Leap.h>
#include <unistd.h>
#include <cstdlib>
#include <time.h>

#include "task_runner.h"

#define CIRCLE_THRESHOLD .4
#define MIN_RADIUS 30 //default 5mm
#define MIN_ARC 3     //default 1.5*pi radians
#define MIN_SWIPE_LENGHT 50  //default 150
#define MIN_VELOCITY 650      //default 1000

using namespace Leap;

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_STOP"};

class GestureListener : public Listener {
  public:
    // virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    // virtual void onDisconnect(const Controller&);
    // virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    // virtual void onFocusGained(const Controller&);
    // virtual void onFocusLost(const Controller&);
    // virtual void onDeviceChange(const Controller&);
    // virtual void onServiceConnect(const Controller&);
    // virtual void onServiceDisconnect(const Controller&);

  private:
};


void GestureListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

double distance2d_squared(Vector a, Vector b){

  double deltax = b.x-a.x;
  double deltay = b.y-a.y;
  double deltaz = b.z - a.z;
  return (deltax * deltax) + (deltay * deltay) + (deltaz * deltaz);
}

float circle_counter = 0; //how much of a circle has been made
bool doing_gesture = false;
int gesture_id = 0;
time_t now;
time_t last;

void GestureListener::onFrame(const Controller& controller) {
  // Get the most recent frame
  const Frame frame = controller.frame();

  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = *hl;
    std::string handType = hand.isLeft() ? "Left hand" : "Right hand";

    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    Arm arm = hand.arm();
    // std::cout << std::string(2, ' ') <<  "Arm direction: " << arm.direction()
    //           << " wrist position: " << arm.wristPosition()
    //           << " elbow position: " << arm.elbowPosition() << std::endl;

    // Get index finger
    Finger index = hand.fingers().fingerType(Finger::TYPE_INDEX)[0];
    Vector index_pos;
    Vector index_dir;

    //get thumb
    Finger thumb = hand.fingers().fingerType(Finger::TYPE_THUMB)[0];
    Vector thumb_pos;
    Vector thumb_dir;

    if (index.isExtended()){
      index_pos = index.tipPosition();
      index_dir = index.direction();

      move_mouse(index_pos,index_dir);
    }

    thumb_pos = thumb.tipPosition();
    thumb_dir = thumb.tipPosition();

    double i = distance2d_squared(index_pos,thumb_pos);

    if(i < 3500 && difftime(now,last) >= 1){
      time(&last);
      mouse_click();
    } else if(i > 3500){
      time(&now);
    }

  }

  // Get tools
  const ToolList tools = frame.tools();
  for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
    const Tool tool = *tl;
  }

  // Get gestures
  const GestureList gestures = frame.gestures();

  int gestureMatch = 0;
  for (int g = 0; g < gestures.count(); ++g) {
    if (gestures[g].id() == gesture_id) {
      gestureMatch++;
    }
  }
  if (gestureMatch == 0 && doing_gesture == true) {
    doing_gesture = false;
    gesture_id = 0;
    std::cout << "gesture reset due to lack of activity" << std::endl;
  }

  for (int g = 0; g < gestures.count(); ++g) {
    Gesture gesture = gestures[g];

    switch (gesture.type()) {
      case Gesture::TYPE_CIRCLE:
      {
        CircleGesture circle = gesture;
        bool clockwise = false;

        //Check if circle started
        if(circle.state() == Gesture::STATE_START && !doing_gesture){
          doing_gesture = true;
          gesture_id = circle.id();
          std::cout << "scroll START" << std::endl;
          break;
        }
        else if (circle.state() == Gesture::STATE_START) {
          std::cout << "couldn't start scroll because existing scroll" << std::endl;
        }

        // Calculate angle swept since last frame
        float sweptAngle = 0;
        if (circle.state() == Gesture::STATE_UPDATE && gesture_id == circle.id()) {

          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
          if (sweptAngle == -1) {
            //circle.state() = Gesture::STATE_STOP;
            doing_gesture = false;
            gesture_id = 0;
            std::cout << "scroll PREMATURELY EXIT" << std::endl;
            break;
          }
          circle_counter += sweptAngle;

          if(circle_counter >= CIRCLE_THRESHOLD){
            //Find direction
            if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
              clockwise = true;
            } else {
              clockwise = false;
            }

            circle_counter -= sweptAngle;
            circle_action(clockwise);
          }

        }

        if(circle.state() == Gesture::STATE_STOP && doing_gesture && gesture_id == circle.id()){
          doing_gesture = false;
          gesture_id = 0;
          std::cout << "scroll STOP" << std::endl;
        }

        break;
      }
      case Gesture::TYPE_SWIPE:
      {
        SwipeGesture swipe = gesture;

        if((swipe.state() == Gesture::STATE_START || swipe.state() == Gesture::STATE_UPDATE) && !doing_gesture){

          doing_gesture = true;
          gesture_id = swipe.id();

          Vector direction = swipe.direction();
          swipe_action(direction);

        }
        else if(swipe.state() == Gesture::STATE_STOP && doing_gesture && gesture_id == swipe.id()){
          doing_gesture = false;
          gesture_id = 0;

          usleep(250000); // half second wait
        }

        break;
      }
      case Gesture::TYPE_SCREEN_TAP:
      {

        // HandList hands = frame.hands();
        // for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
        //   // Get the first hand
        //   const Hand hand = *hl;
        //
        //   // Get index finger
        //   Finger index = hand.fingers().fingerType(Finger::TYPE_INDEX)[0];
        //
        //   if (index.isExtended()){
        //     Vector pos = index.stabilizedTipPosition();
        //     Vector dir = index.direction();
        //
        //     ScreenTapGesture screentap = gesture;
        //     std::cout << std::string(2, ' ')
        //         << "Screen Tap id: " << gesture.id()
        //         << ", state: " << stateNames[gesture.state()]
        //         << ", position: " << screentap.position()
        //         << ", direction: " << screentap.direction()
        //         << ", timestamp: " << frame.timestamp()
        //         << std::endl;
        //
        //     mouse_click(pos,dir);
        //   }
          break;
        }
      default:
        std::cout << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
        break;
    }
  }
}


int main(int argc, char** argv) {

    // Create a sample listener and controller
    GestureListener listener;
    Controller controller;
    // Have the sample listener receive events from the controller
    controller.addListener(listener);

    //change size of min circle
    controller.config().setFloat("Gesture.Circle.MinRadius", MIN_RADIUS);

    //change min arc of a circle
    controller.config().setFloat("Gesture.Circle.MinArc", MIN_ARC);

    //change min length of a swipe
    controller.config().setFloat("Gesture.Swipe.MinLength", MIN_SWIPE_LENGHT);

    //change min swipe velocity
    controller.config().setFloat("Gesture.Swipe.MinVelocity", MIN_VELOCITY);

    controller.config().save();

    //set the controller to recieve frames even when not in foreground
    controller.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    // if (argc > 1 && strcmp(argv[1], "--bg") == 0)
    //   controller.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
