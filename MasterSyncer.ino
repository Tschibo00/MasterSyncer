#include "DigiEnc.hpp"
#include "KeyboardController.hpp"
#include "HWController.hpp"
#include "Channel.hpp"
#include <EEPROM.h>

/*
 * D11 = select digit 1, blue, start/stop, select
 * D10 = select digit 2, yellow, enc L/R
 * D9  = select digit 3+decimal point
 * 
 * A0-A6,D12,D12 = segment A-H, DP/blue/yellow
 * A0=D,A1=E,A2=G,A3=F,A4=A,A5=B,D12=C,D13=DP
 * A7 = select, ENC r
 * A6 = start/stop, ENC l
 */

#define NUM_CHANNELS 9
//#define TIME_MEASURE    // define to run in time measurement mode to adjust for Arduino's imprecise quartz
                        // adjust OCR1A in setup => decrease if to slow, increase if too fast

DigiEnc *encBPM, *encChannel;
KeyboardController *keys;
HWController *hwc;
long countdownFallbackToBPMDisplay=0;
unsigned char currentChannel=1;
int pos=0;                // position in track, i.e. 16 steps and 48PPQ=>192 positions per track (is divided if lower PPQ is configured for channel)
bool isRunning=false;
unsigned long bpmWaiter=200;  // number of interrupts until pos advances
unsigned long bpmPos=0;

Channel *channels[NUM_CHANNELS];

void setup() {
  DDRB=DDRB&0b11000000|0b00111111;  //set D8-D13 to output
  DDRC=DDRC&0b11000000|0b00111111;  //set A0-A5 to output
  DDRD=DDRD&0b00000011|0b11111100;  //set D2-D7 to output
  hwc=new HWController();
  encBPM=new DigiEnc(hwc,1,999,false,true);
  encChannel=new DigiEnc(hwc,1,NUM_CHANNELS,true,false);
  keys=new KeyboardController(hwc);

  for (unsigned char c=0;c<NUM_CHANNELS;c++) channels[c]=new Channel(hwc,c+1);

  cli();//disable interrupts
  //set timer1 interrupt at 10000Hz
  TCCR1A = 0;                   // set entire TCCR1A register to 0
  TCCR1B = 0;                   // same for TCCR1B
  TCNT1 = 0;                    //initialize counter value to 0
  OCR1A=1508;                   // (16.000.000/10000)-1 = 1600-1
  TCCR1B |= (1 << WGM12);       // turn on CTC mode
  TCCR1B |= 1 << CS10;          // Set prescaler=1
  TIMSK1 |= (1 << OCIE1A);      // enable timer compare interrupt
  sei();//enable interrupts

  encBPM->val=(EEPROM.read(0)<<8)+EEPROM.read(1);
  if (encBPM->val==65535)
    encBPM->val=120;
}

#ifdef TIME_MEASURE
long timetest=0;
#endif

// maybe needs a little adjustment, as 10000 IRQs are 1.05s long by measurement via micros()
// 58 beats in 60 seconds
// so either the interrupt is off or the micros() timer is off :)
ISR(TIMER1_COMPA_vect){
  #ifdef TIME_MEASURE
  if (isRunning)
    timetest++;
  hwc->update();
  keys->readKeys();
  return;
  #endif
  
  if (isRunning){
    if (pos==0)
      hwc->led_key=1;
    else
      hwc->led_key=pos%48==0?2:0;
    bpmPos++;
    if (bpmPos>=bpmWaiter){
      for (uint8_t i=0;i<NUM_CHANNELS;i++)
        channels[i]->setPos(pos);
      bpmPos-=bpmWaiter;
      pos++;
      if (pos==192) pos=0;
    }
  } else {
    bpmPos=bpmWaiter; // ensure that the sequence starts directly with first trigger if start is pressed
    pos=191;
  }
  uint8_t portD=0;    // D0-D7
  for (uint8_t i=1;i<9;i++)
    portD=portD<<1 | channels[i]->getTrigger();
  PORTD=portD;
  PORTB=PORTB&0b11111110|channels[0]->getTrigger();

  // user interface stuff
  countdownFallbackToBPMDisplay--;
  if (countdownFallbackToBPMDisplay<0) countdownFallbackToBPMDisplay=0;
  
  hwc->update();
  keys->readKeys();
}

void loop() {
#ifdef TIME_MEASURE
  while(1){
    if (keys->getKeyClick(0))
    isRunning=true;
      hwc->displayNumber(timetest/10000);
    hwc->signalUpdateDisplay();
    delay(1);       // avoid too frequent updates to the display
  }
#endif
  
  while(1){
    if (countdownFallbackToBPMDisplay>0){   // channel selection mode
      if (keys->getKeyClick(1)){
        channels[currentChannel-1]->displayState++;
        if (channels[currentChannel-1]->displayState>STATE_LENGTH){
          channels[currentChannel-1]->displayState=STATE_CHANNEL;
          countdownFallbackToBPMDisplay=15000;
        } else {
          countdownFallbackToBPMDisplay=50000;
        }
      }
      switch(channels[currentChannel-1]->displayState){
        case STATE_CHANNEL:
          encChannel->process();
          currentChannel=encChannel->val;
          if (encChannel->isUsed()){
            countdownFallbackToBPMDisplay=15000;
          }
          break;
        case STATE_PPQ:
          channels[currentChannel-1]->processEnc();
          if (channels[currentChannel-1]->isUsed())
            countdownFallbackToBPMDisplay=50000;
          break;
        case STATE_LENGTH:
          channels[currentChannel-1]->processEnc();
          if (channels[currentChannel-1]->isUsed())
            countdownFallbackToBPMDisplay=50000;
          break;
      }
      channels[currentChannel-1]->display();
    } else {
      if (keys->getKeyClick(1)){
        countdownFallbackToBPMDisplay=15000;
        channels[currentChannel-1]->displayState=STATE_CHANNEL;
      }
      encBPM->process();
      if (encBPM->isUsed()){
        EEPROM.update(0,(encBPM->val)>>8);
        EEPROM.update(1,(encBPM->val)&255);
      }
      bpmWaiter=12500/encBPM->val;
      hwc->displayNumber(encBPM->val);
    }
    if (keys->getKeyClick(0))
      isRunning=!isRunning;
    hwc->signalUpdateDisplay();
    delay(1);       // avoid too frequent updates to the display
  }
}
