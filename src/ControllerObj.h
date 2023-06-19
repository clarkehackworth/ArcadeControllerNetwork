#include <Arduino.h>
//#include <cstdint>
#include <XInput.h>
#include "CommonController.h"


#ifndef ControllerObj_h
#define ControllerObj_h

class I2CNetwork;
class Logger;

#define ACTIVATED_PINSTATE HIGH

constexpr size_t max_num_interrupts = 42;
// Use IRAM_ATTR for functions called from ISRs to prevent ESP8266 resets.
#if defined(ESP8266) || defined(ESP32)
#define IC_ISR_ATTR IRAM_ATTR
#else
#define IC_ISR_ATTR
#endif

class ControllerObject {
  public:


    String name();

    virtual String performAction(int groupState=0);
    virtual String performControllerAction(String action,int state, int groupState=0);
    virtual int getState();
    virtual bool isDigital();
    void setState(int state);
    virtual void initialize();
    virtual void deinitialize();

    //uint8_t xboxlookup( String name );
    int pin();
    
    int remoteAddress();
    int remoteIndex();
    
    void setGroup(int group);
    int getGroup();
    
    virtual void count(int count);


    String _name = "";
    String _type = "";
    I2CNetwork* _i2c;
    Logger* _logger;
    //digital params
    int _pinState = ACTIVATED_PINSTATE;
    int _pin = -1;
    int _group=0;

    //remote params
    int _remoteAddress = -1;
    int _remoteIndex = 0;

    //hw interrupt items
    virtual IC_ISR_ATTR void interrupt();
    static ControllerObject *instance_table[max_num_interrupts];
    /// The type of a handler function.
    using isr_func_t = void (*)();
    /// Get a pointer to the interrupt handler function for the given interrupt.
    template <unsigned NumISR = max_num_interrupts>
    static isr_func_t get_isr(unsigned interrupt);
    /// Register the interrupt handler for this instance.
    void attachInterruptCtx(int interrupt);
    /// Un-register the interrupt handler for this instance.
    void detachInterruptCtx(int interrupt);
    bool attached = false;
  private:
    
    
};
#endif