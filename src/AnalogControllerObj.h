#include <RotaryEncoder.h>
#include "ControllerObj.h"


#ifndef AnalogControllerObj_h
#define AnalogControllerObj_h

#define CENTER 511

#define AnalogRead_Max 1023  // 10-bit ADC


class I2CNetwork;
class Logger;

struct performImplReturn {
      int value;
      bool inact;
    };


class AnalogControllerObject : public ControllerObject{
  public:
    
    AnalogControllerObject(String name,String type,int pin,int pin2,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int rotarySpeed,int mouseMode,int debugDeadzone,Logger* logger);//local
    AnalogControllerObject(String name,String type,int pin,int pin2,String axis, int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,int rotarySpeed,int mouseMode,int debugDeadzone,I2CNetwork* i2c,Logger* logger);//remote sender
    AnalogControllerObject(String name,String type, String xboxref, String axis,Logger* logger);//remote reciever

    String performAction(int groupState=0) override;
    String performControllerAction(String action,int state, int groupState=0) override;
    int getState() override;
    bool isDigital() override;

    void initialize() override;
    void deinitialize() override;
    void interrupt() override;
  private:
    
    void setReadings(int value);
    int calcDeadzone(int value);
    String peformEmulateButton(int state);
    performImplReturn performCalculations(int state);
    void processInterupt();
    void xbox(String action,int state);
    void mouse(String action,int state);

  int _pin2 = -1;

  int _smoothing = -1; //higher is more smoothing, but more latency
  int _sensitivity = -1; //higher is less sensitive 

  int _deadzone = -1;

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

  bool _debugDeadzone = false;

  int _rotarySpeed = 70;
  RotaryEncoder* rotaryEncoder;
  volatile int rotaryPosition=CENTER;
  int rotaryPrevPosition=CENTER;
  int prevPosition=CENTER;

  bool _mouseMode = false;//spring back to zero with time
  unsigned long _lastUpdate=0;
  int _mouseModeTimeout = 0;//in milliseconds, the higher the value to longer it will take to spring back to 0

  //bool attachedInterrupt = false;

};
#endif