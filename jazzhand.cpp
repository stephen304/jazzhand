#include <iostream>
#include <string.h>
#include <Leap.h>

#include "task_runner.h"

using namespace Leap;

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};


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

    // Get fingers
    const FingerList fingers = hand.fingers();
    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
      const Finger finger = *fl;
      // std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
      //           << " finger, id: " << finger.id()
      //           << ", length: " << finger.length()
      //           << "mm, width: " << finger.width() << std::endl;

      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = finger.bone(boneType);
        // std::cout << std::string(6, ' ') <<  boneNames[boneType]
        //           << " bone, start: " << bone.prevJoint()
        //           << ", end: " << bone.nextJoint()
        //           << ", direction: " << bone.direction() << std::endl;
      }
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

        if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
          clockwiseness = "clockwise";
        } else {
          clockwiseness = "counterclockwise";
        }

        // Calculate angle swept since last frame
        float sweptAngle = 0;
        if (circle.state() != Gesture::STATE_START) {
          CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
          sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
        }
        std::cout << std::string(2, ' ')
                  << "Circle id: " << gesture.id()
                  << ", state: " << stateNames[gesture.state()]
                  << ", progress: " << circle.progress()
                  << ", radius: " << circle.radius()
                  << ", angle " << sweptAngle * RAD_TO_DEG
                  <<  ", " << clockwiseness
                  << ", timestamp: " << frame.timestamp()
                  << std::endl;
        break;
      }
      case Gesture::TYPE_SWIPE:
      {
        SwipeGesture swipe = gesture;

        bool swiping = false;
        if(stateNames[gesture.state()] == "STATE_START"){
          swiping = true;
          Vector direction = swipe.direction();

          if(direction.x > .8){

          }
        }

        std::cout << std::string(2, ' ')
          << "Swipe id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", direction: " << swipe.direction()
          << ", speed: " << swipe.speed()
          << ", timestamp: " << frame.timestamp()
          << std::endl;
        break;
      }
      case Gesture::TYPE_KEY_TAP:
      {
        KeyTapGesture tap = gesture;
        std::cout << std::string(2, ' ')
          << "Key Tap id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", position: " << tap.position()
          << ", direction: " << tap.direction()
          << std::endl;
        break;
      }
      case Gesture::TYPE_SCREEN_TAP:
      {
        ScreenTapGesture screentap = gesture;
        std::cout << std::string(2, ' ')
          << "Screen Tap id: " << gesture.id()
          << ", state: " << stateNames[gesture.state()]
          << ", position: " << screentap.position()
          << ", direction: " << screentap.direction()
          << ", timestamp: " << frame.timestamp()
          << std::endl;
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
