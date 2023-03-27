#include <xc.h> // include processor files - each processor file is guarded.  


typedef struct {
    int _10ms  :1;
    int _100ms :1;     
    int _1s    :1;
    int _1Sec   :1;
    int _lcd   :1;
    int _CheckStatusBU   :1;
    int _SoundCtrl  :1;
    int _1min       :1;
    int _50ms       :1;
    int Key_10ms    :1;
}interval_t;  //флаги интервалов времени



extern TimeStruct CurrentTime;

extern interval_t Interval;
extern UINT8   CounterDelaySec;
//extern UINT16   CounterDelayMs;
extern UINT16  CounterDelayRs;
extern UINT8    CounterNoAnswer[11];
extern UINT32 Counter1ms;


