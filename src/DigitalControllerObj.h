
#include "ControllerObj.h"


#ifndef DigitalControllerObj_h
#define DigitalControllerObj_h

#define Emulate_Double_Time 2000//in miliseconds. When emulating an analog cotrol, time to hold input to go full speed

#ifndef KEYBOARD_h
// class KeyboardClass {
//   public:
//     void press(char c);
//     void release(char c);
// };
// KeyboardClass Keyboard;
#endif

class I2CNetwork;
class Logger;

struct buttonsReturn {
  String* buttons;
  int size;
};

class DigitalControllerObject : public ControllerObject{
  public:
    //DigitalControllerObject();
    DigitalControllerObject(String name,String type,int pin, String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize,int doubleTapTime,Logger* logger);//local
    DigitalControllerObject(String name,String type,int pin, String remoteAddress,String remoteIndex,int emulateAnalog,int doubleTapTime,I2CNetwork* i2c,Logger* logger);//remote sender
    DigitalControllerObject(String name,String type,String* xboxButtons,int xboxButtonsSize,String* xboxButtonsDoubleTap,int xboxButtonsDoubleTapSize,Logger* logger);//remote reciever

    String performAction(int groupState=0) override;
    String performControllerAction(String action,int state, int groupState=0) override;
    int getState() override;
    bool isDigital() override;

    void initialize() override;
    void deinitialize() override;
    void interrupt() override;
  private:
    
  
    uint8_t inputType();
    uint8_t test();
    buttonsReturn getButton();
    String getValue(String data, char separator, int index);
    void xboxButton(String button,String action);
    void keyboardButton(String button,String action);
    void mouseButton(String buttonStr,String action);
    void mouseScroll(String buttonStr,String action);

    
    String* _xboxButtons;
    String* _xboxButtonsDoubleTap;
    int _xboxButtonsSize;
    int _xboxButtonsSizeDoubleTap;
    int _doubleTapDelay=0;//default comes from config obj
    bool doubleTapEnabled = false;
    bool doubleTapStarted = false;
    bool doubleTapActivated = false;
    bool doubleTapTimedOut = false;
    bool doubleTapRevert = false;
    int doubleTapRevertState1 = -1;
    int doubleTapRevertState2 = -1;
    unsigned long _doubleTapTime=0;
    uint8_t _inputType = INPUT_PULLUP;
    uint8_t _test = LOW;
    bool _xboxState = false;
    int _emulateAnalog=0;

    unsigned long _emulateTime=0;
    bool _emulateLongPressed=false;
    bool _useInterrupt = true;

};



#endif