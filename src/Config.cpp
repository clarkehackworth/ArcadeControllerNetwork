#include "Logger.h"
#include "Config.h"
#include "Controllers.h"
#include "I2CNetwork.h"
#include "CommonController.h"

const char *DEFAULT_CONFIG = 
#include "config.json"
;

Config::Config(){

}

// returns empty string on success
String Config::setup(Logger* logger, Controllers* controllers, I2CNetwork* i2c) {
  _logger=logger;

  StaticJsonDocument<JSON_DOC_BUFFER_SIZE> doc; // 1024 should be fine for now, but for larger configs this may not work
  DeserializationError error;

  if (SD.begin(chipSelect)) {
    
    configFile = SD.open(getConfigFilename().c_str() , FILE_READ);
    if (configFile && configFile.available()) {
    
      //String data = configFile.readString();

      error = deserializeJson(doc, configFile);
    }
    configFile.close();
  }else{
    Serial.println("Error initializing sdcard, using default config");
    error = deserializeJson(doc, DEFAULT_CONFIG);
    //return "SD Card failed, or not found. Using default";
  }

  // Test if parsing succeeds.
  if (error) {
    String errorMessage = "Error: deserializeJson failed, "+String(error.f_str());
    if(String(error.c_str())=="InvalidInput")
      errorMessage+=", check to make sure json is valid.";
    return errorMessage;
  }
  
  version = doc["configVersion"].as<String>().toFloat();
  networkName = doc["deviceId"].as<String>();
  if(networkName=="null")
    return "Error: device must have a deviceId configured.";
  //Serial.println("finding i2c");
  
  if(doc.containsKey("doubleTapTime")){
    buttonRepeatDelay = doc["doubleTapTime"].as<String>().toInt();
  } 

  numberOfSlaves=0;
  if(doc.containsKey("network")){
    //Serial.println("getting slaves");
    for (JsonVariant jsonSlave : doc["network"].as<JsonArray>()) {
      int slave = jsonSlave.as<String>().toInt();
      
      slaves[numberOfSlaves] = slave;
      //Serial.println("Slave: "+String(numberOfSlaves)+" "+String(slave)+" "+String(slaves[numberOfSlaves]));
      numberOfSlaves=numberOfSlaves+1;
    }
  }
  
  if(doc.containsKey("shortcuts")){
    JsonArray shortcutsarr = doc["shortcuts"].as<JsonArray>();
    int numberOfShortcuts = shortcutsarr.size();
    controllers->setShortcutsNumber(numberOfShortcuts);
    
    int shortcutIndex=0;
    for (JsonArray jsonshortcutlist : doc["shortcuts"].as<JsonArray>()) {
      int i=0;
      controllers->setShortcutsItemsNumber(shortcutIndex,jsonshortcutlist.size());
      for (JsonVariant shortcutItem : jsonshortcutlist){
        controllers->setShortcutsItem(shortcutIndex,i,shortcutItem.as<String>());
        i++;
      }
      
      shortcutIndex++;
    }
    
    
  }
  
  JsonArray jsonprofiles = doc["controllerProfiles"].as<JsonArray>();
  int profileIndex = 0;
  controllers->setProfilesNumber(jsonprofiles.size());
  for (JsonObject jsonprofile : jsonprofiles) {
    controllers->addConfig();
    String profileName = jsonprofile["name"].as<String>();
    
    // JsonArray nextShortcut = jsonprofile["shortcuts"]["nextProfile"];
    // int nextShortcutArr[nextShortcut.size()];
    // int i = 0;
    // for (JsonVariant shortcutItem : nextShortcut) { 
    //   nextShortcutArr[i] = xboxlookup(shortcutItem.as<String>());
    //   i++;
    // }
    // controllers->setNextShortcut(nextShortcutArr, i);
    
    
    // JsonArray prevShortcut = jsonprofile["shortcuts"]["previousProfile"];
    // int prevShortcutArr[prevShortcut.size()];
    // i = 0;
    // for (JsonVariant shortcutItem : prevShortcut) { 
    //   prevShortcutArr[i] = xboxlookup(shortcutItem.as<String>());
    //   i++;
    // }
    // controllers->setPreviousShortcut(prevShortcutArr, i);

    if(jsonprofile.containsKey("shortcutMapping")){
      JsonArray mappings = jsonprofile["shortcutMapping"].as<JsonArray>();
      int numberOfMappings = mappings.size();
      controllers->setShortcutMappingsNumber(profileIndex,numberOfMappings);
      int i=0;
      for (JsonVariant mappingItem : mappings){
        String maptype = String("");
        if(mappingItem.containsKey("name") && mappingItem.containsKey("shortcut"))
          
          if(mappingItem.containsKey("type")){
            maptype = mappingItem["type"].as<String>();
          }
          int value = mappingItem["value"].as<int>();
          String name = mappingItem["name"].as<String>();
          int shortcutid = mappingItem["shortcut"].as<int>();
          controllers->setShortcutMappingsItem(profileIndex,i,name,maptype,shortcutid,value);
        i++;
      }
    }

    

    
    for (JsonObject jsoncontrol : jsonprofile["config"].as<JsonArray>()) {
      
      String name = jsoncontrol["name"].as<String>();
      String type = jsoncontrol["type"].as<String>();
      
      int pin = jsoncontrol["pin"].as<int>();
      String remoteAddress = jsoncontrol["remoteAddress"].as<String>();
      int index = jsoncontrol["index"].as<int>();
      int emulateAnalog = jsoncontrol["emulateAnalog"].as<int>();
      
      if(type == "button" || type == "buttonKeyboard"){
        JsonVariant jsonxboxButton = jsoncontrol["buttonRef"];
        JsonVariant jsonxboxButtonDoubleTap = jsoncontrol["buttonRefDoubleTap"];
        bool isString = true;
        int xboxButtonsSize = -1;
        int xboxButtonsDoubleTapSize = -1;
        String* buttons;
        String* buttonsDoubleTap;
        int doubleTapTime = buttonRepeatDelay;
        
        if(jsoncontrol.containsKey("doubleTapTime"))
          doubleTapTime = jsoncontrol["doubleTapTime"].as<int>();
        if(!jsonxboxButton.isNull() && jsonxboxButton.nesting()==0){ //String
          buttons = new String[1];
          buttons[0]=jsoncontrol["buttonRef"].as<String>();
          xboxButtonsSize=1;
        }else if(!jsonxboxButton.isNull() && jsonxboxButton.nesting()==1){ //array
          buttons = new String[jsonxboxButton.size()];
          int i=0;
          for (JsonVariant jsonbutton : jsoncontrol["buttonRef"].as<JsonArray>()) {
            buttons[i]=jsonbutton.as<String>();
            i++;
          }
          xboxButtonsSize=jsonxboxButton.size();
        }
        if(!jsonxboxButtonDoubleTap.isNull() && jsonxboxButtonDoubleTap.nesting()==0){ //String
          buttonsDoubleTap = new String[1];
          buttonsDoubleTap[0]=jsoncontrol["buttonRefDoubleTap"].as<String>();
          xboxButtonsDoubleTapSize=1;
        }else if(!jsonxboxButtonDoubleTap.isNull() && jsonxboxButtonDoubleTap.nesting()==1){ //array
          buttonsDoubleTap = new String[jsonxboxButtonDoubleTap.size()];
          int i=0;
          for (JsonVariant jsonbutton : jsoncontrol["buttonRefDoubleTap"].as<JsonArray>()) {
            buttonsDoubleTap[i]=jsonbutton.as<String>();
            i++;
          }
          xboxButtonsDoubleTapSize=jsonxboxButtonDoubleTap.size();
        }
        //String xboxButton = jsoncontrol["xboxButton"].as<String>();
        
        if(xboxButtonsSize>=0 && remoteAddress!="null"){
          logger->log("Error: "+name+" has buttonRef and remoteAddress. Cannot have both, either choose button or remote.");
          continue;
        }
        
        if(xboxButtonsSize>=0 && pin>0){
          //logger->debug("add controller std "+String(name) +"-"+String(pin)+"-"+String(xboxButtonsSize));
          controllers->addControllerDigital(name,type,pin,buttons,xboxButtonsSize,buttonsDoubleTap,xboxButtonsDoubleTapSize,doubleTapTime); 
          
        }else if(xboxButtonsSize>=0 && pin==0){
          //logger->debug("add controller xboxbutton "+String(name) +"-"+String(xboxButtonsSize));
          controllers->addControllerDigital(name,type,buttons,xboxButtonsSize,buttonsDoubleTap,xboxButtonsDoubleTapSize); 
          
        }else if((remoteAddress!="null" || emulateAnalog!=0) && pin>0){
          String digital_remoteAddress = String(remoteAddress);
          if(digital_remoteAddress=="null")
            digital_remoteAddress=String(networkName);
          
          //logger->debug("add controller remote pin "+String(name) +"-"+String(pin)+"-"+String(remoteAddress)+"-"+String(index)+"-"+String(emulateAnalog));
          controllers->addControllerDigital(name,type,pin,digital_remoteAddress,index,emulateAnalog,doubleTapTime,i2c);
            
          
          
        }
      }
      if(type == "joystick" || type == "trigger" || type == "rotary"){
        String xboxref = "";
        String axis = "";
        int pin2 = -1;
        
        if(jsoncontrol.containsKey("pin2"))
          pin2 = jsoncontrol["pin2"].as<int>();
        if(type == "joystick" || type == "rotary"){
          if(jsoncontrol.containsKey("joystickRef"))
            xboxref = jsoncontrol["joystickRef"].as<String>();
          if(jsoncontrol.containsKey("axis"))
            axis = jsoncontrol["axis"].as<String>();
        }
        if(type == "trigger")
          if(jsoncontrol.containsKey("triggerRef"))
            xboxref = jsoncontrol["triggerRef"].as<String>();
        
        int sensitivity = jsoncontrol["sensitivity"].as<int>();
        if(!jsoncontrol.containsKey("sensitivity"))
          sensitivity = -1;
        int deadzone = jsoncontrol["deadzone"].as<int>();
        if(!jsoncontrol.containsKey("deadzone"))
          deadzone = -1;
        // int smoothing = jsoncontrol["smoothing"].as<int>();
        int offset = jsoncontrol["offset"].as<int>();
        int invertnum = jsoncontrol["invert"].as<int>();
        bool invert = false;
        if(invertnum==1)
          invert = true;

        int emulateDigital = -1;
        if(jsoncontrol.containsKey("emulateDigital"))
          emulateDigital = jsoncontrol["emulateDigital"].as<int>();
        if(jsoncontrol.containsKey("emulateDigital+"))
          emulateDigital = jsoncontrol["emulateDigital+"].as<int>();
        
        int index = -1;
        if(jsoncontrol.containsKey("index"))
          index = jsoncontrol["index"].as<int>();
        if(jsoncontrol.containsKey("index+"))
          index = jsoncontrol["index+"].as<int>();

        int emulateDigitalMinus = -1;
        if(jsoncontrol.containsKey("emulateDigital-"))
          emulateDigitalMinus = jsoncontrol["emulateDigital-"].as<int>();
        
        int indexMinus = -1;
        if(jsoncontrol.containsKey("index-"))
          indexMinus = jsoncontrol["index-"].as<int>();

        String xboxButton = jsoncontrol["buttonRef"].as<String>();

        int rotarySpeed = -1;
        if(jsoncontrol.containsKey("trackspeed"))
          rotarySpeed = jsoncontrol["trackspeed"].as<int>();
        
        int mouseMode = -1;
        if(jsoncontrol.containsKey("trackSpringBack"))
          mouseMode = jsoncontrol["trackSpringBack"].as<int>();

        int debugDeadzone=0;
        if(jsoncontrol.containsKey("debugDeadzone"))
          debugDeadzone = jsoncontrol["debugDeadzone"].as<int>();

        int smoothing=-1;
        if(jsoncontrol.containsKey("smoothing"))
          smoothing = jsoncontrol["smoothing"].as<int>();

        String adaptiveType=""; 
        if(jsoncontrol.containsKey("adaptiveType"))
            adaptiveType = jsoncontrol["adaptiveType"].as<String>();

        int adaptiveCalcMaxValue=-999999;
        if(jsoncontrol.containsKey("adaptiveCalcMaxValue"))
          adaptiveCalcMaxValue = jsoncontrol["adaptiveCalcMaxValue"].as<int>();

        int adaptiveCalcMValue=-999999;
        if(jsoncontrol.containsKey("adaptiveCalcMValue"))
          adaptiveCalcMValue = jsoncontrol["adaptiveCalcMValue"].as<int>();
        
        int adaptiveCalcNValue=-999999;
        if(jsoncontrol.containsKey("adaptiveCalcNValue"))
          adaptiveCalcNValue = jsoncontrol["adaptiveCalcNValue"].as<int>();

        int adaptiveCalcCValue=-999999;
        if(jsoncontrol.containsKey("adaptiveCalcCValue"))
          adaptiveCalcCValue = jsoncontrol["adaptiveCalcCValue"].as<int>();

        if(xboxref!="" && remoteAddress!="null"){ //TODO: check if emulated too
          logger->log("Error: "+name+" has " +xboxref+" and remoteAddress. Cannot have both, either choose xbox or remote.");
          continue;
        }

        // logger->debug("try init analog controller std "+String(name) +"-"+String(pin)+"-"+String(pin2)+"-"+String(xboxref)+"-"+String(axis));
        if(xboxref!="" && pin>0){
          logger->debug("init analog controller std "+String(name) +"-"+String(pin)+"-"+String(pin2)+"-"+String(xboxref)+"-"+String(axis));
          controllers->addControllerAnalog(name,type, pin,pin2,xboxref, axis,smoothing,sensitivity, deadzone,offset,invert, rotarySpeed, mouseMode,debugDeadzone, adaptiveType, adaptiveCalcMaxValue, adaptiveCalcMValue, adaptiveCalcNValue, adaptiveCalcCValue);
          
        }else if(xboxref!="" && pin==0){
          logger->debug("init analog controller xboxref "+String(name) +"-"+String(xboxref)+"-"+String(axis));
          controllers->addControllerAnalog( name,type,  xboxref,  axis, adaptiveType, adaptiveCalcMaxValue, adaptiveCalcMValue, adaptiveCalcNValue, adaptiveCalcCValue);
          
        }else if((remoteAddress!="null"||emulateDigital>=0) && pin>0){
          String analog_remoteAddress = String(remoteAddress);
          if(analog_remoteAddress=="null")
            analog_remoteAddress=String(networkName);
          logger->debug("init analog controller remote pin "+String(name) +"-"+String(pin)+"-"+String(pin2)+"-"+String(remoteAddress)+"-"+String(index));
          controllers->addControllerAnalog( name,type, pin,pin2,axis, smoothing, sensitivity,  deadzone,offset,invert,emulateDigital,index,emulateDigitalMinus,indexMinus, analog_remoteAddress, index,rotarySpeed,mouseMode,debugDeadzone, adaptiveType, adaptiveCalcMaxValue, adaptiveCalcMValue, adaptiveCalcNValue, adaptiveCalcCValue, i2c);
          
        }
      }

      if(type == "rumble"){
        String xboxRumbleSize = "";
        int signal = jsoncontrol["signal"].as<int>();
        int scale = -1;
        if(jsoncontrol.containsKey("scale"))
          scale = jsoncontrol["scale"].as<int>();
        if(jsoncontrol.containsKey("rumbleSize"))
          xboxRumbleSize = jsoncontrol["rumbleSize"].as<String>();
        if(remoteAddress=="null" && xboxRumbleSize!="")
          controllers->addRumble(name, type,pin, xboxRumbleSize,scale,signal);
        else if(remoteAddress!="null"){
          controllers->addRumble(name, type,xboxRumbleSize, remoteAddress, index);
        }else if(pin>=0){
          controllers->addRumble(name,type,pin,scale,signal);
        }
      }

      if(type == "infaredCamera"){
        controllers->addCamera();
      }
      
    }
    profileIndex++;
  }
  logger->log("Config: set "+String(controllers->getNumberOfProfiles()+1)+" profile(s), with controllers of "+controllers->getProfileLengths());

  
  
  return "";
}

String Config::getConfigFilename(){
  String filename="";
  File root  = SD.open("/");
  while (true)
  {
    File entry =  root.openNextFile();
    if (! entry)
      break;
    // _logger->log("Config File checking 1:"+String(entry.name()));
    if(String(entry.name()).startsWith("ACN-config") && String(entry.name()).endsWith(".json")){
      filename = String(entry.name());
      entry.close();
      break;
    }
    entry.close();
  }

  if(filename==""){
    File root  = SD.open("/");
    while (true)
    {
      File entry =  root.openNextFile();
      if (! entry)
        break;
      // _logger->log("Config File checking 2:"+String(entry.name()));
      if(String(entry.name())=="config.json"){
        filename = String(entry.name());
        entry.close();
        break;
      }
      entry.close();
    }  
  }
  if(filename=="")
    _logger->log("No config file found");
  else
    _logger->log("Config File "+filename+" found");
  return filename;
}
String Config::getNetworkName(){
  return networkName;
}

int Config::getNumberOfSlaves(){
  return numberOfSlaves;
}

int* Config::getSlaves(){
  return &slaves[0];
}

int Config::getButtonRepeatDelay(){
  return buttonRepeatDelay;
}

