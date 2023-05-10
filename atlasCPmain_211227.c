/* 
 * File:   AtlasCPmain_200521.c
 * Author: 11
 *
 * Created on 30 ����� 2020 �., 13:16
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
#include    "DriverMFRC522cp.h" // ���. 30.03.22
#include    "FunctionRS485_K.h"
#include    "functionUART24.h"




int main(void) {
    ProcessorInit();
// ������� ������
//    EraseAt45();
//    while(1){
//        if(!SOUND)SOUND = 1;
//    }
    MFRC522_Init();
// ��������� ��������� � ����������
//    MySerialNumber = ReadLongFromAT45(2047,0); // ��������� ���� �� �� 0...3 ����� 2047 �������� at45
    AccessPassword = ReadLongFromAT45(2047,44);// ��������� ������ ������� �� 44...47 ����� 2047 �������� at45
    if(AccessPassword == 0xFFFFFFFF){
        AccessPassword = 1234;
    }
    CurrentEventWrite = ReadCurrentPositionAt45();
    NumberOfResponses[0] = 0;
    UINT8 ii;
    for(ii = 1; ii < 11; ii ++){
        UINT32 tempSN = 0;
        // ��������� �� ������������������ �� �� 4...43 ����� 2047 �������� at45
        tempSN = ReadLongFromAT45(2047,(ii*4));
        if(tempSN == 0xFFFFFFFF){
            StatusBU[ii].SerialNumber = 0;
            SaveSerialNumberBU(StatusBU[ii].SerialNumber, (ii * 4));
        }else{
            StatusBU[ii].SerialNumber = tempSN;
            if(tempSN)NumberOfResponses[0] ++;     // ���. 05.04.22 ���� ������������������ ��������
        }
    }
    for(ii = 1; ii < 11; ii ++){
        // ��������� ������ ��� 0...79 ����� 2045 �������� at45
        ReadRegisteredKid(ii); // ���. 01.04.22
    }
    ModeRs = ReadCharFromAt45(2047,250);   // ���. 13.04.22
    SelectModeRs485(ModeRs);              // ���. 13.04.22
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
            //���� Interval._CheckStatusBU ��������� � ������� CheckStatusBU4() 
            //����� ������ ������ ������������������� ��
        }else{
            if(Interval._250ms){
                Interval._250ms = 0;
                CounterWork = 0;
                do{
                    DirectControl ++;
                    if(DirectControl > 10){
    // ���. 05.04.22++++++++++          
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
                /*�������*/if(LcdFlag.Debug)xprintf("CounterWork %u\r",CounterWork);
                Interval._CheckStatusBU = 1;
            }
        }
        TerminalRs();
    }
    return 0;
}

