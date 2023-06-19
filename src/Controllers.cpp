#include "Logger.h"
#include "DigitalControllerObj.h"
#include "AnalogControllerObj.h"
#include "RumbleObj.h"
#include "Controllers.h"
#include "I2CNetwork.h"


Controllers::Controllers(){

}

String Controllers::Setup(I2CNetwork* i2c,Logger* logger){
  _i2c = i2c;
  _logger = logger;  
  profilesCurrent = 0;
  return "";    
}

void Controllers::setProfilesNumber(int numberOfProfiles){
  
  //_numberOfShortcuts=numberofShortcuts;
  _profileShortcutMappings = new ControllerNameValue*[numberOfProfiles];

  _profileShortcutMappingsLengths = new int[numberOfProfiles];
  
}

void Controllers::setShortcutMappingsNumber(int profileIndex,int numberofShortCutMappings){
  _profileShortcutMappingsLengths[profileIndex] = numberofShortCutMappings;
  _profileShortcutMappings[profileIndex] = new ControllerNameValue[numberofShortCutMappings];
  
}

void Controllers::setShortcutMappingsItem(int profileIndex,int mappingIndex,String name,String type,int mapping,int value){
  ControllerNameValue cnv = { name,type,mapping,value};
  _profileShortcutMappings[profileIndex][mappingIndex]=cnv;
  if(type=="group_select"){
   
  }
  if(type=="group_shift"){
    
  }
}

void Controllers::addConfig(){
  profilesSize = profilesSize + 1;
  configLengths[profilesSize]=0;
  totalSignals[profilesSize]=0;
}

void Controllers::addControllerDigital(String name,String type, int pin,String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize, int doubleTapTime){
  _logger->debug("adding digital std controller "+String(name) +"-"+String(pin)+"-"+String(xboxButtons[0])+"-"+String(xboxButtonsSize));
  configs[profilesSize][configLengths[profilesSize]]= new DigitalControllerObject(name,type,pin,xboxButtons,xboxButtonsSize,xboxButtonsDoubleTap,xboxButtonsDoubleTapSize,doubleTapTime,_logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}
void Controllers::addControllerDigital(String name,String type,String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize){
  _logger->debug("adding digital reciever controller "+String(name) +"-"+String(xboxButtons[0])+"-"+String(xboxButtonsSize));
  configs[profilesSize][configLengths[profilesSize]]= new DigitalControllerObject(name,type,xboxButtons,xboxButtonsSize,xboxButtonsDoubleTap,xboxButtonsDoubleTapSize,_logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}
void Controllers::addControllerDigital(String name,String type,int pin,String remoteAddress,int remoteIndex,int emulateAnalog,int doubleTapTime,I2CNetwork* i2c){
  _logger->debug("adding digital remote pin controller "+String(name) +"-"+String(pin)+"-"+String(remoteAddress)+"-"+String(remoteIndex));
  configs[profilesSize][configLengths[profilesSize]]= new DigitalControllerObject(name,type,pin,remoteAddress,remoteIndex,emulateAnalog,doubleTapTime,i2c,_logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}

void Controllers::addControllerAnalog(String name,String type, int pin,int pin2,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int rotarySpeed, int mouseMode,int debugDeadzone){
  _logger->debug("adding analog std controller "+String(name) +"-"+String(pin)+"-"+xboxref+"-"+axis+"-"+String(smoothing)+"-"+String(sensitivity)+"-"+String(deadzone));
  configs[profilesSize][configLengths[profilesSize]]= new AnalogControllerObject(name,type,pin,pin2,xboxref,axis,smoothing,sensitivity, deadzone,offset,invert,rotarySpeed, mouseMode,debugDeadzone,_logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}
void Controllers::addControllerAnalog(String name,String type, String xboxref, String axis){
  _logger->debug("adding analog reciever controller "+String(name) +"-"+String(xboxref)+"-"+String(axis));
  configs[profilesSize][configLengths[profilesSize]]= new AnalogControllerObject( name,type,  xboxref,  axis, _logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}
void Controllers::addControllerAnalog(String name,String type,int pin, int pin2, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,int rotarySpeed, int mouseMode,int debugDeadzone,I2CNetwork* i2c){
  _logger->debug("adding analog remote pin controller "+String(name) +"-"+String(pin)+"-"+String(remoteAddress)+"-"+String(remoteIndex)+"-"+String(smoothing)+"-"+String(sensitivity)+"-"+String(deadzone));
  configs[profilesSize][configLengths[profilesSize]]= new AnalogControllerObject( name,type, pin, pin2, axis,  smoothing, sensitivity,  deadzone, offset,invert,emulateDigital,index,emulateDigitalMinus,indexMinus, remoteAddress, remoteIndex,rotarySpeed, mouseMode,debugDeadzone, i2c,_logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}

void Controllers::addRumble(String name,String type,int pin, String size,int scale,int signal){
  _logger->debug("adding rumble std "+String(name) +"-"+String(pin)+"-"+String(size));
  RumbleObject* ro = new RumbleObject(name,type, pin, size, scale, _logger);
  //ro->count(10);
  configs[profilesSize][configLengths[profilesSize]]= ro;
  
  if(signal==1){
    //_logger->debug("adding signal on profile "+String(profilesSize)+" "+String(totalSignals[profilesSize]));
    signals[profilesSize][totalSignals[profilesSize]]=configs[profilesSize][configLengths[profilesSize]];
    totalSignals[profilesSize]++;
  }
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}

void Controllers::addRumble(String name,String type, String size,String remoteAddress,int index){
  _logger->debug("adding rumble remote "+String(name) +"-"+String(size)+"-"+String(remoteAddress)+"-"+String(index));
  configs[profilesSize][configLengths[profilesSize]]= new RumbleObject(name,type, size, remoteAddress, index, _i2c,_logger);
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}

void Controllers::addRumble(String name,String type,int pin,int scale,int signal){
  _logger->debug("adding rumble remote reciever "+String(name)+"-"+String(pin));
  RumbleObject* ro = new RumbleObject(name,type,pin, scale,_logger);
  configs[profilesSize][configLengths[profilesSize]]= ro;
  if(signal==1){
    //_logger->debug("adding signal on profile "+String(profilesSize)+" "+String(totalSignals[profilesSize]));
    signals[profilesSize][totalSignals[profilesSize]]=configs[profilesSize][configLengths[profilesSize]];
    totalSignals[profilesSize]++;
  }
  configLengths[profilesSize]=configLengths[profilesSize]+1;
}

ControllerObject* Controllers::getControllerByIndex(int index) {
  if(index>configLengths[profilesCurrent]){
    _logger->log("Error: index "+String(index)+" out of range for config "+String(profilesCurrent));
    return NULL;
  }
  return configs[profilesCurrent][index];
}

ControllerObject* Controllers::startControllerList(int profile) {
  if(profile==-1)
    profile = profilesCurrent;
  controllerCurrent=-1;
  //logger.debug("StartControllerList "+String(profilesCurrent));
  return nextController(profile);
}
ControllerObject* Controllers::nextController(int profile) {
  if(profile==-1)
    profile = profilesCurrent;
  controllerCurrent=controllerCurrent+1;
  //_logger->debug("nextController "+String(controllerCurrent)+" of "+String(configLengths[profile]));
  if(controllerCurrent>=configLengths[profile])
    return NULL;
  return configs[profile][controllerCurrent];
}

void Controllers::initialize(){
  //_logger->log("Initializing controllers profile size "+String(profilesSize));
  clearXInputs();
  XInput.begin();
  XInput.setAutoSend(false);
  deinitializeProfile(profilesPrev);
  initializeProfile(profilesCurrent);
  if(totalSignals[profilesCurrent]>0){
    //send signal to all our setup singal mechanisms as to which profile we have inited
    for(int i=0;i<totalSignals[profilesCurrent];i++){
      //_logger->debug("signal on profile "+String(profilesCurrent) +" "+String(i));
      signals[profilesCurrent][i]->count(profilesCurrent+1);//profiles start at 0
    }
    _logger->log("Controllers Initialized. Lets get ready to rumble!");
  }else{
    _logger->log("Controllers Initialized");
  }
}

void Controllers::initializeProfile(int profile){
  profilesCurrent = profile;
  _logger->log("Initializing controllers with profile "+String(profilesCurrent));
  ControllerObject* controller = startControllerList();
  while(controller!=NULL){
    controller->initialize();
    controller = nextController();
  }
  firstInit=false;
}

void Controllers::deinitializeProfile(int profile){
  if(!firstInit){
    _logger->log("Deinitializing controllers with profile "+String(profile));
    ControllerObject* controller = startControllerList(profile);
    while(controller!=NULL){
      controller->deinitialize();
      controller = nextController(profile);
    }
  }
}

String Controllers::performActions(){
  ControllerObject* controller = startControllerList();
  while(controller!=NULL){
    // unsigned long start = millis();
    // _logger->log("Controllers: cotroller "+controller->name());
    int groupid = controller->getGroup(); 
    int groupState = 0;  //TODO: look up group state
    String action = controller->performAction(groupState);
    
    if(action.startsWith("Error")){
      _logger->log("Controllers: detectected "+controller->name()+" in an invalid state. " +action);
    }else if(action !="" && controller->isDigital()){
      processKeyShortcuts(action);
    }
    // unsigned long currentTiming = millis()-start;
    // _logger->log("Controllers: cotroller "+controller->name() +" in "+String(currentTiming));
    controller = nextController();
  }
  return "";
}

String Controllers::performActionByIndex(int index,String action,int state){
  ControllerObject* controllerobj = getControllerByIndex(index);
  if(controllerobj!=NULL){
    controllerobj->setState(state);//we need to set the state of the pin from remote system
    String actionid = controllerobj->performControllerAction(action,state);
    if(action.startsWith("Error")){
      _logger->log("Controllers: "+controllerobj->name()+" "+action);
    }else{
      processKeyShortcuts(actionid);
    }
    return "";
  }
  return "Error: could not find controller "+String(index);
}


void Controllers::processKeyShortcuts(String key){
  if(key.startsWith("Error"))
    return;
  //_logger->log("Controllers: checking shortcut key "+String(key));
  for(int shortcutIndex=0;shortcutIndex<_numberOfShortcuts;shortcutIndex++){//go through all the shortcuts
    for(int mappingIndex=0;mappingIndex<_profileShortcutMappingsLengths[profilesCurrent];mappingIndex++){//then go through all the mappings
      if(_profileShortcutMappings[profilesCurrent][mappingIndex].valueA==shortcutIndex){ //if a shortcut is in the mapping
        if(key==_shortcuts[shortcutIndex][_shortcutCurrentPosition[shortcutIndex]]){ // check the next item in the shortcut
          _shortcutCurrentPosition[shortcutIndex]++;
          if(_profileShortcutMappings[profilesCurrent][mappingIndex].type.endsWith("shift") && _shortcutCurrentPosition[shortcutIndex]==_profileShortcutMappings[profilesCurrent][mappingIndex].valueB){
            
            performMacro(_profileShortcutMappings[profilesCurrent][mappingIndex]);
          }else if(_shortcutCurrentPosition[shortcutIndex]>=_shortcutLengths[shortcutIndex]){//shortcut completed
            _shortcutCurrentPosition[shortcutIndex]=0;
            performMacro(_profileShortcutMappings[profilesCurrent][mappingIndex]);
              //return;
          }
          //TODO: deal with holding
        }else{
          _shortcutCurrentPosition[shortcutIndex]=0;
        }
      }  
    }    
    
    
  }
}

void Controllers::clearShortcutCounts(){
  for(int i=0;i<_numberOfShortcuts;i++){
    _shortcutCurrentPosition[i]=0;
  }
}

bool Controllers::performMacro(ControllerNameValue shortcutMapping){
  if(shortcutMapping.name=="nextProfile"){
    //_logger->debug("Controller: shortcuts sending next config");
    clearShortcutCounts();
    nextConfig();
    _i2c->sendAction("NextConfig",profilesCurrent);
    return true;
  }
  if(shortcutMapping.name=="previousProfile"){
    //_logger->debug("Controller: shortcuts sending prev config");
    clearShortcutCounts();
    previousConfig();
    _i2c->sendAction("PrevConfig",profilesCurrent);
    return true;
  }
  if(shortcutMapping.name=="reset"){
    //_logger->debug("Controller: shortcuts sending reset config");
    clearShortcutCounts();
    _i2c->sendAction("Reset");      
    initialize();
    return true;
  }
  
  if(shortcutMapping.type=="group_select"){
    
    return true;
  }
  if(shortcutMapping.type=="group_shift"){

    return true;
  }
  return false;
}

void Controllers::nextConfig(){
  profilesPrev = profilesCurrent;
  profilesCurrent++;
  if(profilesCurrent>profilesSize)
    profilesCurrent=0;
  controllerCurrent=-1;
  initialize();
}

int Controllers::getConfig(){
  return profilesCurrent;
}

void Controllers::previousConfig(){
  profilesPrev = profilesCurrent;
  profilesCurrent--;
  if(profilesCurrent<0)
    profilesCurrent=profilesSize;
  controllerCurrent=-1;
  initialize();
}

void Controllers::setConfig(int profileId){
  profilesCurrent = profileId;
  if(profilesCurrent>profilesSize)
    profilesCurrent=0;
  if(profilesCurrent<0)
    profilesCurrent=profilesSize;
  controllerCurrent=-1;
  initialize();
}

String Controllers::getProfileLengths(){
  String output="";
  for(int i=0;i<=profilesSize;i++){
    if(i!=0)
      output+=",";
    output+=String(configLengths[i]);
  }
  return output;
}

int Controllers::getNumberOfProfiles(){
  return profilesSize;
}

void Controllers::setShortcutsNumber(int numberofShortcuts){
  //Serial.println("Setting shortcuts number "+String(numberofShortcuts));
  _numberOfShortcuts=numberofShortcuts;
  _shortcuts = new String*[_numberOfShortcuts];
  _shortcutLengths = new int[_numberOfShortcuts];
  _shortcutCurrentPosition = new int[_numberOfShortcuts];
}

void Controllers::setShortcutsItemsNumber(int index, int numberofItems){
  //Serial.println("Setting shortcuts items number index "+String(index)+": "+String(numberofItems));
  _shortcuts[index] = new String[numberofItems];
  _shortcutLengths[index] = numberofItems;
  _shortcutCurrentPosition[index] = 0;
}

void Controllers::setShortcutsItem(int shortcutIndex,int itemIndex, String item){
  //Serial.println("Setting shortcuts item "+String(shortcutIndex)+", "+String(itemIndex)+": "+String(item));
  _shortcuts[shortcutIndex][itemIndex] = String(item);
  //Serial.println("Done Setting");
}

void Controllers::printShortcuts(){
  for(int shortcutIndex=0;shortcutIndex<_numberOfShortcuts;shortcutIndex++){
    for(int i=0;i<_shortcutLengths[shortcutIndex];i++){
      Serial.println(_shortcuts[shortcutIndex][i]);
    }
  }
}

void Controllers::clearXInputs(){
  XInput.releaseAll();
  XInput.reset();
}