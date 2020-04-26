#include "KeyboardController.hpp"

KeyboardController::KeyboardController(HWController *hwc) {
  keyLocked[0]=false;
  keyLocked[1]=false;
  this->hwc=hwc;
}

void KeyboardController::readKeys(){
  keyStatus[0]=hwc->getBufferedPin(KEY_START);
  keyStatus[1]=hwc->getBufferedPin(KEY_SELECT);
}

bool KeyboardController::getKeyStatus(char key) {
  return keyStatus[key];
}

bool KeyboardController::getKeyClick(char key) {
  if (getKeyStatus(key)) {
    if (keyLocked[key])
      return false;
    else {
      keyLocked[key]=true;
      return true;
    }
  } else {
    keyLocked[key]=false;
    return false;
  }
}
