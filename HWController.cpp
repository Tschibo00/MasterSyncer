#include "HWController.hpp"
#include "led_font.hpp"

void HWController::update(){
  // update of display and keys only every 1ms, otherwise display gets smeared and encoding reading is off
  displayUpdateThrottle--;
  if (displayUpdateThrottle>=0) return;
  displayUpdateThrottle=10;

  if (this->canUpdateDisplay){
    updateDisplay();
    canUpdateDisplay=false;
  }

  if (this->currentDigit==1){
    keyStart=!myDigitalRead(A6);
    keySelect=!myDigitalRead(A7);
  }
  if (this->currentDigit==2){
    encL=myDigitalRead(A6);
    encR=myDigitalRead(A7);
  }
}

void HWController::writeDigit(uint8_t digit, unsigned char c){
  digit=1<<(3-digit);

  // bit shuffling because the segments are connected rather weirdly :)
  PORTB=PORTB&0b11000001|digit|((c&1)<<5)|((c&32)>>1);
  PORTC=PORTC&0b11000000|((c&64)>>1)|((c&128)>>3)|((c&4)<<1)|((c&2)<<1)|((c&8)>>2)|((c&16)>>4);
}

// call this frequently (at least about 300 times/second) to update LEDs based on led[], led_dot and led_key
void HWController::updateDisplay(){
  unsigned char c=led[currentDigit];
  if (c>95) c-=32;  // map a-z to A-Z
  uint8_t dotC=0;
  if (currentDigit==0&&!!(led_key&1)) dotC|=1;
  if (currentDigit==1&&led_dot) dotC|=1;
  if (currentDigit==2&&!!(led_key&2)) dotC|=1;
  writeDigit(currentDigit, led_font[c-32]|dotC);
  currentDigit=(currentDigit+1)%3;
}

void HWController::signalUpdateDisplay(){
  this->canUpdateDisplay=true;
}

bool HWController::myDigitalRead(char pin){
  int v=analogRead(pin);
  if (v<400) return false; else return true;
}

bool HWController::getBufferedPin(uint8_t pin){
  switch(pin){
    case KEY_START: return keyStart;
    case KEY_SELECT: return keySelect;
    case KEY_ENC_L: return encL;
    case KEY_ENC_R: return encR;
  }
}

void HWController::displayNumber(int i){
  if (i>=100) led[0]=(i/100)+48; else led[0]=' ';
  if (i>=10) led[1]=((i/10)%10)+48; else led[1]=' ';
  led[2]=(i%10)+48;
  led_dot=false;
}

void HWController::displayFloat(float f){
  int i=(int)(f*10.0f);
  displayNumber(i);
  if (f<1.0f) led[1]='0';
  led_dot=true;
}

void HWController::displayChannel(uint8_t c){
  led[0]='C';
  led[1]='h';
  led[2]=c+48;
  led_dot=true;
}

void HWController::displayPPQ(uint8_t ppq, bool positivePulse){
  displayNumber(ppq);
  if (positivePulse) led[0]='['; else led[0]=']';
  led_dot=false;
}
