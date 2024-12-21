


#ifndef I2CNetwork_h
#define I2CNetwork_h

#define QUEUE_SIZE 50
#define DATA_PACKET 5 //for queue array, 0: i2c address, 1: controller index, 2: button action, 3: state (two bytes)
#define ERROR_BUS_FAULTS_BEFORE_RESET 200
#define TIME_TO_FORCE_SLAVE_INFORM 200

class Controllers;
class Logger;

class I2CNetwork{
  public:
    I2CNetwork();
    String addToQueue(int address,int index,String action,int value);
    String name();
    bool isEnabled();
    bool isMaster();
    String setup(String networkName, int numberOfSlaves,int* configslaves,Controllers* controllers,Logger* logger);
    void performAction();
    void receiveEvent(int howMany);
    void requestEvent();
    void sendAction(String action,int value=-1);

  private:
    uint8_t actionlookup( String name );
    String buttonlookup(int i);
    String processAction(int actionid, int value);
    const char* doublebyte_to_binary(int x);
    const char* byte_to_binary(int x);
    
    bool enable;
    String _name;
    bool master;
    int _numberOfSlaves;
    int* slaves;
    bool* informedSlaves;
    unsigned long* informedSlavesTimeSince;
    bool once;
    Logger* _logger;
    Controllers* _controllers;
    int transmissionError;
    unsigned long transmissionErrorTime;

    int queue[QUEUE_SIZE][DATA_PACKET]; 
    bool queueSlots[QUEUE_SIZE];

    bool initialized=false;

    String _prevQueuedItem="";

};
#endif