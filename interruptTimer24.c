#include    <xc.h>
#include    <stdio.h>
#include    "alwaysCP_210728.h"
#include    "InterruptTimer24.h"
//#include    "FunctionLCD.h"
#define PERIOD_REPORT 100 // ÉÚÍ. 19.11.19
interval_t Interval;
UINT8   CounterDelaySec = 0;
UINT16  CounterDelayRs = 0;
UINT8   CounterNoAnswer[11];
static UINT16  CounterCheckStatusBU = PERIOD_REPORT;
static UINT16 Counter10ms;
//ĞÒÅÒÙ×ÁÎÉÑ ÏÔ ÔÁÊÍÅÒÁ 1 1ÍÓ
UINT32 Counter1ms;
void __attribute__ ((interrupt,no_auto_psv)) _T1Interrupt (void)
{   
    Counter1ms++;
_T1IF = 0;								//ñáğîñ âåêòîğà ïğåğûâàíèÿ
} //ËÏÎÅÃ ÏÂÒÁÂÏÔŞÉËÁ ĞÒÅÒÙ×ÁÎÉÑ ÏÔ TMR1
//ĞÒÅÒÙ×ÁÎÉÑ ÏÔ ÔÁÊÍÅÒÁ 3 10ÍÓ
void __attribute__ ((interrupt,no_auto_psv)) _T3Interrupt (void)
{
    _T3IF = 0; //ñáğîñ âåêòîğà ïğåğûâàíèÿ
    Counter10ms ++;
    if(Counter10ms > 4){
        Counter10ms = 0;
        Interval._50ms = 1;
    }
    Interval._10ms = 1;
    Interval.Key_10ms = 1;
    if(CounterDelayRs){ // ÉÚÍ. 30.01.20 CounterDelayRs ĞÅÒÅÎÅÓÎ ÉÚ 1 ÍÓ ÔÁÊÍÅÒÁ × 10ÍÓ
        CounterDelayRs --;
        return;
    }
    if(CounterCheckStatusBU){
        CounterCheckStatusBU --;
        if(!CounterCheckStatusBU){
            CounterCheckStatusBU = PERIOD_REPORT;
//            Interval._CheckStatusBU = 1;
        }
    }
} // T3 Interrupt

TimeStruct CurrentTime;
static UINT8 Counter100ms = 0;
UINT8    CounterInfo = 60;
//Ïğåğûâàíèå îò òàéìåğà2 
void __attribute__ ((interrupt,no_auto_psv)) _T2Interrupt (void)
{
    if(Counter100ms){
        Counter100ms --;
    }else{
        Counter100ms = 9;
        Interval._1s = 1;
        Interval._1Sec = 1;
        Interval._lcd = 1;
        if(CounterDelaySec){
            CounterDelaySec --;
        }
// ÷àñû ğåàëüíîãî âğåìåíè
        CurrentTime.Second ++;
        if(CurrentTime.Second == 60){
            CurrentTime.Second = 0;
            CurrentTime.Minute ++;
            Interval._1min = 1;
            if(CurrentTime.Minute == 60){
                CurrentTime.Minute = 0;
                CurrentTime.Hour ++;
                if(CurrentTime.Hour==24){
                    CurrentTime.Hour = 0;
                }
            }
        }
    }
    Interval._100ms = 1;
    Interval._SoundCtrl = 1;
    _T2IF = 0;								//ñáğîñ âåêòîğà ïğåğûâàíèÿ
} // T3 Interrupt
void __attribute__ ((interrupt,no_auto_psv)) _T4Interrupt (void)
{
    if(CounterDelayMs)CounterDelayMs --;
    _T4IF = 0;								
}