#include "ControllerObj.h"


#ifndef IRCameraControllerObj_h
#define IRCameraControllerObj_h


class Logger;


class IRCameraControllerObject: public ControllerObject{
  public:
    IRCameraControllerObject(Logger* logger);

    void initialize() override;
    String performAction(int groupState);

  private:

    void readData();
    void Write_2bytes(byte d1, byte d2);
    bool findInitialFraming();
    bool seeAllPoints();
    bool anyDeadzonePoints();

    int slaveAddress;

    byte data_buf[16];
    int i;

    int Ix[4];
    int Iy[4];
    int s;

    int frameVectorsX[12];
    int frameVectorsY[12];

    int IRsensorAddress = 0xB0;
//int IRsensorAddress = 0x58;

  };
#endif