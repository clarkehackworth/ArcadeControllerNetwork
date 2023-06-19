#include <Wire.h>
#include "Logger.h"
#include "Controllers.h"
#include "I2CNetwork.h"

typedef struct
{
  const char *name;
  uint8_t  value;  
}buttonLookup;
static buttonLookup ButtonAction[] =
{
  {  "Acknowledge", 255  },
  {  "Reset", 103 },
  {  "GetConfig", 102  },
  {  "ChangeConfig", 101  },
  {  "Acknowledge", 100  },
  {  "Rumble" , 12 },
  {  "EmulateJoystick" , 11 },
  {  "EmulateRelease" , 10 },
  {  "EmulatePress" , 9 },
  {  "JOY_LEFTX" , 8 },
  {  "JOY_LEFTY" , 7 },
  {  "JOY_RIGHTX" , 6 },
  {  "JOY_RIGHTY" , 5 },
  {  "TRIGGER_LEFT" , 4 },
  {  "TRIGGER_RIGHT" , 3 },
  {  "Release", 1  },
  {  "Press", 2  },
  {   NULL , 0     }
};

I2CNetwork::I2CNetwork(){
  //Serial.println("I2CNetwork");
}

String I2CNetwork::name() { return _name; }



// returns empty string on success
String I2CNetwork::setup(String networkName, int numberOfSlaves,int* configslaves,Controllers* controllers,Logger* logger) {
  enable=false;
  _logger=logger;
  _controllers = controllers;

  _name = networkName;
  _numberOfSlaves = numberOfSlaves;

  master=false;
  once=false;

  
  
  for(int i=0;i<QUEUE_SIZE;i++){
    queueSlots[i]=false;
  }
  transmissionError=0;

  if(_name.toInt()==0){
    // Initialize I2C communications as Master
    // Wire.begin();
    master=true;
    enable=true;
    slaves = (int*)malloc(_numberOfSlaves*sizeof(int));
    for(int i=0;i<_numberOfSlaves;i++){
      slaves[i] = configslaves[i];
      // Serial.println("i2c Slave: "+String(slaves[i]));
    }
  }else{
    enable=true;
    master=false;
    addToQueue(0, 0, "GetConfig", _name.toInt());

    //Wire.begin((int)_name.toInt());
    // Function to run when data requested from master
    //Wire.onRequest(requestEvent); 
    // Wire.onRequest([] (int howMany) {receiveEvent(howMany);});

    // Function to run when data received from master
    // Wire.onReceive(receiveEvent);
  }
  
  initialized=true;
  return "";  
}

uint8_t I2CNetwork::actionlookup( String name )
{
  buttonLookup *lptr;
  for ( lptr = ButtonAction; lptr->name != NULL; lptr++ )
    if ( strcmp( lptr->name, name.c_str() ) == 0 )
        break;
  return( lptr->value );
}

String I2CNetwork::buttonlookup( int i )
{
  buttonLookup *lptr;
  for ( lptr = ButtonAction; lptr->name != NULL; lptr++ )
    if ( int(lptr->value) == i )
      break;
  return( String(lptr->name) );
}

String I2CNetwork::addToQueue(int address,int index,String action,int value){
  
  if(action!=""){
    //_logger->debug("IC2Network: adding to queue "+String(address)+"-"+String(index)+"-"+action+"-"+String(value));
    int i=0;
    for(;i<QUEUE_SIZE;i++){
      if(!queueSlots[i]){
        queue[i][0]=address;
        queue[i][1]=index;
        queue[i][2]=actionlookup(action);
        queue[i][3]=value; // we store a 16 byte int and later convert it to two bytes (high and low) for sending over the wire.
        queueSlots[i]=true;
        break;
      }
    }
    
    if(i==QUEUE_SIZE){
      return "Error: queue full";
    }
  }
  return "";
  
} 



void I2CNetwork::performAction(){
  for(int i=0;i<QUEUE_SIZE;i++){
    if(queueSlots[i]){
      
      int address = queue[i][0];
      int index = queue[i][1];
      int actionid = queue[i][2];
      int state = queue[i][3];
      //_logger->log("I2CNetwork: found in queue:"+String(address)+", "+String(index)+", "+String(actionid)+", "+String(state));
      if(address==name().toInt() || address==-1){//-1 is how this device references itself, like loopback
        
        if(address==-1)
          _logger->log("I2CNetwork: picked up item for negative address: "+String(address)+", "+String(index)+", "+String(actionid)+", "+String(state));
        _logger->log("I2CNetwork: picked up item for myself: "+String(address)+", "+String(index)+", "+String(actionid)+", "+String(state));
        String action = processAction(actionid,state);
        if(action == ""){
          String result = _controllers->performActionByIndex(index,buttonlookup(actionid),state);
          if(result!="")//error
            _logger->log(result+" using I2cNetwork");
          else
            queueSlots[i]=false;
        }else{
          _logger->log("I2CNetwork: Error item addressed for "+String(address)+" not processed "+String(action)+"-"+String(state));
        }
        
      }else{
        if(master){
          //_logger->debug("performAction: "+name()+" sending "+String(address)+"-"+String(index)+"-"+String(actionid)+"-"+String(state));
          Wire.beginTransmission(queue[i][0]);
          Wire.write(address);
          Wire.write(index);
          Wire.write(actionid);
          // byte highbyte=(signed char)(state & 0xFF00)>>8; //shift right 8 bits, leaving only the 8 high bits. 
          // byte lowbyte=(signed char)state & 0xFF; //bitwise AND with 0xFF
          char highbyte=(queue[i][3] & 0xFF00)>>8; //shift right 8 bits, leaving only the 8 high bits. 
          char lowbyte=queue[i][3] & 0xFF; //bitwise AND with 0xFF
          Wire.write(highbyte);
          Wire.write(lowbyte);
          byte busStatus = Wire.endTransmission();
          // if(actionid!=255)
          //   _logger->debug("I2C raw message 1: "+String(address)+" "+String(index)+" "+String(actionid)+" "+String(state) +" -> "+String(doublebyte_to_binary(state)) +" ["+String(byte_to_binary(highbyte))+","+String(byte_to_binary(lowbyte))+"]");
          if(busStatus != 0x00){
            //Bus fault or slave fault
            //TWCR = 0;
            unsigned long currentTime = millis();
            transmissionError+=1;
            if(transmissionError>ERROR_BUS_FAULTS_BEFORE_RESET && transmissionErrorTime+5000<currentTime){
              _logger->log("I2CNetwork: Transmission Error 1 to slave "+String(queue[i][0])+", resetting bus");
              busStatus = Wire.endTransmission();
              Wire.begin();
              transmissionError=0;
              transmissionErrorTime = currentTime;
            }
            
          }else{
            //_logger->debug("I2C sent message");
            queueSlots[i]=false;
          }
        }
      }
      
    }

  }
  if(master){
    for(int i=0;i<_numberOfSlaves;i++){
      
      bool end = false;
      while(!end){
        //_logger->debug("I2CNetwork: asking slave "+String(slaves[i]));
        //Serial.print(".");
        Wire.requestFrom(slaves[i],DATA_PACKET );
        //_logger->debug("I2CNetwork: asking slave "+String(slaves[i])+" "+String(Wire.available()));
        String response = "";
        if(Wire.available()) {
          int address = Wire.read();
          int index = Wire.read();
          int actionid = Wire.read();
          //_logger->debug("I2CNetwork: data "+String(address)+" "+String(index)+" "+String(actionid));
          int state = (signed char) Wire.read()<<8 | Wire.read();
          // if(actionid!=255)
          //   _logger->debug("I2C raw message 2:"+String(address)+" "+String(index)+" "+String(actionid)+" "+String(state)+" -> "+String(doublebyte_to_binary(state)));

          String action = processAction(actionid,state);
          if(action=="Acknowledge")
            end=true;
          if(end || action!="")
            continue;
          
          
          if(address==0){ //message for Master
            //_logger->debug("I2C Message recieved for master from slave, "+String(address)+" "+String(index)+" "+String(actionid)+" "+String(state));

            String result = _controllers->performActionByIndex(index,buttonlookup(actionid),state);
            
            if(result!="")//error
              _logger->log(result+" using I2cNetwork");
          }else{ // message for another slave, so we pass it on
            String result = addToQueue(address,index,buttonlookup(actionid),state);
            if(result!="")
              _logger->log(result);
          }
          
            
        }else{
          //Bus fault or slave fault
          end=true;
          
          unsigned long currentTime = millis();
          transmissionError+=1;
          if(transmissionError>ERROR_BUS_FAULTS_BEFORE_RESET && transmissionErrorTime+5000<currentTime){
            _logger->log("I2CNetwork: Transmission Error 2 to slave "+String(slaves[i])+", resetting bus");
            Wire.begin();
            byte busStatus = Wire.endTransmission();
            transmissionError=0;
            transmissionErrorTime = currentTime;
          }
        }
      } 
    
    
    }
  }
  
}

void I2CNetwork::sendAction(String action,int value){//modify to send profile config number to over network
  if(action=="NextConfig"){
    for(int i=0;i<_numberOfSlaves;i++){
      addToQueue(slaves[i],0,"ChangeConfig",value);
    }
  }
  if(action=="PrevConfig"){
    for(int i=0;i<_numberOfSlaves;i++){
      addToQueue(slaves[i],0,"ChangeConfig",value);
    }
  }
  if(action=="Reset"){
    for(int i=0;i<_numberOfSlaves;i++){
      addToQueue(slaves[i],0,"Reset",-1);
    }
  }
  performAction();
}

String I2CNetwork::processAction(int actionid, int value) {
  String action = buttonlookup(actionid);
  if(action=="Acknowledge")
    return action;
  if(action=="ChangeConfig"){
    _controllers->setConfig(value);
    return action;
  }
  if(action=="Reset"){
    _controllers->initialize();
  }
  if(action=="GetConfig"){
    //_logger->debug("IC2Network: recieved getconfig");
    addToQueue(value, 0, "ChangeConfig", _controllers->getConfig());
  }
  return "";
}


void I2CNetwork::receiveEvent(int howMany) {
  if(initialized){
    //_logger->debug("recieveEvent");
    // Read while data received
    while (0 < Wire.available()) {
      int address = Wire.read();
      int controllerid = Wire.read();
      int actionid = Wire.read();
      // byte stateHigh = (signed char) Wire.read();
      // byte stateLow = (signed char) Wire.read();
      // int state = (signed char)(stateHigh<<8)|stateLow;
      int state = (signed char) Wire.read()<<8 | Wire.read();
      // if(actionid!=255)
      //   _logger->debug("I2C raw message 3: "+String(address)+" "+String(controllerid)+" "+String(actionid)+" "+String(state) +" -> "+String(doublebyte_to_binary(state)));
      if(address!=(int)name().toInt()){//This really shouldn't happen, but lets make sure
        _logger->log("Error: "+name()+" recieved message for another node "+String(address)+"-"+String(controllerid)+"-"+String(actionid)+"-"+String(state));
        Wire.begin((int)_name.toInt());
        continue;
      }
      //_logger->debug("recieveEvent: "+name()+" recieved message "+String(address)+"-"+String(controllerid)+"-"+String(actionid)+"-"+String(state));

      //String result = _controllers->performActionByIndex(controllerid,buttonlookup(action),state);
      String action = processAction(actionid,state);
      if(action == ""){
        String result = _controllers->performActionByIndex(controllerid,buttonlookup(actionid),state);
        if(result!="")//error
          _logger->log(result+" using I2cNetwork");
      }
    }

    // Print to Serial Monitor
    //_logger->debug("Receive event");
  }
}

void I2CNetwork::requestEvent() {
  if(initialized){
    //_logger->debug("requestEvent");
    int size = 0;
    bool dataToSend = false;
    for(int i=0;i<QUEUE_SIZE;i++){
      if(queueSlots[i]){
        size+=1;
        dataToSend = true;
      }
    }
    if(size==0)
      size = DATA_PACKET;
    else
      size = size*(DATA_PACKET);
    // Setup byte variable in the correct size
    byte response[size];
    if(dataToSend){
      int tmpnum=0;
      for(int i=0;i<QUEUE_SIZE;i++){
        if(queueSlots[i]){
          response[tmpnum+0]=queue[i][0];
          response[tmpnum+1]=queue[i][1];
          response[tmpnum+2]=queue[i][2];
          char highbyte=(queue[i][3] & 0xFF00)>>8; //shift right 8 bits, leaving only the 8 high bits. 
          char lowbyte=queue[i][3] & 0xFF; //bitwise AND with 0xFF
          response[tmpnum+3]=highbyte;
          response[tmpnum+4]=lowbyte;
          queueSlots[i]=false;
          tmpnum+=1;
          //_logger->debug("I2C raw message 4:"+String(queue[i][0])+" "+String(queue[i][1])+" "+String(queue[i][2])+" "+String(queue[i][3]) +" -> "+String(doublebyte_to_binary(queue[i][3])) +" ["+String(byte_to_binary(highbyte))+","+String(byte_to_binary(lowbyte))+"]");
        }
      }
    }else{
      response[0]=0;
      response[1]=0; 
      response[2]=actionlookup("Acknowledge"); 
      response[3]=0; 
      response[4]=0;
    }

    // Send response back to Master
    Wire.write(response,sizeof(response));

    // if(response[4]!=0)
    //   _logger->debug("Request event, to send "+String(dataToSend)+", sent "+String(sizeof(response)));
  }
}

bool I2CNetwork::isEnabled(){
  return enable;
}

bool I2CNetwork::isMaster(){
  return master;
}

//used for debugging
const char* I2CNetwork::doublebyte_to_binary(int x)
{
    static char b[17];
    b[0] = '\0';

    int z;
    for (z = 32768; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}
//used for debugging
const char* I2CNetwork::byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}