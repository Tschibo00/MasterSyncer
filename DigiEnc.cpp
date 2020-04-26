#include "DigiEnc.hpp"

void DigiEnc::process() {
  bool _a=this->_hwc->getBufferedPin(KEY_ENC_L);
  bool _b=this->_hwc->getBufferedPin(KEY_ENC_R);

  _lastValQuad=_valQuad;
  // clockwise
  if (_lastA&&_lastB&&_a&&!_b)  _valQuad++;
  if (!_lastA&&_lastB&&_a&&_b)  _valQuad++;
  if (!_lastA&&!_lastB&&!_a&&_b)  _valQuad++;
  if (_lastA&&!_lastB&&!_a&&!_b)  _valQuad++;
  // counter-clockwise
  if (_lastA&&_lastB&&!_a&&_b)  _valQuad--;
  if (!_lastA&&_lastB&&!_a&&!_b)  _valQuad--;
  if (!_lastA&&!_lastB&&_a&&!_b)  _valQuad--;
  if (_lastA&&!_lastB&&_a&&_b)  _valQuad--;
  
  _lastA=_a;
  _lastB=_b;

  if (_a&&_b) {
    int32_t _stepSize;
    if (_valQuad!=0)
      _deltaLastUpdate=millis()-_lastUpdate;
    if ((_deltaLastUpdate>=23)||(!_dynamic))
      _stepSize=1;
    else
      _stepSize=23-_deltaLastUpdate;
    if (_valQuad<0)
      val-=_stepSize;
    if (_valQuad>0)
      val+=_stepSize;
    if (val>_max){
      if (_wrapping)
        val=_min;
      else
        val=_max;
    }
    if (val<_min){
      if (_wrapping)    // wrapping needs to be adapted if step is larger than 1
        val=_max;
      else
        val=_min;
    }
    _valQuad=0;
    _lastUpdate=millis();
  }
}
