#include "Logger.h"

Logger::Logger(){

}

void Logger::setup() {
  Serial.begin(9600);
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card failed, or not present");
    return;
  } else {
    enable = true; 
    
  }

  log("SD Card Initialized");
}

void Logger::log(String data) {
  
  if(enable && LOG_TO_SD){
    //Serial.print("Writing ");
    //Serial.println(data);
    
    dataFile = SD.open(filename, FILE_WRITE);
    dataFile.println("[+] "+data);
    dataFile.close();
    //Serial.println("log written");
  }
  if(LOG_TO_SERIAL){
    Serial.println("[+] "+data);
  }
}

void Logger::debug(String data) {
  if(DEBUG){
    if(enable && LOG_TO_SD){
      //Serial.print("Writing ");
      //Serial.println(data);
      dataFile = SD.open(filename, FILE_WRITE);
      dataFile.println("[d] "+data);
      dataFile.close();
      //Serial.println("log written");
    }
    if(LOG_TO_SERIAL){
      Serial.println("[d] "+data);
    }
  }
}

