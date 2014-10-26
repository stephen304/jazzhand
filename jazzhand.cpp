#include <iostream>
#include <string.h>
#include <Leap.h>
#include <unistd.h>
#include <cstdlib>

#include "task_runner.h"

#define CIRCLE_THRESHOLD .4
#define MIN_RADIUS 50 //default 5mm
#define MIN_ARC 3     //default 1.5*pi radians
#define MIN_SWIPE_LENGHT 75  //default 150
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
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

float circle_counter = 0; //how much of a circle has been made
bool doing_gesture = false;
bool doing_circle = false;
bool doing_swipe = false;

void GestureListener::onFrame(const Controller& controller) {
  // Get the most recent frame
  const Frame frame = controller.frame();

  // report some basic information
  // std::cout << "Frame id: " << frame.id()
  //           << ", timestamp: " << frame.timestamp()
  //           << ", hands: " << frame.hands().count()
  //           << ", fingers: " << frame.fingers().count()
  //           << ", tools: " << frame.tools().count()
  //           << ", gestures: " << frame.gestures().count() << std::endl;

  // get hand info
  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = *hl;
    std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
    // std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
    //           << ", palm position: " << hand.palmPosition() << std::endl;
    // Get the hand's normal vector and direction
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
    // std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
    //           << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
    //           << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;

    // Get the Arm bone
    Arm arm = hand.arm();
    // std::cout << std::string(2, ' ') <<  "Arm direction: " << arm.direction()
    //           << " wrist position: " << arm.wristPosition()
    //           << " elbow position: " << arm.elbowPosition() << std::endl;

    // Get index finger
    Finger index = hand.fingers().fingerType(Finger::TYPE_INDEX)[0];

    if (index.isExtended()){
      Vector pos = index.tipPosition();
      Vector dir = index.direction();

      move_mouse(pos,dir);
    }

  }

  // Get tools
  const ToolList tools = frame.tools();
  for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
    const Tool tool = *tl;
    // std::cout << std::string(2, ' ') <<  "Tool, id: " << tool.id()
    //           << ", position: " << tool.tipPosition()
    //           << ", direction: " << tool.direction() << std::endl;
  }

  // Get gestures
  const GestureList gestures = frame.gestures();

  for (int g = 0; g < gestures.count(); ++g) {
    Gesture gesture = gestures[g];

    switch (gesture.type()) {
      case Gesture::TYPE_CIRCLE:
      {
        CircleGesture circle = gesture;
        std::string clockwiseness;
        bool clockwise = false;

        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
          clockwiseness = "clockwise";
          clockwise = true;
        } else {
          clockwiseness = "counterclockwise";
          clockwise = false;
        }

        if(circle.pointables().count() > 1){
            break;
        }

        if(circle.state() == Gesture::STATE_START && !doing_gesture){
          doing_gesture = true;
          doing_circle = true;
        }
        // Calculate angle swept since last frame
        float sweptAngle = 0;
        if (circle.state() != Gesture::STATE_START && doing_circle) {

          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
          if (sweptAngle < 0.00008) {
            //circle.state() = Gesture::STATE_STOP;
            doing_gesture = false;
            doing_circle = false;
          }
          circle_counter += sweptAngle;

          if(circle_counter >= CIRCLE_THRESHOLD){
            circle_counter = 0;
            circle_action(clockwise);
            std::cout << "scroll" << std::endl;
          }

          std::cout << std::string(2, ' ')
                  << "Circle id: " << gesture.id()
                  << ", state: " << stateNames[gesture.state()]
                  << ", progress: " << circle.progress()
                  << ", radius: " << circle.radius()
                  << ", angle " << sweptAngle * RAD_TO_DEG
                  <<  ", " << clockwiseness
                  << ", timestamp: " << frame.timestamp()
                  << ", counter: " << circle_counter
                  << std::endl;
        }

        if(circle.state() == Gesture::STATE_STOP && doing_gesture && doing_circle){
          doing_gesture = false;
          doing_circle = false;
        }

        break;
      }
      case Gesture::TYPE_SWIPE:
      {
        SwipeGesture swipe = gesture;

        if(swipe.state() == Gesture::STATE_START && !doing_gesture){

          doing_gesture = true;
          doing_swipe = true;

          Vector direction = swipe.direction();
          swipe_action(direction);

          std::cout << std::string(2, ' ')
          << "Swipe id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", direction: " << swipe.direction()
          << ", speed: " << swipe.speed()
          << ", timestamp: " << frame.timestamp()
          << std::endl;

        } else if(swipe.state() == Gesture::STATE_STOP && doing_gesture && doing_swipe){
          doing_gesture = false;
          doing_swipe = false;

          std::cout << std::string(2, ' ')
          << "Swipe id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", direction: " << swipe.direction()
          << ", speed: " << swipe.speed()
          << ", timestamp: " << frame.timestamp()
          << std::endl;

          usleep(250000); // half second wait
        }

        break;
      }
      case Gesture::TYPE_KEY_TAP:
      {
        // KeyTapGesture tap = gesture;
        // std::cout << std::string(2, ' ')
        //   << "Key Tap id: " << gesture.id()
        //   << ", state: " << stateNames[gesture.state()]
        //   << ", position: " << tap.position()
        //   << ", direction: " << tap.direction()
        //   << std::endl;
        break;
      }
      case Gesture::TYPE_SCREEN_TAP:
      {

        HandList hands = frame.hands();
        for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
          // Get the first hand
          const Hand hand = *hl;

          // Get index finger
          Finger index = hand.fingers().fingerType(Finger::TYPE_INDEX)[0];

          if (index.isExtended()){
            Vector pos = index.stabilizedTipPosition();
            Vector dir = index.direction();

            ScreenTapGesture screentap = gesture;
            std::cout << std::string(2, ' ')
                << "Screen Tap id: " << gesture.id()
                << ", state: " << stateNames[gesture.state()]
                << ", position: " << screentap.position()
                << ", direction: " << screentap.direction()
                << ", timestamp: " << frame.timestamp()
                << std::endl;

            mouse_click(pos,dir);
          }
        }

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
