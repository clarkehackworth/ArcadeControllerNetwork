#include <Wire.h>
#include "Logger.h"
#include "IRCameraControllerObj.h"

IRCameraControllerObject::IRCameraControllerObject(Logger* logger){
 
  _logger = logger;
}

void IRCameraControllerObject::initialize(){
  slaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI
  Wire.begin();
  // IR sensor initialize
  Write_2bytes(0x30,0x01); delay(10);
  Write_2bytes(0x30,0x08); delay(10);
  Write_2bytes(0x06,0x90); delay(10);
  Write_2bytes(0x08,0xC0); delay(10);
  Write_2bytes(0x1A,0x40); delay(10);
  Write_2bytes(0x33,0x33); delay(10);
  delay(100);
  for (i=0;i<16;i++) { data_buf[i]=0; }
  for (i=0;i<14;i++) {frameVectorsX[i]=0;frameVectorsY[i]=0;}
}

String IRCameraControllerObject::performAction(int groupState){
  readData();
  findInitialFraming();
}

void IRCameraControllerObject::readData(){
    Serial.println("readData");
//IR sensor read
    Wire.beginTransmission(slaveAddress);
    Wire.write(0x36);
    Wire.endTransmission();

    Wire.requestFrom(slaveAddress, 16);        // Request the 2 byte heading (MSB comes first)
    // for (i=0;i<16;i++) { data_buf[i]=0; }
    i=0;
    while(Wire.available() && i < 16) {
        data_buf[i] = Wire.read();
        i++;
    }

    Ix[0] = data_buf[1];
    Iy[0] = data_buf[2];
    s   = data_buf[3];
    Ix[0] += (s & 0x30) <<4;
    Iy[0] += (s & 0xC0) <<2;

    Ix[1] = data_buf[4];
    Iy[1] = data_buf[5];
    s   = data_buf[6];
    Ix[1] += (s & 0x30) <<4;
    Iy[1] += (s & 0xC0) <<2;

    Ix[2] = data_buf[7];
    Iy[2] = data_buf[8];
    s   = data_buf[9];
    Ix[2] += (s & 0x30) <<4;
    Iy[2] += (s & 0xC0) <<2;

    Ix[3] = data_buf[10];
    Iy[3] = data_buf[11];
    s   = data_buf[12];
    Ix[3] += (s & 0x30) <<4;
    Iy[3] += (s & 0xC0) <<2;
    Serial.println("readData End");
}

void IRCameraControllerObject::Write_2bytes(byte d1, byte d2){
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1); Wire.write(d2);
    Wire.endTransmission();
}

bool IRCameraControllerObject::findInitialFraming(){
  if(!seeAllPoints())
    return false;
  
  if(anyDeadzonePoints())
    return false;

  //calculate vectors of each point to each other point
  for (i=0;i<4;i++) { 
      int adjustedi1=i+1;
      int adjustedi2=i+2;
      int adjustedi3=i+3;
      if(adjustedi1>=4)
        adjustedi1=4-adjustedi1;
      if(adjustedi2>=4)
        adjustedi2=4-adjustedi2;
      if(adjustedi3>=4)
        adjustedi3=4-adjustedi3;
      frameVectorsX[i]=Ix[i]-Ix[adjustedi1];
      frameVectorsX[i+1]=Ix[i]-Ix[adjustedi2];
      frameVectorsX[i+2]=Ix[i]-Ix[adjustedi3];
      frameVectorsY[i]=Iy[i]-Iy[adjustedi1];
      frameVectorsY[i+1]=Iy[i]-Iy[adjustedi2];
      frameVectorsY[i+2]=Iy[i]-Iy[adjustedi3];
  }

  //find point 1 and point 4
  int point1 = -1;
  int point1Lowest=0;
  int point4 = -1;
  int point4Highest=0;
  for(int i=0;i<4;i++){
    for(int j=0;j<3;i++){
      if(frameVectorsX[i+j]+frameVectorsY[i+j]<point1Lowest){//point 1 will always have the lostest combined vector from abother point
        point1Lowest=frameVectorsX[i+j]+frameVectorsY[i+j];
        point1=i;
      }
      if(frameVectorsX[i+j]+frameVectorsY[i+j]>point4Highest){//point 4 will always have the largest combined vector from abother point
        point4Highest=frameVectorsX[i+j]+frameVectorsY[i+j];
        point4=i;
      }
    }
  }

  //find point 2 and point 3
  int point2 = -1;
  int point2Highest=0;
  int point3 = -1;
  int point3Lowest=0;
  for(int i=0;i<4;i++){
    for(int j=0;j<3;i++){
      if(i!=point1 && i!=point4){
        if(frameVectorsX[i+j]>point2Highest){//point 1 will always have the lostest combined vector from abother point
          point2Highest=frameVectorsX[i+j];
          point2=i;
        }
        if(frameVectorsX[i+j]<point3Lowest){//point 4 will always have the largest combined vector from abother point
          point3Lowest=frameVectorsX[i+j];
          point3=i;
        }
      }
    }
  }
  Serial.print(String(Ix[point1])+","+String(Iy[point1]));
  Serial.print(","+String(Ix[point2])+","+String(Iy[point2]));
  Serial.print(","+String(Ix[point3])+","+String(Iy[point3]));
  Serial.println(","+String(Ix[point4])+","+String(Iy[point4]));
}

bool IRCameraControllerObject::seeAllPoints(){
  Serial.print(".");
  for (i=0;i<4;i++){
    if(Ix[i]==1023 && Iy[i]==1023){
      Serial.print(":");
      return false;
    }
  }
  Serial.print("*");
  return true;
}

bool IRCameraControllerObject::anyDeadzonePoints(){//points that are too close to the middle
  Serial.print("-");
  for (i=0;i<4;i++){
    if((Ix[i]>256 && Ix[i]<768) || (Iy[i]>256 && Iy[i]<768))//TODO: replace with circle calc instead of square
      return true;
  }
  return false;
}