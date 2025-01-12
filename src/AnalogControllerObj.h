#include <RotaryEncoder.h>
#include "ControllerObj.h"


#ifndef AnalogControllerObj_h
#define AnalogControllerObj_h

// #define CENTER 511

// #define AnalogRead_Max 1023

#define JOYSTICK_ANALOG_MIN -32768
#define JOYSTICK_ANALOG_CENTER 0
#define JOYSTICK_ANALOG_MAX 32767

#define TRIGGER_ANALOG_MIN 0
#define TRIGGER_ANALOG_MAX 255

#define ROTARY_ANALOG_MIN -1024
#define ROTARY_ANALOG_CENTER 0
#define ROTARY_ANALOG_MAX 1023// 10-bit ADC


class I2CNetwork;
class Logger;

struct performImplReturn {
      int value;
      bool inact;
    };


class AnalogControllerObject : public ControllerObject{
  public:
    
    AnalogControllerObject(String name,String type,int pin,int pin2,String xboxref, String axis,int smoothing,int sensitivity, int deadzone, int offset,bool invert,int rotarySpeed,int mouseMode,int debugDeadzone,String adaptiveType, int adaptiveCalcMaxValue, int adaptiveCalcMValue, int adaptiveCalcNValue, int adaptiveCalcCValue,Logger* logger);//local
    AnalogControllerObject(String name,String type,int pin,int pin2,String axis, int smoothing,int sensitivity, int deadzone, int offset,bool invert,int emulateDigital, int index, int emulateDigitalMinus,int indexMinus,String remoteAddress,String remoteIndex,int rotarySpeed,int mouseMode,int debugDeadzone,String adaptiveType, int adaptiveCalcMaxValue, int adaptiveCalcMValue, int adaptiveCalcNValue, int adaptiveCalcCValue,I2CNetwork* i2c,Logger* logger);//remote sender
    AnalogControllerObject(String name,String type, String xboxref, String axis,String adaptiveType, int adaptiveCalcMaxValue, int adaptiveCalcMValue, int adaptiveCalcNValue, int adaptiveCalcCValue,Logger* logger);//remote reciever

    String performAction(int groupState=0) override;
    String performControllerAction(String action,int state, int groupState=0) override;
    int getState() override;
    bool isDigital() override;

    void initialize() override;
    void deinitialize() override;
    void interrupt() override;

    struct Range { int32_t min; int32_t max; };
    int getCenter();
    int getMin();
    int getMax();
  private:
    bool isPastSensitivityThreshold(int value);
    int smooth(int state);
    void setReadings(int value);
    int calcDeadzone(int value);
    String peformEmulateButton(int state);
    performImplReturn performCalculations(int state);
    void processInterupt();
    void xbox(String action,int state);
    void mouse(String action,int state);
    int adaptiveUpdate(int state);
    bool adaptiveStart(int state);
    void adaptiveStop(bool updateTarget);
    int lineCalc(unsigned long currentTime, unsigned long timediff,int direction);
    int squareCalc(unsigned long currentTime, unsigned long timediff,int direction );

  int _pin2 = -1;

  bool isRotary = false;
  bool isTrigger = false;
  bool isJoystick = false;
 
  int _sensitivity = -1; //higher is less sensitive 

  

  int _deadzone = -1;

  String _xboxref="";
  String _axis="";

  //smoothing vars
  int _smoothing = -1; //higher is more smoothing, but more latency
  int _readIndex = 0;
  int* _readings;
  int _total = 0;
  int _average = 0;
  int _prev = -1;

  //adaptive movement 
  String _adaptiveType="";
  unsigned long  _adpativeTime = 0;
  bool _adaptiveRunning = false; 
  int _adaptiveCurrent = 0; //TODO: need to set appropriate in constructors for full rand vs half range
  int _adaptiveTarget = 0;
  int _adaptiveStart = 0;
  int _adaptiveCalcMaxValue=-999999;
  int _adaptiveCalcMValue=-999999;
  int _adaptiveCalcNValue=-999999;
  int _adaptiveCalcCValue=-999999;


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
  volatile int rotaryPosition=0;
  int rotaryPrevPosition=0;
  int prevPosition=0;

  bool _mouseMode = false;//spring back to zero with time
  unsigned long _lastUpdate=0;
  int _mouseModeTimeout = 0;//in milliseconds, the higher the value to longer it will take to spring back to 0

  //bool attachedInterrupt = false;

};
#endif