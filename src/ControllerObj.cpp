#include "core_pins.h"
#include "Logger.h"
#include "ControllerObj.h"
#include "I2CNetwork.h"


String ControllerObject::name()  { return _name; }
int ControllerObject::pin()  { return _pin; }
int ControllerObject::remoteAddress()  { return _remoteAddress; }
int ControllerObject::remoteIndex()  { return _remoteIndex; }

String ControllerObject::performAction(int groupState=0){
  
  return "Error";
}

String ControllerObject::performControllerAction(String action,int state, int groupState=0){
  //_logger->debug("Perform action "+String(_name) +" action "+String(action)+" state "+String(state));  
  if(_remoteAddress>=0){
    unsigned long start = millis();
    String result = _i2c->addToQueue(_remoteAddress,_remoteIndex,action,state);

    unsigned long end = millis();
    if(start-end>1 )
      _logger->debug("ControllerObject: "+name()+" adding to queue, action "+String(action)+" on "+String(_remoteAddress)+" took " +String(end-start));

    if(result!=""){
      _logger->log(result);  
    }
    //return "Q-"+String(_remoteIndex);
    return _name;//+"-"+action;
  }
  return "";
}

int ControllerObject::getState(){
  return _pinState;
}

void ControllerObject::setState(int state){
  _pinState=state;
}

void ControllerObject::initialize(){
  
}

void ControllerObject::deinitialize(){
  
}

bool ControllerObject::isDigital(){
  return false;
}

void ControllerObject::setGroup(int group){
  _group=group;
}
int ControllerObject::getGroup(){
  return _group;
}

void ControllerObject::count(int count){
  
}

template <unsigned NumISR>
auto ControllerObject::get_isr(unsigned interrupt) -> isr_func_t {
    return interrupt == NumISR - 1
               ? []() IC_ISR_ATTR { instance_table[NumISR - 1]->interrupt(); }
               : get_isr<NumISR - 1>(interrupt); // Compile-time tail recursion
}

template <>
inline auto ControllerObject::get_isr<0>(unsigned) -> isr_func_t {
    return nullptr;
}

void ControllerObject::attachInterruptCtx(int interrupt) {
    //_logger->debug("ControllerObj: attaching ctx on pin "+String(interrupt));
    if (attached) {
        _logger->log(F("Error: This instance was attached already"));
        return;
    }
    if (interrupt == NOT_AN_INTERRUPT) {
        _logger->log(F("Error: Not an interrupt-capable pin, check your config"));
        return;
    }
    if (instance_table[interrupt] != nullptr) {
        _logger->log(F("Error: Multiple instances on the same pin, check your config"));
        return;
    }
    instance_table[interrupt] = this;
    attached = true;
    attachInterrupt(interrupt, get_isr(interrupt), CHANGE);
    //_logger->debug("ControllerObj: attaching ctx on pin "+String(interrupt)+" "+String(attached));
}

void ControllerObject::detachInterruptCtx(int interrupt) {
  //_logger->debug("ControllerObj: detaching ctx on pin "+String(interrupt));
  detachInterrupt(interrupt);
  attached = false;
  instance_table[interrupt] = nullptr;
}

void ControllerObject::interrupt(){
  return;
}

ControllerObject *ControllerObject::instance_table[] {};