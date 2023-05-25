#include <Arduino.h>
#include <Wire.h>
#include "Logger.h"
#include "Config.h"
#include "Controllers.h"
#include "I2CNetwork.h"

I2CNetwork i2c;
Logger logger;
Config config;
Controllers controllers;

void setup() {
  
  logger.setup();
  logger.log("Arcade Controller Network start");

  controllers.Setup(&i2c,&logger);

  String configsetup = config.setup(&logger,&controllers,&i2c);
  if(configsetup!=""){
    logger.log(configsetup);
  }
  i2c.setup(config.getNetworkName(),config.getNumberOfSlaves(),config.getSlaves(),&controllers,&logger);

  //This is a little hackish to setup the wire handlers here, but apparently it is funny with objects, so here we are.
  if(i2c.isEnabled()){ 
    if(i2c.isMaster()){
      logger.log("Starting I2C Controller Node");
      Wire.begin();
    }else{
      int id = (int)i2c.name().toInt();
      logger.log("Starting I2C Child Node "+String(id));
      Wire.begin(id);
      Wire.onRequest([] () {i2c.requestEvent();});
      Wire.onReceive([] (int howMany) {i2c.receiveEvent(howMany);});
    }
  }

  controllers.initialize();


}

void loop() {
    
  unsigned long start = millis();
  controllers.performActions();
  i2c.performAction();
  XInput.send();
  unsigned long currentTiming = millis()-start;
  //Serial.print(".");
  if(currentTiming>20)
    logger.log("INFO: Main loop took "+String(currentTiming)+ " milliseconds.");
}
