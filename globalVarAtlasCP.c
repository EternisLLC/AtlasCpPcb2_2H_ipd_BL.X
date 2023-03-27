#include    "alwaysCP_210728.h"


UINT8           CurrentScreen = 255; // номер текущего экрани NEXTION
UINT8           PreviousScreen;     // экран с которого выполняется переход
UINT8           SelectedDirection = 0; /* переменная определяющее выбранное направление
                              * (если 0 не выбрано ни одно направление 
                              * 1...10 номер выбранного направления
                              *  255 номер выбранного направления не меняется)
                              */
LcdFlagBits     LcdFlag;    // структура определяющая смену экранов и данных на них
UINT16          CommandDebug = 0;
UINT32          AccessPassword   = 1234;
UINT32          OldAccessPassword;
UINT32          MySerialNumber;
UnionStatusBU       StatusBU[11]; // структура описывающая состояние БУ
UINT8           ClassAlgoritm[12]= {0,0,0,0,0,0,0,0,0,0,0,0};// вводим пераметр класс ИП или алгоритм СПС
UINT8           ConnectBUR[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // массив показывающий наличие связи с БУ
UINT32          RequestedSerial;// серийный номер к которому выполняется обращение
UINT8           WaitData; // флаг ожидания данных по RS
UINT8           CounterDirect = 1;
UINT8           CounterCheckBU; // подробный пакет запрашивается с периодичностью определяемой этой переменной
UINT16          IndexBos;// индекс последовательного перебора с.н. БОС
UINT32          TempSerialNumberBUR; // регистр временного хранения серийного номера удаляемого БУР
UINT16          CounterDelayStart[11]; // изм 28.07.21 переменная текущего значения времени задержки пуска
UINT8           CounterDelayDoor[12]; // изм 28.07.21 переменная текущего значения времени задержки пуска

// изм. 24.03.22
UINT8           Bright = 80;
UINT8           CounterLcdSleep = 30;
UINT16          CounterErrBos;
UINT16          CounterAlarmBos;
UINT16          CounterFireDevice;
UINT16          CounterAttentionDevice;
UINT16          CounterStartDevice;


// изм. 29.03.22 
UINT8 NumberKID;
UINT16 CounterDelayMs;

// изм. 05.04.22
UINT8 NumberOfResponses[4];       // количество зарегистрированных приборов и принятых ответов на запросы по RS485

// изм. 06.04.22
FildControlBits ControlFlagCP;
UINT8 ErrPwr;
unsigned char ModeRs; // изм. 13.04.22
UINT16 CounterWaitingNewScreen;  // 07.10.22
UINT8 WaitingScreen = 255;  // 07.10.22 ожидаемая страница

UINT8 TempSound;
UINT8 TempLedError;
UINT8 TempLedFire;
UINT8 TempLedStart;
UINT8 TempLedAuto;

//
FildNewEventForLcd NewEventLcdFlag;