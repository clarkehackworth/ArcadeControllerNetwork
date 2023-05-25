#include "Logger.h"
#include "ControllerObj.h"
#include "I2CNetwork.h"
#include "AnalogControllerObj.h"


AnalogControllerObject::AnalogControllerObject(String name,int pin,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,Logger* logger){//local
  _name=name;
  _pin = pin;
  _xboxref = xboxref;
  _axis = axis;
  if(smoothing>0)
    _smoothing = smoothing;
  if(sensitivity>=0)
    _sensitivity = sensitivity;
  if(deadzone>=0)
    _deadzone = deadzone;
  _invert = invert;
  _offset = offset;

  _logger = logger;
}
AnalogControllerObject::AnalogControllerObject(String name,int pin, String axis, int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,I2CNetwork* i2c,Logger* logger){//remote sender
  _name=name;
  _pin = pin;
  _axis = axis;
  if(smoothing>0)
    _smoothing = smoothing;
  if(sensitivity>=0)
    _sensitivity = sensitivity;
  if(deadzone>=0)
    _deadzone = deadzone;
  _offset = offset;
  _invert = invert;

  _emulateDigital=emulateDigital;
  _index=index;
  _emulateDigitalMinus=emulateDigitalMinus;
  _indexMinus=indexMinus;

  _remoteAddress=(int)remoteAddress.toInt();
  _remoteIndex=(int)remoteIndex.toInt();
  _i2c = i2c; 
  _logger = logger;
}
AnalogControllerObject::AnalogControllerObject(String name, String xboxref, String axis,Logger* logger){//remote reciever
  _name=name;
  _xboxref = xboxref;
  _axis = axis;
  _logger = logger;
}


String AnalogControllerObject::performAction(int groupState){
  //_logger->debug("AnalogControllerObj: performAction");
  int state = getState();
  //_logger->debug("AnalogControllerObj: performAction " +String(state));
  _total -= _readings[_readIndex];
  _readings[_readIndex] = state;
  _total += _readings[_readIndex];
  _readIndex++;
  if(_readIndex >= _smoothing)
     _readIndex = 0;
  
  _average = _total / _smoothing;
  int diff = _average - _prev;
 
  int value = _average+_offset;
  value = calcDeadzone(value);
  if(_invert)
    value = AnalogRead_Max - value;
  //_logger->debug("AnalogControllerObj: performAction value " +String(value));
  if(diff > _sensitivity || diff < -_sensitivity){
    String axisInfo = "";
    if(_axis!="")
      axisInfo = " on "+_axis;
    //_logger->debug("AnalogControllerObj: controller "+_name+" value"+axisInfo+": " +String(value));
    String result = "";
    if(_emulateDigital>=0){ // emulating button
      result = peformEmulateButton(value);
    }else{//not emulating button
      result = performControllerAction(_xboxref+_axis,value);
    }
    _prev=_average;
    return result;
  }
  return "";
}

String AnalogControllerObject::performControllerAction(String action,int state, int groupState){
  //emulate joystick
  if(action=="EmulatePress"){
    int value = (AnalogRead_Max/2.0)+(AnalogRead_Max/2.0)*(state/100.0);
    value = calcDeadzone(value);
    _logger->debug("AnalogControllerObj: press controller "+_name+" value: " +String(value));
    setReadings(value);
    setState(value);
    //_logger->debug("AnalogController: state:"+String(state)+" max:"+String(AnalogRead_Max)+" value:"+String(value));
    return performControllerAction(_xboxref+_axis,value)+"-Press";
  }
  if(action=="EmulateRelease"){
    int value = (AnalogRead_Max/2.0);
    value = calcDeadzone(value);
    _logger->debug("AnalogControllerObj: release controller "+_name+" value: " +String(value));
    setReadings(value);
    setState(value);
    return performControllerAction(_xboxref+_axis,value)+"-Release";
  }

  String result = ControllerObject::performControllerAction(action,state);
  if(result==""){
    if(action=="JOY_LEFTX"){
      XInput.setJoystickX(JOY_LEFT, state);
      //return "X-"+action;
      return _name;
    }
    if(action=="JOY_LEFTY"){
      XInput.setJoystickY(JOY_LEFT, state);
      //return "X-"+action;
      return _name;
    }
    if(action=="JOY_RIGHTX"){
      XInput.setJoystickX(JOY_RIGHT, state);
      //return "X-"+action;
      return _name;
    }
    if(action=="JOY_RIGHTY"){
      XInput.setJoystickY(JOY_RIGHT, state);
      //return "X-"+action;
      return _name;
    }
    if(action=="TRIGGER_LEFT"){
      XInput.setTrigger(TRIGGER_LEFT, state);
      //return "X-"+action;
      return _name;
    }
    if(action=="TRIGGER_RIGHT"){
      XInput.setTrigger(TRIGGER_RIGHT, state);
      //return "X-"+action;
      return _name;
    }
  }
  if(_emulateDigital>=0 && _index>=0){ // catch for emulating button
    return _name;
  }
  return "Error-Unknown:"+_name+"-"+action+"-"+_xboxref+"-"+String(_pin)+"-"+String(state)+"-"+result;
}

String AnalogControllerObject::peformEmulateButton(int state){
  int adjustedValue = -1;
  int adjustedRemoteIndex = -1;
  int stateAdjusted =-1;
  if(_axis==""){
    //_logger->debug("AnalogControllerObj: controller emlulated button trigger formula");
    stateAdjusted = int((float(state)/float(AnalogRead_Max))*100);
    //_logger->debug("AnalogControllerObj: controller emlated trigger "+String(stateAdjusted)+ " compared to "+String(_emulateDigital)+" - "+String(_xboxEmulatedButtonState));
    adjustedRemoteIndex = _index;
  }else{
    
    if(state>=CENTER){
      //_logger->debug("AnalogControllerObj: controller emlulated button trigger upper formula");
      stateAdjusted = int(((float(state)-CENTER)/(float(AnalogRead_Max)/2.0))*100);
      adjustedRemoteIndex = _indexMinus;
    }else{
      //_logger->debug("AnalogControllerObj: controller emlulated button trigger lower formula");
      stateAdjusted = 100-int((float(state)/(float(AnalogRead_Max)/2.0))*100);
      adjustedRemoteIndex = _index;
    }
  }
  //_logger->debug("AnalogControllerObj: controller emlulated button "+String(_name)+" - "+String(stateAdjusted)+ " compared to "+String(_emulateDigital)+" - "+String(_xboxEmulatedButtonState));
  if(_xboxEmulatedButtonState && stateAdjusted >_emulateDigital){
    //_logger->debug("AnalogControllerObj: controller emlated button release ");
    //button release
    _xboxEmulatedButtonState = false;
    adjustedValue = 0;
  }else if(!_xboxEmulatedButtonState && stateAdjusted <_emulateDigital){
    //button press
    //_logger->debug("AnalogControllerObj: controller emlated button press ");
    _xboxEmulatedButtonState = true;
    adjustedValue = 1;
  }
  _remoteIndex = adjustedRemoteIndex;
  //set _remoteIndex
  if(adjustedValue>=0){
    //_logger->debug("AnalogControllerObj: controller emlated "+String(adjustedValue));
    return performControllerAction("EmulateJoystick",adjustedValue);
  }
  return "";
}

int AnalogControllerObject::calcDeadzone(int value){
  int _value = value;
  int deadzoneHigh = _deadzone;
  int deadzoneLow = 0;
  if(_axis!=""){//is joystick
    deadzoneHigh = CENTER+_deadzone;
    deadzoneLow = CENTER-_deadzone;
  }
  if(value<deadzoneHigh && value>deadzoneLow){
    String axisInfo = "";
    if(_axis!="")
      axisInfo = " on "+_axis;
    //_logger->debug("AnalogControllerObj: controller "+_name+" value"+axisInfo+": axis centered in deadzone");
    _value = CENTER;
  }
  return _value;
}

int AnalogControllerObject::getState(){
  if(_pin>=0){
    int reading = analogRead(_pin);
    return reading;
  }
  return ControllerObject::getState();
}

void AnalogControllerObject::initialize(){
  ControllerObject::initialize();
  _logger->debug("Initialize "+String(_name)+" on "+String(_pin)+", smoothing "+String(_smoothing));
  
  _readings = (int*)malloc(_smoothing*sizeof(int));
  setReadings(0);
  
  if(_axis!=""){
    XInput.setRange(xboxlookup(_xboxref), 0, AnalogRead_Max-1);//if joystick set range based on max of read analog. 
  }else{
    XInput.setTriggerRange(0, 255);//should be default, but lets set it
  }
}

bool AnalogControllerObject::isDigital(){
  return false;
}

void AnalogControllerObject::setReadings(int value){
  for(int i=0;i<_smoothing;i++){
    _readings[i] = value;
  }
}