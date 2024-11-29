#include <SD.h>
#include <ArduinoJson.h>


# define MAX_CONFIGS 6
# define MAX_CONTROLS 20 //per config
# define MAX_SLAVES 20
# define JSON_DOC_BUFFER_SIZE 6144

#ifndef Config_h
#define Config_h


// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy audio board: pin 10
// Teensy 3.5 & 3.6 & 4.1 on-board: BUILTIN_SDCARD
// Wiz820+SD board: pin 4
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
const int chipSelect = BUILTIN_SDCARD;

class Logger;
class Controllers;
class I2CNetwork;

class Config {
  
  public:
    Config();
    String setup(Logger* logger, Controllers* controllers, I2CNetwork* i2c);
    String getNetworkName();
    int getButtonRepeatDelay();
    int getNumberOfSlaves();
    int* getSlaves();

  private:
    

    File configFile;
    Sd2Card card;
    
    
    float version=0;
    float buttonRepeatDelay=300;
    String networkName="";
    int slaves[MAX_SLAVES];
    int numberOfSlaves=0;
    bool i2cSetup = false;

};
#endif