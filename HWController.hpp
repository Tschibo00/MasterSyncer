#ifndef HW_CONTROLLER
#define HW_CONTROLLER

#include <Arduino.h>

#define KEY_START 0
#define KEY_SELECT 1
#define KEY_ENC_L 2
#define KEY_ENC_R 3

class HWController {
  private:
    unsigned char led[3];
    bool led_dot=false;
    uint8_t currentDigit=0;
    char displayUpdateThrottle=0;
    bool canUpdateDisplay=false;
    
    bool keyStart;
    bool keySelect;
    bool encL;
    bool encR;

    void writeDigit(uint8_t digit, unsigned char c);
    void updateDisplay();
    bool myDigitalRead(char pin);

  public:
    uint8_t led_key=0;  //1=blue, 2=yellow

    void update();
    void signalUpdateDisplay();
    bool getBufferedPin(uint8_t pin);
    void displayNumber(int i);
    void displayFloat(float f);
    void displayChannel(uint8_t c);
    void displayPPQ(uint8_t ppq, bool positivePulse);
};

#endif
