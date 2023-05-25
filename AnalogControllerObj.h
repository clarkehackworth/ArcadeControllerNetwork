#include "ControllerObj.h"


#ifndef AnalogControllerObj_h
#define AnalogControllerObj_h

#define CENTER 511

#define AnalogRead_Max 1023  // 10-bit ADC


class I2CNetwork;
class Logger;

class AnalogControllerObject : public ControllerObject{
  public:
    
    AnalogControllerObject(String name,int pin,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,Logger* logger);//local
    AnalogControllerObject(String name,int pin, String axis, int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,I2CNetwork* i2c,Logger* logger);//remote sender
    AnalogControllerObject(String name, String xboxref, String axis,Logger* logger);//remote reciever

    String performAction(int groupState=0) override;
    String performControllerAction(String action,int state, int groupState=0) override;
    int getState() override;
    bool isDigital() override;

    void initialize() override;
  private:
    void setReadings(int value);
    int calcDeadzone(int value);
    String peformEmulateButton(int state);

  int _smoothing = 5; //higher is more smoothing, but more latency
  int _sensitivity = 3; //higher is less sensitive

  int _deadzone = 6;

  String _xboxref="";
  String _axis="";
  int _readIndex = 0;
  int* _readings;
  int _total = CENTER;
  int _average = CENTER;

  int _prev = CENTER;

  int _offset = 0;
  bool _invert = false;
  
  int _emulateDigital=-1;
  int _index=-1;
  int _emulateDigitalMinus=-1;
  int _indexMinus=-1;
  bool _xboxEmulatedButtonState = false;
};
#endif