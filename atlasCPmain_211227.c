/* 
 * File:   AtlasCPmain_200521.c
 * Author: 11
 *
 * Created on 30 марта 2020 г., 13:16
 */
#include    <xc.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <libpic30.h>
//#include    "SetConfigurationGA106.h"
#include    "alwaysCP_210728.h"
#include    "init256GA106AtlasCP.h"
#include    "terminal.h"
#include    "interruptTimer24.h"
#include    "functionCP.h"
#include    "driverAt45.h"
#include    "functionCP.h"
#include    "DriverMFRC522cp.h" // изм. 30.03.22
#include    "FunctionRS485_K.h"
#include    "functionUART24.h"




int main(void) {
    ProcessorInit();
// ОЧИСТКА ПАМЯТИ
//    EraseAt45();
//    while(1){
//        if(!SOUND)SOUND = 1;
//    }
    MFRC522_Init();
// считываем установки в переменные
//    MySerialNumber = ReadLongFromAT45(2047,0); // считываем свой СН из 0...3 ячеек 2047 страницы at45
    AccessPassword = ReadLongFromAT45(2047,44);// считываем пароль доступа из 44...47 ячеек 2047 страницы at45
    if(AccessPassword == 0xFFFFFFFF){
        AccessPassword = 1234;
    }
    CurrentEventWrite = ReadCurrentPositionAt45();
    NumberOfResponses[0] = 0;
    UINT8 ii;
    for(ii = 1; ii < 11; ii ++){
        UINT32 tempSN = 0;
        // считываем СН зарегистрированных БУ из 4...43 ячеек 2047 страницы at45
        tempSN = ReadLongFromAT45(2047,(ii*4));
        if(tempSN == 0xFFFFFFFF){
            StatusBU[ii].SerialNumber = 0;
            SaveSerialNumberBU(StatusBU[ii].SerialNumber, (ii * 4));
        }else{
            StatusBU[ii].SerialNumber = tempSN;
            if(tempSN)NumberOfResponses[0] ++;     // изм. 05.04.22 учет зарегистрированных приборов
        }
    }
    for(ii = 1; ii < 11; ii ++){
        // считываем номера КИД 0...79 ячеек 2045 страницы at45
        ReadRegisteredKid(ii); // изм. 01.04.22
    }
    ModeRs = ReadCharFromAt45(2047,250);   // изм. 13.04.22
    SelectModeRs485(ModeRs);              // изм. 13.04.22
//    LCD_EN = 1;
    Interval._1s = 0;
    TempDirectRs = 0;
    TempCommandRs = 0;
    Interval._CheckStatusBU = 1;
//    WaitingScreen = 0;
    ControlFlagCP.NoBurLast = 1;
    ControlFlagCP.RsBreakLast = 1;
    while(1){
        if(Interval._CheckStatusBU){
            (void)CheckStatusBU201106(DirectControl);
            //флаг Interval._CheckStatusBU снимается в функции CheckStatusBU4() 
            //после опроса одного зарегистрированного БУ
        }else{
            if(Interval._250ms){
                Interval._250ms = 0;
                CounterWork = 0;
                do{
                    DirectControl ++;
                    if(DirectControl > 10){
    // изм. 05.04.22++++++++++          
                        switch(ControlFlagCP.CurrentModeRs){
                            case 0:
                                SelectModeRs485(0);
                                break;
                            case 1:
                                SelectModeRs485(1);
                                break;
                            case 2:
                                SelectModeRs485(3);
                                break;
                            case 3:
                                SelectModeRs485(2);
                                break;
                        }
    //++++++++++++++++++++++++                  
                        DirectControl = 1;
    //                            LcdFlag.NewData = 1;
                    }
                }while(!StatusBU[DirectControl].SerialNumber/* && DirectControl != 1*/);
                /*отладка*/if(LcdFlag.Debug)xprintf("CounterWork %u\r",CounterWork);
                Interval._CheckStatusBU = 1;
            }
        }
        TerminalRs();
    }
    return 0;
}

