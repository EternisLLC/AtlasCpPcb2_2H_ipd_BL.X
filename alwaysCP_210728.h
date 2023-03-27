/* 
 * File:   AlwaysCP.h
 * Author: 11
 *
 * Created on 21 ЛЮЪ 2021 г., 14:41
 */

#ifndef ALWAYSCP_H
#define	ALWAYSCP_H

#ifdef	__cplusplus
extern "C" {
#endif
//РАСПАКРВКА ВЕРСИИ ПРОГРАММЫ
    #define VERSION 230202
    #define VerY    VERSION/10000
    #define VerM    (VERSION/100)%100
    #define VerD    VERSION%100    
    #define FCY 32000000UL/2
    // ++++++++ГЛОБАЛЬНЫЕ ОПРЕДЕЛЕНИЯ++++++++++++++
    typedef unsigned char   UINT8;
    typedef unsigned int    UINT16;
    typedef unsigned long   UINT32;
    typedef signed char   INT8;
    typedef signed int    INT16;
    typedef signed long   INT32;
    #define GREEN   (UINT16)14180//2032//34800
    #define GREEN_B   (UINT16)34800
    #define GREY_B  (UINT16)50712
    #define GREY_BB (UINT16)61309
    #define GREY_D  (UINT16)31695
    #define ORANG   (UINT16)64992//64512//64520
    #define RED     (UINT16)63488
    #define BORDO   (UINT16)55715//32768
    #define YELOW   (UINT16)65504
    #define YELOW_B   (UINT16)63315
    #define WHITE   (UINT16)65535
    #define BLACK   (UINT16)0
    #define in          1
    #define out         0
    #define	DIGITAL		1
    #define	ANALOG      0
    // переназначение выходов
    #define LED_START       LATFbits.LATF1  // схема 2.1  нога 59
    #define TRIS_LED_START  TRISFbits.TRISF1
    #define LED_FIRE        LATFbits.LATF0  // схема 2.1  нога 58
    #define TRIS_LED_FIRE   TRISFbits.TRISF0
    #define LCD_EN          LATDbits.LATD6
    #define TRIS_LCD_EN     TRISDbits.TRISD6
    #define LED_ERROR         LATDbits.LATD7  // схема 2.1  нога 55
    #define TRIS_LED_ERR    TRISDbits.TRISD7
    #define EXT_SOUND       LATBbits.LATB11  // схема 2.1  нога 24
    #define TRIS_EXT_SOUND  TRISBbits.TRISB11
    #define SOUND           LATGbits.LATG8  // схема 2.1  нога 6
    #define TRIS_SOUND      TRISGbits.TRISG8
    #define LED_USB_REDY        PORTGbits.RG2    // на схеме не указана нога 37
    #define TRIS_LED_USB_REDY   TRISGbits.TRISG2
    
    #define LED_AUTO       LATEbits.LATE0           //изм. 12.07.22
    #define TRIS_LED_AUTO  TRISEbits.TRISE0         //изм. 12.07.22
    #define LED_PWR1       LATDbits.LATD3           //изм. 12.07.22
    #define TRIS_LED_PWR1   TRISDbits.TRISD3        //изм. 12.07.22
    #define LED_PWR2       LATDbits.LATD2           //изм. 12.07.22
    #define TRIS_LED_PWR2   TRISDbits.TRISD2        //изм. 12.07.22
    
    #define OUT_ERROR   LATBbits.LATB8
    #define TRIS_OUT_ERROR  TRISBbits.TRISB8
    
    #define OUT_FIRE       LATBbits.LATB9
    #define TRIS_OUT_FIRE  TRISBbits.TRISB9
    
    #define OUT_START       LATBbits.LATB10
    #define TRIS_OUT_START  TRISBbits.TRISB10
    
    
    #define TXRS_ISO       LATFbits.LATF3       // изменения для pcb 2.2 и TD331S485H-A
    #define TRIS_TXRS_ISO  TRISFbits.TRISF3     // изменения для pcb 2.2 и TD331S485H-A
    // переназначение входов
    #define KEY_AUTO        PORTCbits.RC14  // схема 2.1  нога 1
    #define KEY_START       PORTDbits.RD1  // схема 2.1  нога 2
    #define KEY_SOUND       PORTCbits.RC13  // схема 2.1  нога 44
    #define USB_EN          PORTGbits.RG3  // схема 2/1  нога 43
    #define IN_ERR_RIR      PORTBbits.RB2  // изм. 13.04.22
    // изм. 06.04.22
    #define MaxNumberRequests 12 
    //+++++++++++++++++++++++++++++++++++++++++++++++
    typedef struct{
        UINT16   Year;
        UINT8   Month;
        UINT8   Day;
        UINT8   Hour;
        UINT8   Minute;
        UINT8   Second;
    }TimeStruct;
    typedef    union {
        struct{
            UINT8 Bos        :1;
            UINT8 ConnectBos :1;
            UINT8 SMK        :1;
            UINT8 IPR        :1;
            UINT8 noteAUTO       :1;
            UINT8 noteALARM      :1;
            UINT8 Pwr1        :1;
            UINT8 Pwr2        :1;  
        };
        UINT8 ErrByte;     // интегральное значение состояния прибора
    }FlagErrBits;
    typedef union {
        struct{
                UINT8 SoundOff      :1;     // звук 0 включен 1 выключен
                UINT8 StatusDoor    :1;     // дверь 0 закрыта 1 открыта
                UINT8 LoopIPR       :1;     // ИПР 0 нет сработки 1 есть сработка
                UINT8 nOteAUTO      :1;     // оповещатель автоматика 0 выключен 1 включен
                UINT8 nOteALARM     :1;     // оповещатель тревога 0 выключен 1 включен
                UINT8 reserv        :3;

        };
        UINT8 ControlByte;
    }FlagControlBits;
    typedef union {
        struct {
            unsigned char CounterDelay10s        :5;
            unsigned char Reserv                 :1;
//            unsigned char Mode                   :2;
            unsigned char Manual                 :1;
            unsigned char StopStart              :1;
        };
        unsigned char StatusAutoByte;
    }statusAuto;
    typedef struct StatusBU_Type{
        TimeStruct      Time;             // 7 байт
        UINT8           Direct;             // 1 байт
        UINT32          SerialNumber;     // 4 байта
        UINT16          IdNet;            // 2 байта
        UINT16          QuantityBos;        // 2 байта
        UINT8           Situation;        // 1 байт
        FlagErrBits     FlagErrRoom;      // 1 байт
        statusAuto      StatusAuto;       // 1 байт
        FlagControlBits FlagControlDirect;  // 1 байт итого 20 байт
    }UnionStatusBU;
    typedef union{
        struct{
            UINT16  NewPage     :1;
            UINT16  NewSn       :1;
            UINT16  NewData     :1;
            UINT16  Select      :3;
            UINT16  HandUp      :1;
            UINT16  HandDoun    :1;
            UINT16  Play        :1;
            UINT16  Yes         :1;
            UINT16  No          :1;
            UINT16  End         :1;
            UINT16  UP          :1;
            UINT16  DOUN        :1;
            UINT16  WaitNewScreen1   :1;     // изм. 07.10.22 флаг ожидания переключения на новый экран
            UINT16  Debug      :1;
        };
        UINT16 LcdFlagsWord;
    }LcdFlagBits;
    extern UINT16          CommandDebug;
    extern UINT16 CounterWaitingNewScreen;  // 07.10.22
    extern UINT8 WaitingScreen;  // 07.10.22 ожидаемая страница
    typedef struct StatusBos_Type{
        unsigned long SerialNumber;
        UINT16  ShortAddres;
        union {
            struct{
                unsigned char StatusReservPwr:1; // состояние резервного источника питания вкл/выкл
                unsigned char StausPwr:1;        // уровень напряжения питания 0-норма, 1- низкий
                unsigned char StatusLoopRT0:2;   // состояние ШС ПИ или активатора
                unsigned char StatusLoopRT1:2;   //0-норма, 1- обрыв, 2- КЗ 
                unsigned char Activator:2;
            };
            unsigned char StatusByte;     // новое интегральное значение состояния прибора
        };
//        union {
//            struct{
//                //unsigned char BosLoop0:2;          //состояние ШС ПИ 0-норма, 1- внимание, 2- пожар, 3 - 130град
//                //unsigned char BosLoop1:2;
//                //unsigned char BosSituation:2;      //интегральное состояние обстановки 0-норма, 1-внимание, 2-пожар,3-пуск
//                //unsigned char BosStatusActivatora:1;// состояние активатора 0-не стрелял, 1-отстрелялся
//                //unsigned char BosCapacitorRedy:1;  // готовность накопительного конденсатора к выстрелу 0-разряжен, 1-заряжен
//            };
            unsigned char AlarmByte;//новое интегральное значение обстановки и этапа пуска
//        };
        signed int    TemperaturaSensor[2];
    }UnionBosStatus;
    typedef union{
        struct{
            UINT8 IntegralStatus;
            UINT8 IntegralSituation;
        };
        UINT16 Integral;
    }IntegralUnion;
    typedef struct{
        UINT16 CP_Sound_off     :1;
        UINT16 CurrentModeRs    :2;
        UINT16 NoBurLast        :1;     // изм. 08.04.22 флаг есть потеря связи с БУР или РКН
        UINT16 NoBurNew         :1;     // изм. 08.04.22 флаг есть потеря связи с БУР или РКН
        UINT16 RsBreakLast      :1;     // изм. 08.04.22 флаг если установлено кольцо есть обрыв кольца
        UINT16 RsBreakNew       :1;     // изм. 08.04.22 флаг если установлено кольцо есть обрыв кольца
        UINT16  ErrorRip        :1;     // изм. 13.04.22 флаг ошибки по входу рип
        UINT16 reserv           :8;
    }FildControlBits;
    typedef union{
        struct{
            UINT16  NewSituation:1;
            UINT16  NewStaus    :1;
            UINT16  NewAuto     :1;
            UINT16  NewOut      :1;
            UINT16  NewDataBos  :1;
            UINT16  RedyDataBos :1;
            UINT16  NewReserv   :10;
        };
        UINT16 NewEventLcdWord;
    }FildNewEventForLcd;
    extern FildNewEventForLcd NewEventLcdFlag;
    extern UnionBosStatus   StatusBos[513];
    extern UINT8            CurrentScreen;
    extern UINT8            PreviousScreen;     // экран с которого выполняется переход
    extern UINT8            SelectedDirection;  // переменная выбраного направления
    extern UINT32           AccessPassword;
    extern UINT32           OldAccessPassword;
    extern LcdFlagBits      LcdFlag;
    extern UnionStatusBU    StatusBU[11]; // структура описывающая состояние БУ
    extern UINT8           ClassAlgoritm[12];// вводим пераметр класс ИП или алгоритм СПС
    extern UINT32           RequestedSerial;// серийный номер к которому выполняется обращение
    extern UINT8            ConnectBUR[12]; // массив показывающий наличие связи с БУ
    extern UINT8            WaitData; // флаг ожидания данных по RS
    extern UINT8            CounterDirect;
    extern UINT32           MySerialNumber;
    extern UINT8            CounterCheckBU; // подробный пакет запрашивается с периодичностью определяемой этой переменной
    extern UINT16           IndexBos;// индекс последовательного перебора с.н. БОС
    extern FildControlBits   ControlFlagCP;
    extern UINT8            TempNumberKid;
    extern UINT8            TempDir;
    extern UINT32           TempSerialNumberBUR; // регистр временного хранения серийного номера удаляемого БУР
    extern UINT8            DirectControl;
    extern UINT16           CounterDelayStart[11];
//    extern UINT8            CounterDelayDoor[12];
    
    // изм. 24.03.22
    extern UINT8           Bright;
    extern UINT8           CounterLcdSleep;
    extern UINT16          CounterErrBos;
    extern UINT16          CounterAlarmBos;
    extern UINT16          CounterFireDevice;
    extern UINT16          CounterAttentionDevice;
    extern UINT16          CounterStartDevice;
    
// изм. 29.03.22 
extern UINT8 NumberKID;    
extern UINT16 CounterDelayMs;

// изм. 05.04.22
extern UINT8 NumberOfResponses[4];       // количество зарегистрированных приборов и принятых ответов на запросы по RS485
extern  UINT8 ErrPwr;
extern unsigned char ModeRs;

extern UINT8 TempSound;
extern UINT8 TempLedError;
extern UINT8 TempLedFire;
extern UINT8 TempLedStart;
extern UINT8 TempLedAuto;
#ifdef	__cplusplus
}
#endif

#endif	/* ALWAYSCP_H */

