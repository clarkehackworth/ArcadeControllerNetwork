#include <XInput.h>
#include "ControllerObj.h"
#include "DigitalControllerObj.h"

# define MAX_CONFIGS 6
# define MAX_CONTROLS 20 //per config
# define MAX_SIGNALS 5

#ifndef Controllers_h
#define Controllers_h

class Logger;
class ControllerObject;
class RumbleObject;
class I2CNetwork;

class Controllers {
  
  
  struct ControllerNameValue
  {
    String name;
    String type;
    int valueA;
    int valueB;
  };

  public:
    Controllers();
    String Setup(I2CNetwork* i2c,Logger* logger);
    void initialize();
    void initializeProfile(int profile);
    void addConfig();
    void addControllerDigital(String name, int pin,String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize);
    void addControllerDigital(String name,String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize);
    void addControllerDigital(String name,int pin,String remoteAddress,int remoteIndex,int emulateAnalog,I2CNetwork* i2c);
    void addControllerAnalog(String name, int pin,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert);
    void addControllerAnalog(String name, String xboxref, String axis);
    void addControllerAnalog(String name,int pin, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,I2CNetwork* i2c);
    void addRumble(String name,int pin, String size,int scale,int signal);
    void addRumble(String name, String size,String remoteAddress,int index);
    void addRumble(String name,int pin,int scale,int signal);

    String performActions();
    String performActionByIndex(int index,String action,int state);

    void setShortcutsNumber(int numberofShortcuts);
    void setShortcutsItemsNumber(int index, int numberofItems);
    void setShortcutsItem(int shortcutIndex,int itemIndex, String item);
    void printShortcuts();
    //void setPreviousShortcut(int* nextSC,int size);

    void nextConfig();
    void previousConfig();
    void setConfig(int profileId);
    
    int getNumberOfProfiles();
    String getProfileLengths();

    void setProfilesNumber(int numberOfProfiles);
    void setShortcutMappingsNumber(int profileIndex,int numberofShortCutMappings);
    void setShortcutMappingsItem(int profileIndex,int mappingIndex,String name,String type,int mapping,int value);
    //void setShortcutMappingsItem(int profileIndex,int mappingIndex,String name,String type,int mapping);

  private:
    ControllerObject* getControllerByIndex(int index);
    ControllerObject* startControllerList();
    ControllerObject* nextController();
    void processKeyShortcuts(String key);
    void clearShortcutCounts();
    bool performMacro(ControllerNameValue key);
    void clearXInputs();
    

    int profilesSize=-1;
    int profilesCurrent=0;
    ControllerObject* configs[MAX_CONFIGS][MAX_CONTROLS];
    int configLengths[MAX_CONTROLS];
    int controllerCurrent=0;
    Logger* _logger;
    I2CNetwork* _i2c;
    
    String** _shortcuts;
    int _numberOfShortcuts=0;
    int* _shortcutLengths;
    int* _shortcutCurrentPosition;

    ControllerNameValue** _profileShortcutMappings;
    int* _profileShortcutMappingsLengths;
    ControllerObject* signals[MAX_CONFIGS][MAX_SIGNALS];
    int totalSignals[MAX_CONFIGS];
    
};
#endif