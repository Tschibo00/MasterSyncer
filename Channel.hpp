#ifndef CHANNEL
#define CHANNEL

#include "HWController.hpp"
#include "DigiEnc.hpp"

#define STATE_CHANNEL 0
#define STATE_PPQ 1
#define STATE_LENGTH 2

class Channel {
  private:
    int _ppq=4;
    int _ppqDivider=12 ;
    bool _positivePulse=true;
    HWController *_hwc;
    int syncTypeppqList[8]={2,4,24,48,2,4,24,48};
    bool syncTypePulseList[8]={true,true,true,true,false,false,false,false};
    DigiEnc *_encLength;
    DigiEnc *_encSyncType;
    uint8_t _pulseCount=0;
    uint8_t _channelNumber;
    
  public:
    uint8_t displayState=STATE_CHANNEL;

    Channel(HWController *hwc,uint8_t channelNumber);
    void setSyncType(int ppq, bool positivePulse);
    void processEnc();
    bool getTrigger(){
      if (_pulseCount>0){
        _pulseCount--;
        return _positivePulse;
      } else
        return !_positivePulse;
    }
    void setPos(uint8_t pos);
    void display();
    uint8_t getDisplayState(){
      return displayState;
    }
    bool isUsed();
};

#endif
