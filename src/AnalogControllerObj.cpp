#include "pins_arduino.h"
#include "Logger.h"
#include "ControllerObj.h"
#include "I2CNetwork.h"
#include "AnalogControllerObj.h"



AnalogControllerObject::AnalogControllerObject(String name,String type,int pin,int pin2,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int rotarySpeed,int mouseMode,int debugDeadzone,Logger* logger){//local
  _name=name;
  _type = type;
  _pin = pin;
  _pin2 = pin2;
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
  if(rotarySpeed>0)
    _rotarySpeed = rotarySpeed;
  if(mouseMode>0)
    _mouseModeTimeout = mouseMode;
  if(debugDeadzone==1)
    _debugDeadzone=true;
  _logger = logger;
}
AnalogControllerObject::AnalogControllerObject(String name,String type,int pin,int pin2, String axis, int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,int rotarySpeed,int mouseMode,int debugDeadzone,I2CNetwork* i2c,Logger* logger){//remote sender
  _name=name;
  _type=type;
  _pin = pin;
  _pin2 = pin2;
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

  if(rotarySpeed>0)
    _rotarySpeed = rotarySpeed;
  if(mouseMode>0)
    _mouseModeTimeout = mouseMode;

  if(debugDeadzone==1)
    _debugDeadzone=true;

  _i2c = i2c; 
  _logger = logger;
}
AnalogControllerObject::AnalogControllerObject(String name,String type, String xboxref, String axis,Logger* logger){//remote reciever
  _name=name;
  _type=type;
  _xboxref = xboxref;
  _axis = axis;
  _logger = logger;
}


String AnalogControllerObject::performAction(int groupState){
  //_logger->debug("AnalogControllerObj: performAction");
  int state = getState();
  //_logger->debug("AnalogControllerObj: performAction "+String(_name)+"-" +String(state)+" "+String(_pin));
  performImplReturn result = performCalculations(state);
  //_logger->debug("AnalogControllerObj: result "+String(result.inact)+"-" +String(result.value));
  if(result.inact){
    if(_invert)
      result.value = AnalogRead_Max - result.value;
    if(_emulateDigital>=0){ // emulating button
      return peformEmulateButton(result.value);
      
    }else{//not emulating button
      //_logger->debug("AnalogControllerObj: performControllerAction " +String(result.value)+" "+String(_pin));
      return performControllerAction(_xboxref+_axis,result.value);
    }
  }
  return "";
}

String AnalogControllerObject::performControllerAction(String action,int state, int groupState){
  //emulate joystick
  if(action=="EmulatePress"){
    int value = (AnalogRead_Max/2.0)+(AnalogRead_Max/2.0)*(state/100.0);
    value = calcDeadzone(value);
    //_logger->debug("AnalogControllerObj: press controller "+_name+" value: " +String(value));
    setReadings(value);
    setState(value);
    //_logger->debug("AnalogController: state:"+String(state)+" max:"+String(AnalogRead_Max)+" value:"+String(value));
    return performControllerAction(_xboxref+_axis,value)+"-Press";
  }
  if(action=="EmulateRelease"){
    int value = (AnalogRead_Max/2.0);
    value = calcDeadzone(value);
    //_logger->debug("AnalogControllerObj: release controller "+_name+" value: " +String(value));
    setReadings(value);
    setState(value);
    return performControllerAction(_xboxref+_axis,value)+"-Release";
  }

  String result = ControllerObject::performControllerAction(action,state);
  if(result==""){
    //_logger->debug("AnalogControllerObj: perform controller action "+_name+" "+action+", value: " +String(state));
    if(_type=="joystick" || _type=="trigger"){
      xbox(action,state);
      return _name;
    }else if(_type=="mouse"){
      mouse(action,state);
      return _name;
    }
  }
  if(_emulateDigital>=0 && _index>=0){ // catch for emulating button
    return _name;
  }
  return "Error-Unknown:"+_name+"-"+action+"-"+_xboxref+"-"+String(_pin)+"-"+String(state)+"-"+result;
}

void AnalogControllerObject::xbox(String action,int state){
  if(action=="JOY_LEFTX"){
      XInput.setJoystickX(JOY_LEFT, state);
    }
    if(action=="JOY_LEFTY"){
      XInput.setJoystickY(JOY_LEFT, state);
    }
    if(action=="JOY_RIGHTX"){
      XInput.setJoystickX(JOY_RIGHT, state);
    }
    if(action=="JOY_RIGHTY"){
      XInput.setJoystickY(JOY_RIGHT, state);      
    }
    if(action=="TRIGGER_LEFT"){
      XInput.setTrigger(TRIGGER_LEFT, state);
    }
    if(action=="TRIGGER_RIGHT"){
      XInput.setTrigger(TRIGGER_RIGHT, state);
    }
}

void AnalogControllerObject::mouse(String action,int state){
  if(_axis!=""){
    state=CENTER-state;
  }else{
    state=255-state;
  }
  if(_axis=="X"){
    #ifdef MOUSE_INTERFACE
    Mouse.move(state, 0, 0);
    #endif
  }else if(_axis=="Y"){
    #ifdef MOUSE_INTERFACE
    Mouse.move(0, state, 0);
    #endif
  }else if(_axis==""){
    #ifdef MOUSE_INTERFACE
    Mouse.move(0, 0, state);
    #endif
  }
}

performImplReturn AnalogControllerObject::performCalculations(int state){
  //_logger->debug("AnalogControllerObj: performcalc "+_name+" - "+String(_smoothing));
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
  
  //_logger->debug("AnalogControllerObj: performcalc value "+_name+" - "+String(value));
  unsigned long currentTime = millis();
  if(diff > _sensitivity || diff < -_sensitivity){
    String axisInfo = "";
    if(_axis!="")
      axisInfo = " on "+_axis;
    //_logger->debug("AnalogControllerObj: controller "+_name+" value"+axisInfo+": " +String(value));
    //_logger->debug("AnalogControllerObj: performcalc "+_name+" - "+_xboxref+" value"+axisInfo+": " +String(value)+" - "+String(state)+","+String(_average)+","+String(_total)+","+String(_readIndex));
    _prev=_average;
    _lastUpdate = currentTime;
    return {value,true};
  }else if(_mouseMode){
    int targetValue = CENTER;
    if(_axis=="")
      targetValue = 255;
    if(value!=targetValue && (value >= targetValue || value <= targetValue) && _lastUpdate+_mouseModeTimeout<currentTime){
      int newState = value+((targetValue-value)/2);
      newState = calcDeadzone(newState);
      newState = newState+_offset;
      _lastUpdate = currentTime;
      setReadings(newState);
      //_logger->debug("AnalogControllerObj: mouse mode "+String(value)+"->"+String(newState)+" of "+String(targetValue));
      //return performCalculations(newState);
      return {newState, true};
    }
    return {0,false};
  }
  return {0,false};
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
  String axisInfo = "";
  if(_axis!="")
    axisInfo = " on "+_axis; 
  if(value<deadzoneHigh && value>deadzoneLow){
    if(_debugDeadzone)
      _logger->log("Debug Deadzone: "+_name+axisInfo+": axis centered in deadzone with value "+String(value)+" of 511 (center)");
    _value = CENTER;
  }else{
    if(_debugDeadzone)
      _logger->log("Debug Deadzone: "+_name+axisInfo+": axis not in the deadzone with value "+String(value)+" of 511 (center)");
  }
  return _value;
}

int AnalogControllerObject::getState(){
  if(_pin>=0 && _pin2<0){
    if(!attached)
      _pinState = analogRead(_pin);
    // String axisInfo = "";
    // if(_axis!="")
    //   axisInfo = " on "+_axis; 
    // _logger->debug("AnalogControllerObj: controller "+_name+axisInfo+" reading "+String(_pinState));
    return _pinState;
  }else if(_pin>=0 && _pin2>=0){

    if(!attached)
      rotaryEncoder->tick();
    int position = rotaryEncoder->getPosition();
    int rpm = rotaryEncoder->getRPM();
    if(position!=rotaryPrevPosition){
      int dir = (int)rotaryEncoder->getDirection(); //0, 1, or -1
      
   
      rotaryPosition = rotaryPosition + ((position - prevPosition)*((rpm/3)*(_rotarySpeed/100.0)));
      
      if(rotaryPosition<0)
        rotaryPosition=0;
      if(rotaryPosition>AnalogRead_Max)
        rotaryPosition=AnalogRead_Max;
      if(rotaryPosition != rotaryPrevPosition){

        //_logger->debug("AnalogControllerObj: controller "+_name+" rotary read "+String(position)+" "+String(dir)+" "+String(rpm)+" - "+String(rotaryPosition));
        rotaryPrevPosition = rotaryPosition;
      }
      prevPosition = position;
      
      return rotaryPosition;
    }
    return rotaryPosition;
  }
  return ControllerObject::getState();
  
}

void AnalogControllerObject::initialize(){
  ControllerObject::initialize();
  _logger->debug("Initialize "+String(_name)+" on "+String(_pin)+","+String(_pin2)+", smoothing "+String(_smoothing));
  
  if(_pin>=0 && _pin2<0){
    pinMode(_pin,INPUT);
    //attachInterruptCtx(digitalPinToInterrupt(_pin)); not a digital input
    
    //defaults for joysticks
    if(_smoothing==-1)
      _smoothing = 6; //higher is more smoothing, but more latency
    if(_sensitivity==-1)
      _sensitivity = 3; //higher is less sensitive
    if(_deadzone==-1)
      _deadzone = 8;
  } else if(_pin>=0 && _pin2>=0){
    _mouseMode = true;//TODO: for now
    pinMode(_pin,INPUT);
    pinMode(_pin2,INPUT);
    rotaryEncoder = new RotaryEncoder(_pin, _pin2, RotaryEncoder::LatchMode::TWO03);
    attachInterruptCtx(digitalPinToInterrupt(_pin));
    attached = false; // we need to attach two pins
    attachInterruptCtx(digitalPinToInterrupt(_pin2));

    //defaults for tackball
    if(_smoothing==-1)
      _smoothing = 4; //higher is more smoothing, but more latency //old default 8
    if(_sensitivity==-1)
      _sensitivity = 2; //higher is less sensitive //old default 3
    if(_deadzone==-1)
      _deadzone = 1;//old default 8
    if(_mouseModeTimeout<=0){//default configs
      _mouseModeTimeout = 20;
      _mouseMode=true;
    }else if(_mouseModeTimeout==-1){ //disabled in config
      _mouseMode=false;
    }

  }else{
    //defaults for items with no pins defined
    if(_smoothing==-1)
      _smoothing = 6; //higher is more smoothing, but more latency
    if(_sensitivity==-1)
      _sensitivity = 3; //higher is less sensitive
    if(_deadzone==-1)
      _deadzone = 8;
  }
  _readings = (int*)malloc(_smoothing*sizeof(int));
  
  
  if(_axis!=""){
    XInput.setRange(xboxlookup(_xboxref), 0, AnalogRead_Max-1);//if joystick set range based on max of read analog. 
    setReadings(CENTER);
    performControllerAction(_xboxref+_axis,CENTER);
  }else{
    XInput.setTriggerRange(0, 255);//should be default, but lets set it
    setReadings(255);
    performControllerAction(_xboxref,255);
  }

}

void AnalogControllerObject::deinitialize(){
  _logger->debug("Deinitialize "+String(_name)+" on "+String(_pin)+","+String(_pin2)+", smoothing "+String(_smoothing)+", interrupt "+String(attached));
  if(attached){
    if(_pin>=0)
      detachInterruptCtx(digitalPinToInterrupt(_pin));
    if(_pin2>=0)
      detachInterruptCtx(digitalPinToInterrupt(_pin2));
  }
}

void AnalogControllerObject::interrupt(){
  //_logger->log(F("AnalogControllerObject: interrupt"));
  // if(_pin>=0 && _pin2<0){
  //   _pinState = analogRead(_pin);
  if(_pin>=0 && _pin2>=0)
    rotaryEncoder->tick();
}

bool AnalogControllerObject::isDigital(){
  return false;
}

void AnalogControllerObject::setReadings(int value){
  //_logger->debug("AnalogControlObj: set reading "+String(value));
  for(int i=0;i<_smoothing;i++){
    _readings[i] = value;
  }
  _total = value *(_smoothing);
  _prev = value;
  _pinState = value;
  rotaryPosition=value;
  rotaryPrevPosition=value;
}