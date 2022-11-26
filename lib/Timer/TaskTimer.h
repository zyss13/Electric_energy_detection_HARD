#ifndef TaskTimer_h
#define TaskTimer_h
class TaskTimer{
private:
    typedef void (*Fun)();
    int second;  //间隔多少秒运行
    unsigned long runTime;  // 运行时的毫秒值
    int count;   //运行了多少次
    int model;   //运行的模式 1:只运行一次 0:运行多次
    Fun fun;
public:
    TaskTimer();

    TaskTimer(int second,int model,Fun fun);

    int getSecond();

    unsigned long getRunTime();

    int getCount();

    void setRunTime(unsigned long time);

    void setCount(int count);

    void run();
};


#endif