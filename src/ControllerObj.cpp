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
    
    String result = _i2c->addToQueue(_remoteAddress,_remoteIndex,action,state);
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