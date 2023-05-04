#include    <xc.h>
#include    <stdio.h>
#include    "alwaysCP_210728.h"
#include    "InterruptTimer24.h"
//#include    "FunctionLCD.h"
#define PERIOD_REPORT 100 // ���. 19.11.19
interval_t Interval;
UINT8   CounterDelaySec = 0;
UINT16  CounterDelayRs = 0;
UINT8   CounterNoAnswer[11];
static UINT16  CounterCheckStatusBU = PERIOD_REPORT;
static UINT16 Counter10ms;
//���������� �� ������� 1 1��
UINT32 Counter1ms;
void __attribute__ ((interrupt,no_auto_psv)) _T1Interrupt (void)
{   
    Counter1ms++;
_T1IF = 0;								//����� ������� ����������
} //����� ����������� ���������� �� TMR1
//���������� �� ������� 3 10��
void __attribute__ ((interrupt,no_auto_psv)) _T3Interrupt (void)
{
    _T3IF = 0; //����� ������� ����������
    Counter10ms ++;
    if(!(Counter10ms%5)){
        Interval._50ms = 1;
        Interval.usb50ms = 1;
    }
//    if(Counter10ms > 4){
//        Counter10ms = 0;
//        Interval._50ms = 1;
//    }
    Interval.Key_10ms = 1;
    Interval.Rs_10ms = 1;
//    if(CounterDelayRs){ // ���. 30.01.20 CounterDelayRs �������� �� 1 �� ������� � 10��
//        CounterDelayRs --;
//        return;
//    }
    if(CounterCheckStatusBU){
        CounterCheckStatusBU --;
        if(!CounterCheckStatusBU){
            CounterCheckStatusBU = PERIOD_REPORT;
        }
    }
} // T3 Interrupt

TimeStruct CurrentTime;
static UINT8 Counter100ms = 0;
UINT8    CounterInfo = 60;
//���������� �� �������2 
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
// ���� ��������� �������
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
    if(!(Counter100ms%5)){
        Interval._500ms = 1;
    }
    Interval._100ms = 1;
    Interval._SoundCtrl = 1;
    _T2IF = 0;								//����� ������� ����������
} // T3 Interrupt
UINT16 CounterWork;
void __attribute__ ((interrupt,no_auto_psv)) _T4Interrupt (void)
{
    CounterWork ++;
    if(CounterDelayMs)CounterDelayMs --;
    _T4IF = 0;								
}