#ifndef DIGIENC
#define DIGIENC

#include "HWController.hpp"

class DigiEnc {
  private:
    int32_t _min;
    int32_t _max;
    bool _wrapping=false;
    bool _dynamic=true;

    bool _lastA=true;
    bool _lastB=true;
    int8_t _valQuad=0;
    int8_t _lastValQuad=0;
    unsigned long _lastUpdate=0;
    unsigned long _deltaLastUpdate=0;
    HWController *_hwc;

  public:
    int32_t val=0;
    bool pinA, pinB;
  
    DigiEnc(HWController *hwc, int32_t _min=0, int32_t _max=999, bool _wrapping=false, bool _dynamic=true){
      this->_min=_min;
      this->_max=_max;
      this->_wrapping=_wrapping;
      this->_dynamic=_dynamic;
      this->_hwc=hwc;
      this->val=_min;
    }

    void process();     // should be called at least 250 times/second to ensure errorfree processing even for fast turning
    bool isUsed(){
      return _lastValQuad!=_valQuad;
    }
};

#endif
