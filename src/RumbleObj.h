#include "ControllerObj.h"


#ifndef RumbleObj_h
#define RumbleObj_h

#define LED_PIN 13

#define RUMBLE_ARRAY_SIZE 30
#define AnalogWrite_Max 255  // 10-bit ADC


class I2CNetwork;
class Logger;


class RumbleObject : public ControllerObject{
  public:
    
    RumbleObject(String name,String type, int pin, String size,int scale,Logger* logger);
    RumbleObject(String name,String type, String size, String remoteAddress, String index,I2CNetwork* i2c,Logger* logger);
    RumbleObject(String name,String type,int pin,int scale,Logger* logger);

    String performAction(int groupState=0) override;
    String performControllerAction(String action,int state, int groupState=0) override;
    int getState() override;

    void initialize() override;
    void count(int count) override;
  private:
    int getArrayState();
    bool isArrayEmpty();
    
    bool addToArray(uint8_t value,unsigned long time);
    void clearArray();
    int calcScale(int state);

    String _size = ""; 
    uint8_t _rumbleArray[RUMBLE_ARRAY_SIZE];
    unsigned long _rumbleTimeArray[RUMBLE_ARRAY_SIZE];
    int _rumbleArrayCurrent = 0;
    int _rumbleArraySize = 0;

    int _prevState = 0;
    int _scale=50;

};
#endif