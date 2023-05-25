#include <Arduino.h>
//#include <cstdint>
#include <XInput.h>
#include "CommonController.h"


#ifndef ControllerObj_h
#define ControllerObj_h

class I2CNetwork;
class Logger;

#define ACTIVATED_PINSTATE HIGH

class ControllerObject {
  public:


    String name();

    virtual String performAction(int groupState=0);
    virtual String performControllerAction(String action,int state, int groupState=0);
    virtual int getState();
    virtual bool isDigital();
    void setState(int state);
    virtual void initialize();

    //uint8_t xboxlookup( String name );
    int pin();
    
    int remoteAddress();
    int remoteIndex();
    
    void setGroup(int group);
    int getGroup();
    
    virtual void count(int count);

    String _name = "";
    I2CNetwork* _i2c;
    Logger* _logger;
    //digital params
    int _pinState = ACTIVATED_PINSTATE;
    int _pin = -1;
    int _group=0;

    //remote params
    int _remoteAddress = -1;
    int _remoteIndex = 0;
  private:
    
    
};
#endif