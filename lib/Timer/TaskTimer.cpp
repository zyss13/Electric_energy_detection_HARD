#include "TaskTimer.h"
#include "Arduino.h"

TaskTimer::TaskTimer(){
    this->second = 0;
    this->fun = NULL;
    this->count = 0;
    this->runTime = millis() + second*1000;
    this->model = -1;
};

TaskTimer::TaskTimer(int second,int model,Fun fun){
    this->second = second;
    this->fun = fun;
    this->count = 0;
    this->runTime = millis() + second*1000;
    this->model = model;
};

int TaskTimer::getSecond(){
    return this->second;
};

unsigned long TaskTimer::getRunTime(){
    return this->runTime;
};

int TaskTimer::getCount(){
    return this->count;
};

void TaskTimer::setRunTime(unsigned long time){
    this->runTime = time;
};

void TaskTimer::setCount(int count){
    this->count = count;
};
void TaskTimer::run(){
    if(model==0||(model==1&&count<1)){
        this->setRunTime(this->runTime+this->second*1000);
        this->fun();
        this->count++;
    }
};
