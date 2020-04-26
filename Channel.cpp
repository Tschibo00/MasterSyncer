#include "Channel.hpp"
#include <EEPROM.h>

Channel::Channel(HWController *hwc,uint8_t channelNumber) {
  _encLength=new DigiEnc(hwc,1,99,false,true);
  _encSyncType=new DigiEnc(hwc,0,7,true,false);
  _hwc=hwc;
  _channelNumber=channelNumber;
  if (EEPROM.read(_channelNumber*2)!=255)
    _encLength->val=EEPROM.read(_channelNumber*2);
  else
    _encLength->val=10;
  if (EEPROM.read(_channelNumber*2+1)!=255)
    _encSyncType->val=EEPROM.read(_channelNumber*2+1);
  else
    _encSyncType->val=1;
}

void Channel::processEnc(){
  if (displayState==STATE_PPQ) {
    _encSyncType->process();
    setSyncType(syncTypeppqList[_encSyncType->val], syncTypePulseList[_encSyncType->val]);
    if (_encSyncType->isUsed())
      EEPROM.update(_channelNumber*2+1,_encSyncType->val);
  }
  if (displayState==STATE_LENGTH) {
    _encLength->process();
    if (_encLength->isUsed())
      EEPROM.update(_channelNumber*2,_encLength->val);
  }
}

void Channel::setSyncType(int ppq, bool positivePulse){
  _positivePulse=positivePulse;
  _ppq=ppq;
  _ppqDivider=48/_ppq;
}

void Channel::display(){
  switch(displayState){
    case STATE_CHANNEL: _hwc->displayChannel(_channelNumber); break;
    case STATE_PPQ:     _hwc->displayPPQ(_ppq, _positivePulse); break;
    case STATE_LENGTH:  _hwc->displayFloat(((float)_encLength->val)/10.f); break;
  }
}

bool Channel::isUsed(){
  return (displayState==STATE_PPQ && _encSyncType->isUsed()) || (displayState==STATE_LENGTH && _encLength->isUsed());
}

void Channel::setPos(uint8_t pos){
  if (pos%_ppqDivider==0)
    _pulseCount=_encLength->val;
}
