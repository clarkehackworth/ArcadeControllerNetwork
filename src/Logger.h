#include <SD.h>
#include "Config.h"

#ifndef Logger_h
#define Logger_h

# define LOG_TO_SD 1
# define LOG_TO_SERIAL 1 // not available when in xinput mode to emulate xbox controller
# define DEBUG 1

class Logger {

  public:
    Logger();
    void setup();
    void log(String data);
    void debug(String data);
    void error(String data);
    void flush();

  private:  
    const char* filename = "Ardcade_log.txt";
    File dataFile;
    bool enable = false;

    Sd2Card card;
};

#endif

