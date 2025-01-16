#include "pins_arduino.h"
#include "Logger.h"
#include "ControllerObj.h"
#include "I2CNetwork.h"
#include "AnalogControllerObj.h"
#include <math.h>


AnalogControllerObject::AnalogControllerObject(String name,String type,int pin,int pin2,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int rotarySpeed,int mouseMode,int debugDeadzone,String adaptiveType, int adaptiveCalcMaxValue, int adaptiveCalcMValue, int adaptiveCalcNValue, int adaptiveCalcCValue,Logger* logger){//local
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

  if(adaptiveType!="")
    _adaptiveType=adaptiveType;
  if(adaptiveCalcMaxValue!=-999999)
    _adaptiveCalcMaxValue=adaptiveCalcMaxValue;
  if(adaptiveCalcMValue!=-999999)
    _adaptiveCalcMValue=adaptiveCalcMValue;
  if(adaptiveCalcNValue!=-999999)
    _adaptiveCalcNValue=adaptiveCalcNValue;
  if(adaptiveCalcCValue!=-999999)
    _adaptiveCalcCValue=adaptiveCalcCValue;
}
  
AnalogControllerObject::AnalogControllerObject(String name,String type,int pin,int pin2, String axis, int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,int rotarySpeed,int mouseMode,int debugDeadzone,String adaptiveType, int adaptiveCalcMaxValue, int adaptiveCalcMValue, int adaptiveCalcNValue, int adaptiveCalcCValue,I2CNetwork* i2c,Logger* logger){//remote sender
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

  if(adaptiveType!="")
    _adaptiveType=adaptiveType;
  if(adaptiveCalcMaxValue!=-999999)
    _adaptiveCalcMaxValue=adaptiveCalcMaxValue;
  if(adaptiveCalcMValue!=-999999)
    _adaptiveCalcMValue=adaptiveCalcMValue;
  if(adaptiveCalcNValue!=-999999)
    _adaptiveCalcNValue=adaptiveCalcNValue;
  if(adaptiveCalcCValue!=-999999)
    _adaptiveCalcCValue=adaptiveCalcCValue;
}
AnalogControllerObject::AnalogControllerObject(String name,String type, String xboxref, String axis,String adaptiveType, int adaptiveCalcMaxValue, int adaptiveCalcMValue, int adaptiveCalcNValue, int adaptiveCalcCValue,Logger* logger){//remote reciever
  _name=name;
  _type=type;
  _xboxref = xboxref;
  _axis = axis;
  _logger = logger;

  if(adaptiveType!="")
    _adaptiveType=adaptiveType;
  if(adaptiveCalcMaxValue!=-999999)
    _adaptiveCalcMaxValue=adaptiveCalcMaxValue;
  if(adaptiveCalcMValue!=-999999)
    _adaptiveCalcMValue=adaptiveCalcMValue;
  if(adaptiveCalcNValue!=-999999)
    _adaptiveCalcNValue=adaptiveCalcNValue;
  if(adaptiveCalcCValue!=-999999)
    _adaptiveCalcCValue=adaptiveCalcCValue;
}


String AnalogControllerObject::performAction(int groupState){
  // _logger->debug("AnalogControllerObj: performAction ");
  int state = getState();
  // _logger->debug("AnalogControllerObj: performAction "+String(state));
  //_logger->debug("AnalogControllerObj: performAction "+String(_name)+"-" +String(state)+" "+String(_pin));
  performImplReturn result = performCalculations(state);
  //_logger->debug("AnalogControllerObj: result "+String(result.inact)+"-" +String(result.value));
  if(result.inact){
    // if(_invert) //TODO: fix later
    //   result.value = AnalogRead_Max - result.value;
    
    if(_emulateDigital>=0){ // analog emulating digital button
      return peformEmulateButton(result.value);
      
    }else{//analog not emulating digital button
      //_logger->debug("AnalogControllerObj: performControllerAction " +String(result.value)+" "+String(_pin));
      return performControllerAction(_xboxref+_axis,result.value);
    }
  }
  return "";
}

String AnalogControllerObject::performControllerAction(String action,int state, int groupState){
  //emulate digital joystick/button, press and release
  if(action=="EmulatePress"){
    // int value = int((AnalogRead_Max/2.0)+float((AnalogRead_Max/2.0)*(state/100.0)));
    int value = getCenter()+state;
    
    //_logger->debug("AnalogControllerObj: press controller "+_name+" value: " +String(value));
    // setReadings(value);
    // setState(value);
    adaptiveStop(false);
    adaptiveStart(state);
    value=adaptiveUpdate(state);
    //_logger->debug("AnalogController: state:"+String(state)+" max:"+String(AnalogRead_Max)+" value:"+String(value));
    return performControllerAction(_xboxref+_axis,value)+"-Press";
  }
  if(action=="EmulateRelease"){
   
    int value = getCenter();

    // _logger->debug("AnalogControllerObj: release controller "+_name+" value: " +String(value));
    // setReadings(value);
    // setState(value);
    adaptiveStop(false);
    adaptiveStart(value);
    value=adaptiveUpdate(value);
    return performControllerAction(_xboxref+_axis,value)+"-Release";
  }

  //try to send action to remote
  String result = ControllerObject::performControllerAction(action,state);

  //if not remote, then we need to handle it here
  if(result==""){
    // _logger->debug("AnalogControllerObj: perform controller action "+_name+" "+action+", value: " +String(state));
    if(_xboxref!="mouse"){
      xbox(action,state);
      return _name;
    }else if(_xboxref=="mouse"){
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

  state = getCenter()-state;
 
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
  // _logger->debug("AnalogControllerObj: performcalc "+_name+" - "+String(_smoothing));
  int value = smooth(state);
  value = adaptiveUpdate(value);
  value = calcDeadzone(value)+_offset;
  
  // _logger->debug("AnalogControllerObj: performcalc value "+_name+" - "+String(value)+ " sensitivity "+String(isPastSensitivityThreshold(value)));
  unsigned long currentTime = millis();
  if(!isPastSensitivityThreshold(value))
    return {0,false};
  // String axisInfo = "";
  // if(_axis!="")
  //   axisInfo = " on "+_axis;
  //_logger->debug("AnalogControllerObj: controller "+_name+" value"+axisInfo+": " +String(value));
  //_logger->debug("AnalogControllerObj: performcalc "+_name+" - "+_xboxref+" value"+axisInfo+": " +String(value)+" - "+String(state)+","+String(_average)+","+String(_total)+","+String(_readIndex));

  _prev=value;
  _lastUpdate = currentTime;
  return {value,true};
}

bool AnalogControllerObject::isPastSensitivityThreshold(int value){
  if(_sensitivity<0){//if sensitivity is disabled then just return true
    return true;
  }
  // _logger->debug("AnalogControllerObj: controller "+_name+" sensitivityThreshhold: " +String(value)+" prev "+String(_prev));
  int diff = value - _prev;
  // _logger->debug("AnalogControllerObj: controller "+_name+" sensitivityThreshhold: diff " +String(diff)+" sensitivity "+String(_sensitivity));
  if(diff > _sensitivity || diff < -_sensitivity){
    return true;
  }
  return false;
}

int AnalogControllerObject::smooth(int state){
  if(_smoothing<=0){//if disabled just return value
    return state;
  }
  _total -= _readings[_readIndex];
  _readings[_readIndex] = state;
  _total += _readings[_readIndex];
  _readIndex++;
  if(_readIndex >= _smoothing)
     _readIndex = 0;
  
  _average = _total / _smoothing;
  return _average;
}


bool AnalogControllerObject::adaptiveStart(int state){
  // _logger->debug("AnalogControllerObj: adaptiveStart 1");
  if(_adaptiveType=="None")
    return false;
  // _logger->debug("AnalogControllerObj: adaptiveStart 2");
  if(_adaptiveRunning)
    return false;
  // _logger->debug("AnalogControllerObj: adaptiveStart 3 "+String(state));
  if(_adaptiveCurrent==state)
    return false;

  // _logger->debug("AnalogControllerObj: adaptiveStart :" +String(_adaptiveCurrent)+" to "+String(state));
 
  _adpativeTime= millis();
  _adaptiveStart=_adaptiveCurrent;
  _adaptiveTarget=state;
  _adaptiveRunning=true;
  return true;
}

void AnalogControllerObject::adaptiveStop(bool updateTarget){
  if(_adaptiveType=="None")
    return;
  if(!_adaptiveRunning)
    return;
  // _logger->debug("AnalogControllerObj: adaptiveStop cur: "+String(_adaptiveCurrent)+", target: "+String(_adaptiveTarget));
  if(updateTarget)
    _adaptiveCurrent = _adaptiveTarget;

  _adaptiveTarget=0;
  _adaptiveStart = 0;
  _adaptiveRunning=false;
}

int AnalogControllerObject::adaptiveUpdate(int state){
  // _logger->debug("AnalogControllerObj: adaptiveUpdate "+String(state));
  
  if(_adaptiveType=="None"){//if disabled just return value
    // _logger->debug("AnalogControllerObj: adaptiveUpdate early return "+String(state));
    // _adaptiveRunning=false;
    adaptiveStop(false);
    return state;
  }

  if(!_adaptiveRunning){ // not currently doing an adaptive target
    // _adaptiveCurrent = state;
    return _adaptiveCurrent;
  }

  int direction=1;
  if(_adaptiveTarget<_adaptiveStart){
    direction=-1;
  }

  unsigned long currentTime = millis();
  // if(state!=_adaptiveCurrent)
    // _logger->debug("AnalogControllerObj: adaptiveUpdate 1: "+_name+", state: " +String(state)+" current:"+String(_adaptiveCurrent)+" target: "+String(_adaptiveTarget)+" direction "+String(direction));


  if(_adaptiveCurrent==_adaptiveTarget){
    adaptiveStop(false);
    return _adaptiveTarget;
  }

  // _logger->debug("AnalogControllerObj: adaptiveUpdate 2:" +String(state)+" current:"+String(_adaptiveCurrent));
  int step = abs(_adaptiveTarget-_adaptiveStart);
  // _logger->debug("AnalogControllerObj: adaptiveUpdate 2.1:" +String(state)+" current:"+String(_adaptiveCurrent)+" step:"+String(step)+" percent:"+String(step/float(getMax())));
  if(step/float(getMax())<.01){//if state is within percent then just update target
    // _adaptiveCurrent=_adaptiveTarget;
    // _adaptiveRunning=false;
    adaptiveStop(true);
    // _logger->debug("AnalogControllerObj: adaptiveUpdate max return early "+_name+" " +String(state)+" current:"+String(_adaptiveCurrent));
    return _adaptiveTarget;
  }

  unsigned long timediff = currentTime - _adpativeTime;

  // _logger->debug("AnalogControllerObj: adaptiveUpdate 3:" +String(state)+" current:"+String(_adaptiveCurrent)+", timediff "+String(timediff)+", direction "+String(direction));
  
  // float maxValueUpdate = 27305;
  int updateValue=0;
  if(_adaptiveType=="line")
    updateValue=lineCalc(currentTime,timediff,direction);
  if(_adaptiveType=="squared")
    updateValue=squareCalc(currentTime,timediff,direction);

  if(_adaptiveCalcMaxValue!=0 && updateValue >=_adaptiveCalcMaxValue){
    adaptiveStop(true);
    return _adaptiveCurrent;
  }
    
  _adaptiveCurrent=_adaptiveStart+updateValue;

  if((direction==1 && _adaptiveCurrent>=_adaptiveTarget) || (direction==-1 && _adaptiveCurrent<=_adaptiveTarget)) {
    adaptiveStop(true);
    // return state;
  }
  // _adpativeTime=currentTime;
  
  // _logger->debug("AnalogControllerObj: adaptiveUpdate "+_name+" 4: " +String(state)+" current:"+String(_adaptiveCurrent)+", updatevalue "+String(updateValue));

  return _adaptiveCurrent;
}


int AnalogControllerObject::lineCalc(unsigned long currentTime, unsigned long timediff,int direction ){
  int n=_adaptiveCalcNValue;//150;
  int m=_adaptiveCalcMValue;//2;
  int c=_adaptiveCalcCValue;//0;

  int updatedValue = (float(n)/float(m))*timediff+c;

  updatedValue=updatedValue*direction;
  
  return updatedValue;
}

int AnalogControllerObject::squareCalc(unsigned long currentTime, unsigned long timediff,int direction ){
  int n=_adaptiveCalcNValue;//1;
  int m=_adaptiveCalcMValue;//2;
  int c=_adaptiveCalcCValue;//getMax()/6.0;

  int updatedValue = (n/float(m))*pow(timediff, 2)+c;
  // _logger->debug("AnalogControllerObj: controller "+_name+" squareCalc: "+String(updatedValue)+" time "+ String(timediff)+" m "+ String(m)+" n "+ String(n));
  
  updatedValue=updatedValue*direction;
  
  return updatedValue;
}

String AnalogControllerObject::peformEmulateButton(int state){
  

  int adjustedValue = -1;
  int adjustedRemoteIndex = -1;
  int stateAdjusted =-1;
  if(isTrigger){
    //_logger->debug("AnalogControllerObj: controller emlulated button trigger formula");
    stateAdjusted = int((float(state)/float(getMax()))*100);
    //_logger->debug("AnalogControllerObj: controller emlated trigger "+String(stateAdjusted)+ " compared to "+String(_emulateDigital)+" - "+String(_xboxEmulatedButtonState));
    adjustedRemoteIndex = _index;
  }
  if(isJoystick){
    
    if(state>=getCenter()){
      //_logger->debug("AnalogControllerObj: controller emlulated button trigger upper formula");
      stateAdjusted = int(((float(state)-getCenter())/(float(getMax())/2.0))*100);
      adjustedRemoteIndex = _indexMinus;
    }else{
      //_logger->debug("AnalogControllerObj: controller emlulated button trigger lower formula");
      stateAdjusted = 100-int((float(state)/(float(getMax())/2.0))*100);
      adjustedRemoteIndex = _index;
    }
  }
  if(isRotary){
    _logger->error("Error: Rotary not implmented as emulated button, AnalogControllerObj peformEmulateButton "+_name+" " +String(state));
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
  
  if(_deadzone==-1)
    return value;

  int _value = value;
  int deadzoneHigh = _deadzone;
  int deadzoneLow = 0;
  if(_axis!=""){//is joystick
    deadzoneHigh = getCenter()+_deadzone;
    deadzoneLow = getCenter()-_deadzone;
  }
  String axisInfo = "";
  if(_axis!="")
    axisInfo = " on "+_axis; 
  if(value<deadzoneHigh && value>deadzoneLow){
    if(_debugDeadzone)
      _logger->log("Debug Deadzone: "+_name+axisInfo+": axis centered in deadzone with value "+String(value)+" of center"+String(getCenter()));
    _value = getCenter();
  }else{
    if(_debugDeadzone)
      _logger->log("Debug Deadzone: "+_name+axisInfo+": axis not in the deadzone with value "+String(value)+" of center "+String(getCenter()));
  }
  return _value;
}

int AnalogControllerObject::getState(){
  if(isJoystick || isTrigger){
    if(!attached)
      _pinState = analogRead(_pin);
    // String axisInfo = "";
    // if(_axis!="")
    //   axisInfo = " on "+_axis; 
    // _logger->debug("AnalogControllerObj: controller "+_name+axisInfo+" reading "+String(_pinState));
    return _pinState;
  }else if(isRotary){
    
    if(!attached)
      rotaryEncoder->tick();
    int position = rotaryEncoder->getPosition();
    // setState(position);
    int rpm = rotaryEncoder->getRPM();
    // _logger->debug("AnalogControllerObj: controller "+_name+" rotary getstate position "+String(position)+", rpm "+String(rpm)+", prev "+String(prevPosition));
    if(position!=prevPosition){
      int dir = (int)rotaryEncoder->getDirection(); //0, 1, or -1
      // setState(position);
   
      float updatevalue = ((position - prevPosition)*((rpm/3)*(_rotarySpeed/100.0)));
      rotaryPosition += updatevalue;
      
      if(rotaryPosition<ROTARY_ANALOG_MIN)
        rotaryPosition=ROTARY_ANALOG_MIN;
      if(rotaryPosition>ROTARY_ANALOG_MAX)
        rotaryPosition=ROTARY_ANALOG_MAX;

      // _logger->debug("AnalogControllerObj: controller "+_name+" rotary read "+String(position)+" "+String(dir)+" "+String(rpm)+",update "+String(updatevalue)+", rotpos "+String(rotaryPosition));
      
      rotaryPrevPosition = rotaryPosition;
      if(_mouseMode){
        adaptiveStop(false);
        adaptiveStart(rotaryPosition);
        adaptiveUpdate(rotaryPosition);
      }
    
      prevPosition = position;
      
      return rotaryPosition;
    }
    else if(_mouseMode && rotaryPosition!=getCenter()){
      // _logger->debug("AnalogControllerObj: controller "+_name+" rotary read adaptive start "+String(position)+" "+String(rpm)+" - "+String(rotaryPosition));
      adaptiveStart(getCenter());
      rotaryPosition=adaptiveUpdate(getCenter());
      // rotaryPosition=getCenter();
      
    }
    return rotaryPosition;
  }
  return ControllerObject::getState();
  
}

void AnalogControllerObject::initialize(){
  ControllerObject::initialize();
  _logger->debug("Initialize "+String(_name)+" on "+String(_pin)+","+String(_pin2)+", smoothing "+String(_smoothing));
  
  if(_type=="joystick")
    isJoystick=true;
  if(_type=="trigger")
    isTrigger=true;
  if(_type=="rotary")
    isRotary = true;
  

  if(isJoystick){
    if(_pin>=0)
      pinMode(_pin,INPUT);
    //attachInterruptCtx(digitalPinToInterrupt(_pin)); not a digital input 

    //defaults for joysticks
    if(_smoothing==-1)
      _smoothing = 6; //higher is more smoothing, but more latency
    if(_sensitivity==-1)
      _sensitivity = 3; //higher is less sensitive
    if(_deadzone==-1)
      _deadzone = 8;

    if(_adaptiveType=="")
      _adaptiveType="line";

    if(_adaptiveType=="line"){
      if(_adaptiveCalcMaxValue==-999999)
        _adaptiveCalcMaxValue=0;
      if(_adaptiveCalcMValue==-999999)
        _adaptiveCalcMValue=1;
      if(_adaptiveCalcNValue==-999999)
        _adaptiveCalcNValue=300;
      if(_adaptiveCalcCValue==-999999)
        _adaptiveCalcCValue=0;
    }
   
    if(_adaptiveType=="squared"){
      if(_adaptiveCalcMaxValue==-999999)
        _adaptiveCalcMaxValue=27305;
      if(_adaptiveCalcMValue==-999999)
        _adaptiveCalcMValue=2;
      if(_adaptiveCalcNValue==-999999)
        _adaptiveCalcNValue=1;
      if(_adaptiveCalcCValue==-999999)
        _adaptiveCalcCValue=5461;
    }
    

  }else if(isTrigger){
    if(_pin>=0)
      pinMode(_pin,INPUT);

    //defaults for triggers
    if(_smoothing==-1)
      _smoothing = 6; //higher is more smoothing, but more latency
    if(_sensitivity==-1)
      _sensitivity = 3; //higher is less sensitive
    if(_deadzone==-1)
      _deadzone = 8;
    

  }else if(isRotary){
    // if(_adaptiveType=="")
    //   _adaptiveType = "None";
    _mouseMode = true;
    if(_pin>=0)
      pinMode(_pin,INPUT);
    if(_pin2>=0)
      pinMode(_pin2,INPUT);
    
    if(_pin>=0 && _pin2>=0)
      rotaryEncoder = new RotaryEncoder(_pin, _pin2, RotaryEncoder::LatchMode::TWO03);
    if(_pin>=0){
      attachInterruptCtx(digitalPinToInterrupt(_pin));
      attached = false; // we need to attach two pins
    }
    if(_pin2>=0)
      attachInterruptCtx(digitalPinToInterrupt(_pin2));

    //defaults for tackball
    if(_smoothing==-1)
      _smoothing = -1; //higher is more smoothing, but more latency //old default 8
    if(_sensitivity==-1)
      _sensitivity = 2; //higher is less sensitive //old default 3
    if(_deadzone==-1)
      _deadzone = -1;//old default 8
    if(_mouseModeTimeout<=0){//default configs
      _mouseModeTimeout = 20;
      _mouseMode=true;
    }else if(_mouseModeTimeout==-1){ //disabled in config
      _mouseMode=false;
    }

    if(_adaptiveType=="")
      _adaptiveType="line";

    if(_adaptiveType=="line"){
      if(_adaptiveCalcMaxValue==-999999)
        _adaptiveCalcMaxValue=200;
      if(_adaptiveCalcMValue==-999999)
        _adaptiveCalcMValue=1;
      if(_adaptiveCalcNValue==-999999)
        _adaptiveCalcNValue=300;
      if(_adaptiveCalcCValue==-999999)
        _adaptiveCalcCValue=0;
    }
    
    if(_adaptiveType=="squared"){
      if(_adaptiveCalcMaxValue==-999999)
        _adaptiveCalcMaxValue=200;
      if(_adaptiveCalcMValue==-999999)
        _adaptiveCalcMValue=1;
      if(_adaptiveCalcNValue==-999999)
        _adaptiveCalcNValue=1;
      if(_adaptiveCalcCValue==-999999)
        _adaptiveCalcCValue=100;
    }
  }
  if(_smoothing>=1)
    _readings = (int*)malloc(_smoothing*sizeof(int));
  
  XInput.setRange(xboxlookup(_xboxref), getMin(), getMax());
  setReadings(getCenter());
  setState(getCenter());
  performControllerAction(_xboxref+_axis,getCenter());
  _logger->debug("Initialized "+String(_name)+" on "+String(_pin)+","+String(_pin2)+", smoothing "+String(_smoothing)+", isJoystick "+String(isJoystick)+", isRotary "+String(isRotary)+", isTrigger "+String(isTrigger));
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

  // _logger->debug("AnalogControlObj: set reading "+String(value));
  _pinState = value;
  
  if(_smoothing>=1){
    for(int i=0;i<_smoothing;i++){
      _readings[i] = value;
    }
    _total = value *(_smoothing);
    _prev = value;
    rotaryPosition=value;
    rotaryPrevPosition=value;
  }
}

int AnalogControllerObject::getCenter(){
  int center = 0;
  if(isJoystick)
    center = JOYSTICK_ANALOG_CENTER;
  if(isRotary)
    center=ROTARY_ANALOG_CENTER;
  if(isTrigger)
    center=TRIGGER_ANALOG_MAX;
  return center;
}

int AnalogControllerObject::getMin(){
  int middle = 0;
  if(isJoystick)
    middle = JOYSTICK_ANALOG_MIN;
  if(isRotary)
    middle=ROTARY_ANALOG_MIN;
  if(isTrigger)
    middle=TRIGGER_ANALOG_MIN;
  return middle;
}

int AnalogControllerObject::getMax(){
  int max = 0;
  if(isJoystick)
    max = JOYSTICK_ANALOG_MAX;
  if(isRotary)
    max=ROTARY_ANALOG_MAX;
  if(isTrigger)
    max=TRIGGER_ANALOG_MAX;
  return max;
}

