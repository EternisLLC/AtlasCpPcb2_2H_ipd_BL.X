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
#include    "crc8.h"
#include    "FunctionRS485_K.h"
#include    "functionUART24.h"

/*
 * проверка GitHub
 */
UINT8 DirectControl = 1;
UINT32 tempSN = 0;
UINT16 CounterDelayWork = 100;
static UINT16   CounterJampPage;

int main(void) {
    UINT8 ii;
    ProcessorInit();
// ОЧИСТКА ПАМЯТИ
//    EraseAt45();
//    while(1){
//        if(!SOUND)SOUND = 1;
//    }
    MFRC522_Init();
// считываем установки в переменные
    MySerialNumber = ReadLongFromAT45(2047,0); // считываем свой СН из 0...3 ячеек 2047 страницы at45
    AccessPassword = ReadLongFromAT45(2047,44);// считываем пароль доступа из 44...47 ячеек 2047 страницы at45
    if(AccessPassword == 0xFFFFFFFF){
        AccessPassword = 1234;
    }
    CurrentEventWrite = ReadCurrentPositionAt45();
    NumberOfResponses[0] = 0;
    for(ii = 1; ii < 11; ii ++){
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
// изм. 29.03.22         RegistrationNumberIbuttonRead(ii); 
    }
    ModeRs = ReadCharFromAt45(2047,250);   // изм. 13.04.22
    SelectModeRs485(ModeRs);              // изм. 13.04.22
    LCD_EN = 1;
//    printf("restЪЪЪ"); // жчочлпуомачлльх ъюомъ ТИ
//    printf("restЪЪЪ"); // гарантированный сброс ЖК
    
    Interval._1s = 0;
    TempDirectRs = 0;
    TempCommandRs = 0;
    Interval._CheckStatusBU = 1;
    WaitingScreen = 0;
    while(1){
        while(CurrentScreen > 9 && CurrentScreen != 15 && CurrentScreen != 255){

//            if(ScreenLast != CurrentScreen){
//                xprintf("%u->%u dir %u\r",ScreenLast,CurrentScreen,SelectedDirection);
//                ScreenLast = CurrentScreen;
//            }
            if(Interval._CheckStatusBU){
                (void)CheckStatusBU201106(DirectControl);
                //флаг Interval._CheckStatusBU снимается в функции CheckStatusBU4() 
                //после опроса одного зарегистрированного БУ
            }
//            if(!CounterDelayRs && (ENTX485 || !ENTX485_I)){ // если мы не ждем данные и в этот момент нет передачи
            if(!CounterDelayRs && !ENTX485 && ENTX485_I){ // если мы не ждем данные и в этот момент нет передачи
            /*
            здесь можно вклинивать передачу команд управления
            т.е. мы не ждем данные от подключенных БУ и мы сами 
            не работаем на передачу
            */
                if(TempCommandRs){
                    if(TempDirectRs){
                        // передача команд по направлениям
                        UINT8   kkk;
                        // если есть данные для передачи передаем пакты всем заинтересованным БУ
                        if(TempCommandRs){
                            RsTxLength = sprintf((char*)RsTxBuffer,"? %lu %u ",StatusBU[TempDirectRs].SerialNumber,TempCommandRs);
                            RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // вычисляем контрольную сумму 
                            RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                            TransmittRsPacket();
                            while(ENTX485 || !ENTX485_I); // ожидаем окончания предыдущей передачи
                        }
                        if(TempCommandRs > 3 && TempCommandRs < 11){ // изм. 12.09.22
                            for(kkk = 1; kkk < 11; kkk++){
                                if(kkk != TempDirectRs && StatusBU[kkk].Direct == StatusBU[TempDirectRs].Direct){
                                    while(ENTX485 || !ENTX485_I); // ожидаем окончания предыдущей передачи
                                    RsTxLength = sprintf((char*)RsTxBuffer,"? %lu %u ",StatusBU[kkk].SerialNumber,TempCommandRs);
                                    RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // вычисляем контрольную сумму 
                                    RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                                    TransmittRsPacket();
                                    while(ENTX485 || !ENTX485_I); // ожидаем окончания предыдущей передачи
                                }
                            }
                        }
                        TempDirectRs = 0;
                    }else{
                        // передача ШВ команд
                        RsTxLength = sprintf((char*)RsTxBuffer,"? %lu %u ",0xFFFFFFFF,TempCommandRs);
                        RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // вычисляем контрольную сумму 
                        RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                        TransmittRsPacket();
                        while(ENTX485 || !ENTX485_I); // ожидаем окончания предыдущей передачи
                    }
                    TempCommandRs = 0;
                }
            }
            if(Interval._1Sec){
                if(!Interval._CheckStatusBU){
                    IndicatorDirection(DirectControl);
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
                    Interval._CheckStatusBU = 1;
                }
                /*if(CurrentScreen != 13)*/Interval._1Sec = 0;
                Reset_IKZ ();
                                           
            }
            if(Interval._100ms){
                CheckStausPWR();
                // 30.09.22
                if(ControlFlagCP.ErrorRip != IN_ERR_RIR){   // 30.09.22
                    ControlFlagCP.ErrorRip = IN_ERR_RIR;
                    if(CurrentScreen == 10){     
                        if(ControlFlagCP.ErrorRip){
                            printf("page10.t13.pic=56ЪЪЪ");
                        }else{
                            printf("page10.t13.pic=57ЪЪЪ");
                        }
                    }
                }
                Interval._100ms = 0;
            }
//+++++++++++            
            KeyCheckMain();
            KeyHandler();
//+++++++++++            
            TerminalRs ();
            IndicationControl();
            TerminalLcd();
            if(LcdFlag.WaitNewScreen1){// если выставлен флаг перехода на новую страницн
                if(CurrentScreen == 15){
                    SOUND = TempSound;
                    LED_ERROR = TempLedError;
                    LED_FIRE = TempLedFire;
                    LED_START = TempLedStart;
                    LED_AUTO = TempLedAuto;
                }
//            printf("page %uЪЪЪ",(UINT8)param[0]); // передаем команду перехода на указанную станицу 
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page %uЪЪЪ",WaitingScreen);
                printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s 1b\r",LcdBufferData);
                Counter1ms = 0;
                CounterWaitingNewScreen = 1000;
                // сбрасываем все флаги воспроизведения при переходе на новую страницу
                LcdFlag.Yes = 0;
                LcdFlag.HandDoun = 0;
                LcdFlag.HandUp = 0;
                LcdFlag.Play = 0;
                while(!LcdFlag.NewPage ){
                    TerminalLcd();
                    if(Interval._50ms){
                        CounterJampPage ++;
                        Interval._50ms = 0;
                        if(!(CounterJampPage%20)){
                            /*отладка*/if(LcdFlag.Debug)xprintf("waitin jamp to page %u\r",WaitingScreen);
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page %uЪЪЪ",WaitingScreen);
                            printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s 2b\r",LcdBufferData);
                        }
                    }
                }
                CounterJampPage = 0;
            }
//=================            
            if(CommandDebug == 1){
                if(!LcdFlag.Debug){
                    LcdFlag.Debug = 1;
                    xprintf("LcdFlag.Debug = 1;\r");
                }
            }else{
                if(LcdFlag.Debug){
                    LcdFlag.Debug = 0;
                    xprintf("LcdFlag.Debug = 0;\r");
                }
            }
//===================            
        }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++        
    if(CommandDebug == 1){
        if(!LcdFlag.Debug){
            LcdFlag.Debug = 1;
            xprintf("LcdFlag.Debug = 1;\r");
        }
    }else{
        if(LcdFlag.Debug){
            LcdFlag.Debug = 0;
            xprintf("LcdFlag.Debug = 0;\r");
        }
    }
    if(LcdFlag.WaitNewScreen1){// если выставлен флаг перехода на новую страницн
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page %uЪЪЪ",WaitingScreen);
            printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s 1a\r",LcdBufferData);
            Counter1ms = 0;
            CounterWaitingNewScreen = 1000;
            // сбрасываем все флаги воспроизведения при переходе на новую страницу
            LcdFlag.Yes = 0;
            LcdFlag.HandDoun = 0;
            LcdFlag.HandUp = 0;
            LcdFlag.Play = 0;
            while(!LcdFlag.NewPage ){
                TerminalLcd();
                if(Interval._50ms){
                    CounterJampPage ++;
                    Interval._50ms = 0;
                    if(!(CounterJampPage%20)){
                        /*отладка*/if(LcdFlag.Debug)xprintf("waitin jamp to page %u\r",WaitingScreen);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page %uЪЪЪ",WaitingScreen);
                        printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s 2a\r",LcdBufferData);
                    }
                }
            }
            
            CounterJampPage = 0;
        }
        TerminalLcd();
// изм. 01.04.22       
//        if(Interval._100ms){
////            xprintf("Timer  %u\r",Timer++);
//            if(RsMode !=(UINT8)ControlFlagCP.CurrentModeRs){
//                RsMode =(UINT8)ControlFlagCP.CurrentModeRs;
//                xprintf("ModeRs  %u\r",RsMode);
//            }
//            Interval._100ms = 0;
//        }
        switch(CurrentScreen){
            case 0:
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                TestKey(); 
                break;
            case 4:
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                if(Interval._1s){
                    if(!SerialNumberKid[NumberKID].SerialKid){ // изм. 20.06.22
//                        printf("page4.b0.txt=\"ЗАПИСАТЬ\"ЪЪЪ");
                        SerialNumberKid[0].SerialKid = 0;
                        FlagMFRC522._ReadCart = 1;
                        FlagMFRC522._newCart = 0;
                        counterReadCart = 3;
                        while(!FlagMFRC522._newCart && counterReadCart){
                            ReadSerialNumberKID(SerialNumberKid[0].BufferKid);
                            counterReadCart --;
                        }
                        if(SerialNumberKid[0].SerialKid){
                            printf("page4.t4.txt=\"%lu\"ЪЪЪ", SerialNumberKid[0].SerialKid);
                        }else{
                            printf("page4.t4.txt=\"0\"ЪЪЪ");
                            printf("page4.t14.txt=\"\"ЪЪЪ");
                        }
                    }else{
//                        printf("page4.b0.txt=\"УДАЛИТЬ\"ЪЪЪ");
                    }
                    Interval._1s = 0;
                }                
                break;
            case 8:
                DisplayReadArhiv0525();
                break;
            case 9:
                TransmittArhivUSB();
                break;
            case 15:
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                if(Interval._100ms){
                    SOUND = !SOUND;
                    LED_ERROR = !LED_ERROR;
                    LED_FIRE = !LED_FIRE;
                    LED_START = !LED_START;
                    LED_AUTO = !LED_AUTO;
                    Interval._100ms = 0;
                }
                break;
            default:
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                break;
        }
//=============        
// изм. 01.04.22        DisplayReadArhiv0525(); 
// изм. 01.04.22        TransmittArhivUSB();
    }
    return 0;
}

