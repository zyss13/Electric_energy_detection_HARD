#include "TimerHelper.h"
#include "Arduino.h"
#include "map"
#include "stdlib.h"

TimerHelper::TimerHelper(){
    this->length = 100;
    this->timers = new TaskTimer[this->length];
    this->size = 0;
};

TimerHelper::TimerHelper(int length){
    this->length = length;
    this->timers = new TaskTimer[this->length];
    this->size = 0;
};


void TimerHelper::add(TaskTimer timer){
    timers[size++] = timer;
}

void TimerHelper::loop(){
    unsigned long current = millis();
    for (int i = 0; i < size; i++)
    {
        if(this->timers[i].getRunTime() <= current){
            this->timers[i].run();
        }
    }
    
}