#ifndef TimerHelper_h
#define TimerHelper_h

#include "Arduino.h"
#include "TaskTimer.h"

class TimerHelper{
private:
    
    TaskTimer *timers;
    int length;
    int size;
    

public:
    TimerHelper();

    TimerHelper(int length);

    void add(TaskTimer timer);

    void loop();
};

#endif