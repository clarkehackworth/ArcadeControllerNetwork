#include "Logger.h"
#include "RumbleObj.h"

RumbleObject::RumbleObject(String name,String type, int pin, String size,int scale,Logger* logger){
  _name = name;
  _type=type;
  _pin = pin;
  _size = size;
  if(scale>=0)
    _scale = scale;
  _logger = logger;
}
RumbleObject::RumbleObject(String name,String type, String size, String remoteAddress, String index,I2CNetwork* i2c,Logger* logger){
  _name = name;
  _type=type;
  _size = size;
  _remoteAddress=(int)remoteAddress.toInt();
  _remoteIndex=(int)index.toInt();
  _i2c = i2c; 
  _logger = logger;
}
RumbleObject::RumbleObject(String name,String type,int pin,int scale,Logger* logger){
  _name = name;
  _type=type;
  _pin = pin;
  if(scale>=0)
    _scale = scale;
  _logger = logger;
}


void RumbleObject::initialize(){
  ControllerObject::initialize();
  //_logger->log("Let's get ready to rumble!");
  _logger->debug("Initialize "+String(_name)+" on "+String(_pin));
  if(_pin>=0){
    pinMode(_pin,OUTPUT);
     if(_size=="left"){
       pinMode(LED_PIN,OUTPUT);
     }
  }
}

String RumbleObject::performAction(int groupState){
  // _logger->debug("Rumble: perform action "+String(_name)+" "+String(_rumbleArrayCurrent)+" "+String(_rumbleArraySize));
  int state = -1;
  if(isArrayEmpty()){
    state = getState();
  }else{
    state = getArrayState();
  }
  
  if(state>=0 && state!=_prevState){  
    return performControllerAction("Rumble",state);
  }
  return "";
}

String RumbleObject::performControllerAction(String action,int state, int groupState){
  String result = ControllerObject::performControllerAction(action,state);
  if(result==""){
    if(action=="Rumble" && _pin>=0){
      //_logger->debug("Rumble: perform controller action "+String(action)+" "+String(state)+" "+String(_pin)+" adjusted "+String(calcScale(state)));

      analogWrite(_pin, calcScale(state));
      
      if(_size=="left"){
        analogWrite(LED_PIN,state);
      }
      _prevState = state;
    }
    return "";
  }
  return "Error-Unknown:"+_name+"-"+action+"-"+String(state)+"-"+result;
}

int RumbleObject::calcScale(int state){
  if(state==0)
    return 0;

  //Should likely use map instead of my own janky math
  //_logger->debug("Rumble AdjustedValue state scale "+String(state)+" "+String(_scale));
  float percent = float(AnalogWrite_Max)*(float(_scale)/100.0);
  float percentleft = float(AnalogWrite_Max)*(float(100-_scale)/100.0);
  //_logger->debug("Rumble AdjustedValue percents "+String(percent)+" "+String(percentleft));
  float adjustedValue = (percent)+((percentleft)*(float(state)/float(AnalogWrite_Max)));
  //_logger->debug("Rumble AdjustedValue "+String(int(adjustedValue)));
  
  if(adjustedValue>255)
    adjustedValue=255;
  if(adjustedValue<0)
    adjustedValue=0;
  return int(adjustedValue);
}

bool RumbleObject::isArrayEmpty(){
  if(_rumbleArrayCurrent==_rumbleArraySize)
    return true;
  return false;
}

int RumbleObject::getArrayState(){
  int result = -1;
  if(!isArrayEmpty()){
    unsigned long curTime = millis();
    //_logger->debug("Rumble: getArrayState "+String(_rumbleArrayCurrent)+" "+String(_rumbleTimeArray[_rumbleArrayCurrent])+" compared to "+String(curTime));
    if(_rumbleTimeArray[_rumbleArrayCurrent]<=curTime){
      result = int(_rumbleArray[_rumbleArrayCurrent]);
      _rumbleArrayCurrent++;
      if(_rumbleArrayCurrent==_rumbleArraySize){
        _rumbleArraySize=0;
        _rumbleArrayCurrent=0;
      }
    }
  }
  return result;
}

void RumbleObject::count(int count){
  clearArray();
  unsigned long curTime = millis();
  for(int i=0;i<count*2;i++){
    if(!addToArray(255,1000+curTime+(i*500)))
      _logger->debug("Error: adding to rumble queue");
    i++;
    if(!addToArray(0,1000+curTime+(i*500)))
      _logger->debug("Error: adding to rumble queue");
  }
}

bool RumbleObject::addToArray(uint8_t value,unsigned long time){
  if(_rumbleArraySize<RUMBLE_ARRAY_SIZE){
    
    _rumbleArray[_rumbleArraySize]=value;
    _rumbleTimeArray[_rumbleArraySize]=time;
    //_logger->debug("Rumble: adding to array "+String(_rumbleArraySize)+" "+String(_rumbleArray[_rumbleArraySize]));
    _rumbleArraySize++;
    return true;
  }
  return false;
}

void RumbleObject::clearArray(){
  _rumbleArraySize=0;
  _rumbleArrayCurrent=0;
}

int RumbleObject::getState(){
  uint16_t result = 0;
  if(_size=="left"){
    result = (uint16_t)XInput.getRumbleLeft();  // 8 bit, big motor
    //_logger->debug("Rumble: rumble left: "+String(int(result)));
  }
  if(_size=="right"){
    result = (uint16_t)XInput.getRumbleRight();  // 8 bit, small motor
    //_logger->debug("Rumble: rumble right: "+String(int(result)));
  }
  if(_size=="both"){
    result = XInput.getRumble();  // 16 bit, left motor as 0xFF00, right motor as 0x00FF
  }
  // if(_pin>=0){
  //   int reading = analogRead(_pin);
  //   return reading;
  // }
  return int(result); //yes we loose some numbers on the high end for systems with 16 bit ints, like teensy4.1, and running both rumble to one motor. maybe fix later. 
}