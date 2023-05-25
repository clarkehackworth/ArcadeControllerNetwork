#include "Logger.h"
#include "ControllerObj.h"
#include "I2CNetwork.h"
#include "DigitalControllerObj.h"

//DigitalControllerObject::DigitalControllerObject() {}
DigitalControllerObject::DigitalControllerObject(String name,int pin, String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize,Logger* logger) {//standard all in one loca pin/xbox combo
  _name=name;
  _pin = pin;
  //_xboxButton=xboxButton;
  _logger = logger;
  _xboxButtonsSize= xboxButtonsSize;
  _xboxButtons = new String[_xboxButtonsSize];
  for(int i=0;i<_xboxButtonsSize;i++){
    _xboxButtons[i]= String(xboxButtons[i]);
  }
  _xboxButtonsSizeDoubleTap= xboxButtonsDoubleTapSize;
  if(_xboxButtonsSizeDoubleTap>0){
    doubleTapEnabled = true;
    _xboxButtonsDoubleTap = new String[_xboxButtonsSizeDoubleTap];
    for(int i=0;i<_xboxButtonsSizeDoubleTap;i++){
      _xboxButtonsDoubleTap[i]= String(xboxButtonsDoubleTap[i]);
    }
  }

}
DigitalControllerObject::DigitalControllerObject(String name,int pin, String remoteAddress,String remoteIndex,int emulateAnalog,I2CNetwork* i2c,Logger* logger){ //remote pin side
   _name = name;
   _pin = pin;
   _remoteAddress=(int)remoteAddress.toInt();
   _remoteIndex=(int)remoteIndex.toInt();
   _emulateAnalog = emulateAnalog;
   _i2c = i2c; 
   _logger = logger;
}
DigitalControllerObject::DigitalControllerObject(String name,String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize,Logger* logger){//For remote to trigger
  _name = name;
  //_xboxButton = xboxButton;
  _logger = logger;
  _xboxButtonsSize= xboxButtonsSize;
  
  _xboxButtons = new String[_xboxButtonsSize];
  for(int i=0;i<_xboxButtonsSize;i++){
    _xboxButtons[i]= String(xboxButtons[i]);
  }
  
  _xboxButtonsSizeDoubleTap= xboxButtonsDoubleTapSize;
  if(_xboxButtonsSizeDoubleTap>0){
    doubleTapEnabled = true;
    _xboxButtonsDoubleTap = new String[_xboxButtonsSizeDoubleTap];
    for(int i=0;i<_xboxButtonsSizeDoubleTap;i++){
      _xboxButtonsDoubleTap[i]= String(xboxButtonsDoubleTap[i]);
    }
  }
} 

uint8_t DigitalControllerObject::inputType()  { return _inputType; }
uint8_t DigitalControllerObject::test()  { return _test; }
String DigitalControllerObject::xboxButton()  { 
  if(doubleTapEnabled && doubleTapActivated)
    return _xboxButtonsDoubleTap[_group];
  return _xboxButtons[_group]; 
}

String DigitalControllerObject::performAction(int groupState){//int groupState
  
  int state = getState();
  //_logger->debug("Perform action "+String(_name) +" state "+String(_xboxState)+" "+String(state));
  
  unsigned long curTime = millis();
  if(doubleTapEnabled && doubleTapStarted && _doubleTapTime+_doubleTapDelay<curTime){
    //_logger->debug("doubleTap deactivate "+String(doubleTapRevertState1)+","+String(doubleTapRevertState2));
    doubleTapTimedOut=true;
    doubleTapStarted = false;
    doubleTapActivated = false;
    _doubleTapTime= 0;
  }
  if(doubleTapTimedOut){
    if(doubleTapRevertState1>=0){
      //_logger->debug("doubleTap replay state 1 "+String(_xboxState)+"-"+String(doubleTapRevertState1)+"-"+String(_test));
      state = doubleTapRevertState1;
      doubleTapRevertState1=-1;
      _xboxState=false;
      doubleTapRevert=true;
    }else if(doubleTapRevertState2>=0){
      //_logger->debug("doubleTap replay state 2 "+String(_xboxState)+"-"+String(doubleTapRevertState2)+"-"+String(_test));
      state = doubleTapRevertState2;
      doubleTapRevertState2=-1;
      _xboxState=true;
      doubleTapRevert=true;
    }else{
      //_logger->debug("doubleTap timout clear");
      doubleTapTimedOut=false;
      doubleTapRevert=false;
    }
  }

  //emulated state check
  if(_emulateAnalog!=0){
    if(_xboxState){
      if(state!=_test){ // hw key let up
        _emulateLongPressed=false;
        return performControllerAction("EmulateRelease",_emulateAnalog);
      }
      unsigned long currentTime = millis();
      if(!_emulateLongPressed && currentTime >= (_emulateTime+Emulate_Double_Time)){
        _emulateLongPressed=true;
        int value = 100;
        if(_emulateAnalog<0)
          value=-100;
        return performControllerAction("EmulatePress",value);
      }
    }else{
      //xbox key not in pressed state 
      if(state==_test){ // hw key pressed
        _emulateTime = millis();
        return performControllerAction("EmulatePress",_emulateAnalog);
      }
    }  
  }
  
  

  //regular state check
  if(_xboxState){
    //xbox key in pressed state
    if(state!=_test){ // hw key let up
      
      if(doubleTapEnabled && doubleTapStarted && !doubleTapRevert){
        //_logger->debug("doubleTap release activate "+String(state));
        doubleTapRevertState2 = state;
        _xboxState=false;
        return "";
      }
      String result = performControllerAction("Release",state);
      if(doubleTapEnabled && doubleTapActivated){
        //_logger->debug("doubleTap end");
        doubleTapActivated = false;
      }
      return result;
    }
  }else{
    //xbox key not in pressed state 
    if(state==_test){ // hw key pressed
      
      if(doubleTapEnabled && !doubleTapRevert){
        //_logger->debug("doubleTap check: "+String(_name)+" "+String(state)+" "+String(doubleTapEnabled)+ "-"+String(doubleTapStarted)+"-"+String(_doubleTapTime)+"->"+String(curTime)+"-"+String(doubleTapRevertState1)+"-"+String(doubleTapRevertState2));
        if(!doubleTapStarted){
          //_logger->debug("doubleTap press activate "+String(state));
          doubleTapStarted = true;
          doubleTapRevertState1 = state;
          _doubleTapTime= curTime;
          _xboxState=true;
          return "";
        }else if(!doubleTapActivated){
          //_logger->debug("doubleTap full activate");
          doubleTapActivated=true;
          doubleTapStarted = false;
          doubleTapRevertState1 =-1;
          doubleTapRevertState2 =-1; 
        }
      }
    
      return performControllerAction("Press",state);
    }
  }
  return "";
}

String DigitalControllerObject::performControllerAction(String action,int state, int groupState){
  //_logger->debug("Perform action "+String(_name) +" action "+String(action)+" state "+String(state));  
  if(action=="EmulateJoystick"){
    if(state==1 && _xboxState){
      return performControllerAction("Release",state);
    }
    if(state==0 && !_xboxState){
      return performControllerAction("Press",state);
    }
    //_logger->debug("DigitalControllerObject: "+String(_name)+" state:"+String(state));
    return "Error: EmulateJoystick out of sync"; 
  }
  
  String result = ControllerObject::performControllerAction(action,state);
  //_logger->debug("Perform action base result "+String(result));  
  if(result!=""){
    if(!_xboxState && action.endsWith("Press")){
      _xboxState = true;
      return result+"-Press";
    }
    if(_xboxState && action.endsWith("Release")){
      _xboxState = false;
      return result+"-Release";
    }
    return result;
  } else {
    int button = xboxlookup(xboxButton());
    
    //_logger->debug("DigitalController "+_name+" "+action+" "+xboxButton()+" "+String(_pin));
    if(!_xboxState && action=="Press"){
      
      XInput.press(button);
      _xboxState=true;
      return _name+"-"+action;
    }
    if(_xboxState && action=="Release"){
      XInput.release(button);
      _xboxState = false;
      return _name+"-"+action;
    }
  }
  //_xboxState=!_xboxState;//correct invalid state
  return "Error-Unknown:"+_name+"-"+action+"-"+xboxButton()+"-"+String(_pin)+"-"+String(state)+"-"+String(_xboxState);
}

int DigitalControllerObject::getState(){
  if(_pin>=0)
    return digitalRead(_pin);
  return ControllerObject::getState();
}

void DigitalControllerObject::initialize(){
  ControllerObject::initialize();
  _logger->debug("Initialize "+String(_name)+" on "+String(_pin)+" type "+String(_inputType));
  if(_pin>=0)
    pinMode(_pin,_inputType);
}

bool DigitalControllerObject::isDigital(){
  return true;
}