#ifndef KEYBOARD_CONTROLLER
#define KEYBOARD_CONTROLLER

#include "HWController.hpp"

class KeyboardController {
  private:
    bool keyStatus[2];
    bool keyLocked[2];
    HWController *hwc;

  public:
    KeyboardController(HWController *hwc);
    void readKeys();
    bool getKeyStatus(char key);
    bool getKeyClick(char key);
};
#endif
