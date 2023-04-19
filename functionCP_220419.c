#include    <xc.h>
#include    <stdio.h>
#include    <stdbool.h>
#include    "alwaysCP_210728.h"
#include    "interruptTimer24.h"
#include    "terminal.h"
#include    "driverAt45.h"
#include    "functionCP.h"
#include    "FunctionRS485_K.h"
#include "DriverMFRC522cp.h"
#include    "functionUART24.h"
#define MAX_QUANTITY_PAGE   2047
#define SIZE_PAGE           264
#define SIZE_EVENT          22
#define EVENT_ON_PAGE       12
#define MAX_QUANTITY_EVENT  24000
#define MAX_EVENT_PAGE      2000
#define ADDR_CURRENT_PAGE   22
#define ADDR_CURRENT_BYTE   24


fildFlagBitsPcb2 FildFlagsPcb2[12];   // изм. 25.03.22

fildFlagBitsPcb2 TempEventRead;
fildFlagBitsPcb2 TempEventWrite;

UINT16  ValueVdd[2] = {0,0};
UINT8   StatusPwr[2] = {0,0};

UINT16    CurrentEventPage = 0;   // текущая страница памяти для нового события
UINT16    CurrentEventByte = 0;   // текущий байт на странице памяти для нового события
UINT16  CurrentEventWrite;
void SaveCurrentPosition(void){
    MainMemoryPageToBuffer(2,2046);
    WriteToBufferAt45(2,ADDR_CURRENT_PAGE,2,(unsigned char*)&CurrentEventPage);
    WriteToBufferAt45(2,ADDR_CURRENT_BYTE,2,(unsigned char*)&CurrentEventByte);
    BufferToMainMemoryPageE(2,2046);
}
unsigned int ReadCurrentPositionAt45(void){
    // переносим страницу с установочными параметрами в буфер
    MainMemoryPageToBuffer(2,2046);
    // читаем номер страницы 
    ReadFromBufferAt45(2,ADDR_CURRENT_PAGE,2,(unsigned char*)&CurrentEventPage);
    // читаем номер последнего сохраненного байта
    ReadFromBufferAt45(2,ADDR_CURRENT_BYTE,2,(unsigned char*)&CurrentEventByte);
    if(CurrentEventPage >= MAX_EVENT_PAGE){
        CurrentEventPage = 0;
        CurrentEventByte = 0;
        WriteToBufferAt45(2,ADDR_CURRENT_PAGE,2,(unsigned char*)&CurrentEventPage);
        WriteToBufferAt45(2,ADDR_CURRENT_BYTE,2,(unsigned char*)&CurrentEventByte);
        BufferToMainMemoryPageE(2,2046);
    }
    // переписываем текущую страницу памяти в БУФЕР
    MainMemoryPageToBuffer(1,CurrentEventPage);
    // расситываем CurrentEventWrite
    return ((CurrentEventPage * EVENT_ON_PAGE) + (CurrentEventByte / SIZE_EVENT));
}
void SaveAccessPassword(unsigned long accessPassword){
    MainMemoryPageToBuffer(2,2047);
    WriteToBufferAt45(2,44,4,(unsigned char*)&accessPassword);
    BufferToMainMemoryPageE(2,2047);
}
void SaveSerialNumberBU(unsigned long tempSerial,UINT16 addr){
    UINT32 ReadSN;
    MainMemoryPageToBuffer(2,2047);
    WriteToBufferAt45(2,addr,4,(unsigned char*)&tempSerial);
    BufferToMainMemoryPageE(2,2047);
    ReadSN = ReadLongFromAT45(2047,addr);
    if(ReadSN != tempSerial){
        Nop();
    }
}
// изм. 07.04.22++++++++++++++
void SaveModeRs485(unsigned char mode485){
    MainMemoryPageToBuffer(2,2047);
    WriteToBufferAt45(2,250,1,&mode485);
    BufferToMainMemoryPageE(2,2047);
    ControlFlagCP.CurrentModeRs = ReadCharFromAt45(2047,250);//
}
//++++++++++++++++++++++++++++++++
// изм. 29.03.22#include    "iButton.h"
EventStruct BufferWriteEvent;
UINT8 tempDirect; // временная переменная в которой сохраняется значение направления
void SaveEvent(UINT8 TypeEvent) {
    unsigned char *pcc; // указатель на чар
    unsigned int*   pii; // указатель на uint
    if(!TypeEvent) return; // нет нового события запись не требуется
    // сохраняем время события в буфере записи 7 байт
    BufferWriteEvent.Year = CurrentTime.Year;     //0,1 байт
    BufferWriteEvent.Month = CurrentTime.Month;   //2 байт
    BufferWriteEvent.Day = CurrentTime.Day;       //3 байт
    BufferWriteEvent.Hour = CurrentTime.Hour;     //4 байт
    BufferWriteEvent.Minute = CurrentTime.Minute; //5 байт
    BufferWriteEvent.Second = CurrentTime.Second; //6 байт
    // сохраняем данные о событии в буфере записи
    switch(TypeEvent){
        case 100:case 101:// событие включение питания КП
            BufferWriteEvent.SerialNumber = 0;    //8...11 байт
            BufferWriteEvent.IdNet = 0;                  //12,13 байт
            BufferWriteEvent.QuantityBos = 0; // 14,15 байт
            BufferWriteEvent.Direct = 0;                    //7 байт
            BufferWriteEvent.Situation = 0;          //16 байт
            BufferWriteEvent.ErrByte = 0;  //17 байт
            BufferWriteEvent.StatusAutoByte = 0;   //18 байт
            BufferWriteEvent.ControlByte = 0;  //19 байт
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 102:   // событие добавлен новый БУ в память КП
            BufferWriteEvent.Direct = SelectedDirection;                    //7 байт
            BufferWriteEvent.SerialNumber = StatusBU[SelectedDirection].SerialNumber;    //8...11 байт
            BufferWriteEvent.IdNet = 0;                  //12,13 байт
            BufferWriteEvent.QuantityBos = 0; // 14,15 байт
            BufferWriteEvent.Situation = 0;          //16 байт
            BufferWriteEvent.ErrByte = 0;  //17 байт
            BufferWriteEvent.StatusAutoByte = 0;   //18 байт
            BufferWriteEvent.ControlByte = 0;  //19 байт
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 103: // событие удален БУ из памяти КП
            BufferWriteEvent.Direct = SelectedDirection;                    //7 байт
            BufferWriteEvent.SerialNumber = TempSerialNumberBUR;//StatusBU[SelectedDirection].SerialNumber;    //8...11 байт
            BufferWriteEvent.IdNet = 0;                  //12,13 байт
            BufferWriteEvent.QuantityBos = 0; // 14,15 байт
            BufferWriteEvent.Situation = 0;          //16 байт
            BufferWriteEvent.ErrByte = 0;  //17 байт
            BufferWriteEvent.StatusAutoByte = 0;   //18 байт
            BufferWriteEvent.ControlByte = 0;  //19 байт
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 104: //  событие изменилось напряжение питания КП
            BufferWriteEvent.SerialNumber = 0;    //8...11 байт
            BufferWriteEvent.IdNet = ValueVdd[0];                  //12,13 байт
            BufferWriteEvent.QuantityBos = ValueVdd[1]; // 14,15 байт
            BufferWriteEvent.Direct = 0;                    //7 байт
            BufferWriteEvent.Situation = 0;          //16 байт
            BufferWriteEvent.ErrByte = 0;  //17 байт
            BufferWriteEvent.StatusAutoByte = 0;   //18 байт
            BufferWriteEvent.ControlByte = 0;  //19 байт
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 105:    //событие смена пароля
            BufferWriteEvent.SerialNumber = AccessPassword; // новый пароль
            pii = (unsigned int*)&OldAccessPassword;        
            BufferWriteEvent.IdNet = *pii++;                // старый пароль
            BufferWriteEvent.QuantityBos = *pii;
            break;
        case 106: // событие записан новый КИД
            BufferWriteEvent.reserv = TempNumberKid;
            pcc = (unsigned char*)&BufferWriteEvent.SerialNumber;// выставляем указатель на начало буфера записи
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[0];
            pcc ++;
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[1];
            pcc ++;
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[2];
            pcc ++;
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[3];
            break;
        case 1: // установлена связь с БУ
            BufferWriteEvent.Direct = StatusBU[0].Direct;                    //7 байт
            BufferWriteEvent.SerialNumber = StatusBU[0].SerialNumber;    //8...11 байт
            BufferWriteEvent.IdNet = 0;                  //12,13 байт
            BufferWriteEvent.QuantityBos = StatusBU[0].QuantityBos; // 14,15 байт
            BufferWriteEvent.Situation = 0;          //16 байт
            BufferWriteEvent.ErrByte = 0;  //17 байт
            BufferWriteEvent.StatusAutoByte = 0;   //18 байт
            BufferWriteEvent.ControlByte = 0;  //19 байт
// добавляем информацию о типе СПС/АУТП
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 2: // потеряна связь с БУ
            BufferWriteEvent.Direct = TempDir;// былоk;                    //7 байт
            BufferWriteEvent.SerialNumber = StatusBU[TempDir/* было k*/].SerialNumber;    //8...11 байт
            BufferWriteEvent.IdNet = 0;                  //12,13 байт
            BufferWriteEvent.QuantityBos = 0; // 14,15 байт
            BufferWriteEvent.Situation = 0;          //16 байт
            BufferWriteEvent.ErrByte = 0;  //17 байт
            BufferWriteEvent.StatusAutoByte = 0;   //18 байт
            BufferWriteEvent.ControlByte = 0;  //19 байт
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 3:case 4:case 5:// событие об изменениях подключенных БУ
            BufferWriteEvent.Direct = StatusBU[0].Direct;                    //7 байт
            BufferWriteEvent.SerialNumber = StatusBU[0].SerialNumber;    //8...11 байт
            BufferWriteEvent.IdNet = StatusBU[0].IdNet;                  //12,13 байт
            BufferWriteEvent.QuantityBos = StatusBU[StatusBU[0].Direct].QuantityBos; // 14,15 байт
            BufferWriteEvent.Situation = StatusBU[0].Situation;          //16 байт
            BufferWriteEvent.ErrByte = StatusBU[0].FlagErrRoom.ErrByte;  //17 байт
            BufferWriteEvent.StatusAutoByte =StatusBU[0].StatusAuto.StatusAutoByte;   //18 байт
            BufferWriteEvent.ControlByte = StatusBU[0].FlagControlDirect.ControlByte;  //19 байт
// изм. 28.03.22    // добавляем информацию о типе СПС/АУТП                
            BufferWriteEvent.reserv = FildFlagsPcb2[0].ByteFlagRoom_Pcb2; 
            
 //====================
            break;
        case 6:             // передача команды "СБРОС"
        case 7: case 8:     // передача команд "ПУСК"
        case 14: case 15:   // событие нажата кнопка управленияавтоматикой на КП
            // сохраняем направление
            BufferWriteEvent.Direct = StatusBU[TempDirectRs].Direct;
            // сохраняем номер приложенного ключа
            BufferWriteEvent.reserv = TempNumberKid;
            break;
    }
    BufferWriteEvent.TypeEvent  = TypeEvent;     // 20
    T3CONbits.TON = 0; // изм. 18.08.20 остановка таймера на время записи события
    // записываем новое событие в соответствующую позицию в буфере AT45
    WriteToBufferAt45(1,CurrentEventByte,SIZE_EVENT,(unsigned char *)&BufferWriteEvent);
//==============
    BufferToMainMemoryPageE(1,CurrentEventPage);// буфер закончился записываем его в страницу основной памяти
//================
    // изменяем номер позиции  в буфере AT45
    CurrentEventByte += SIZE_EVENT;
    if(CurrentEventByte >= SIZE_PAGE){
        CurrentEventByte = 0;
        BufferToMainMemoryPageE(1,CurrentEventPage);// буфер закончился записываем его в страницу основной памяти
        CurrentEventPage ++;// изменяем номер страницы памяти в которую будет производиться запись
        if(CurrentEventPage >= MAX_EVENT_PAGE){
            CurrentEventPage = 0;
        }
        SaveCurrentPosition();//========= сохранение номера стр и номера байта в памяти изм. 191003
        MainMemoryPageToBuffer(1,CurrentEventPage);// читаем новую страницу в буфер
    }
//=============
    SaveCurrentPosition();//========= сохранение номера стр и номера байта в памяти изм. 191003
//==============
    T3CONbits.TON = 1; // изм. 18.08.20 включение таймера после записи события
    CurrentEventWrite = (CurrentEventPage * 12) + (CurrentEventByte / 22); // расчет номера текущего события для записи
    return;   // новое событие надо сохранить в памяти
}
// функция формирования пакетов управления для устройств в том же направлении
// в TempCommandRs и TempDirectRs формируются значения команд и направлений
void CommandRs( UINT8 Cmnd,UINT8 dir){
    TempCommandRs = Cmnd;
    TempDirectRs = dir;
}
// ПЕРЕМЕННЫЕ И ФУНКЦИИ КОНТРОЯ ПОДКЛЮЧЕННЫХ БУ
statusAuto IndicationStatusAuto[11];
void SetStatusIndictionDoor(UINT8 status,UINT8 dir){   // проверка состояния двери для идикации на дисплее
    UINT8 j;
    for(j = 1; j < 11; j++){
        if(StatusBU[j].Direct == StatusBU[dir].Direct){
            IndicationStatusAuto[j].Manual = status;
        }
    }
}
UINT8 CheckStatusDoor(UINT8 dir){
    UINT8 j;
    for(j = 1; j < 11; j++){
        if(StatusBU[j].Direct == StatusBU[dir].Direct){
            if((StatusBU[j].StatusAuto.StatusAutoByte & 0b11000000) == MANUAL){
                /*отладка*/if(LcdFlag.Debug)xprintf("dir %u hands\r",dir);
                return 1;
            }
        }
    }
    /*отладка*/if(LcdFlag.Debug)xprintf("dir %u auto\r", dir);
    return 0;
}
//UINT16 NewEvent[11];
void CopyUnuonStatusBu(UINT8 dir){
    StatusBU[dir].Time.Year = StatusBU[0].Time.Year;
    StatusBU[dir].Time.Month = StatusBU[0].Time.Month;
    StatusBU[dir].Time.Day = StatusBU[0].Time.Day;
    StatusBU[dir].Time.Hour = StatusBU[0].Time.Hour;
    StatusBU[dir].Time.Minute = StatusBU[0].Time.Minute;
    StatusBU[dir].Time.Second = StatusBU[0].Time.Second;
    StatusBU[dir].Direct = StatusBU[0].Direct;
    StatusBU[dir].SerialNumber = StatusBU[0].SerialNumber;
    StatusBU[dir].IdNet = StatusBU[0].IdNet;
    if(StatusBU[dir].Situation != StatusBU[0].Situation){
        StatusBU[dir].Situation = StatusBU[0].Situation;
        // событие изменилась обстановка
        SaveEvent(3);
        Interval._1min = 1;
    }
    if(StatusBU[dir].FlagErrRoom.ErrByte != StatusBU[0].FlagErrRoom.ErrByte){
        StatusBU[dir].FlagErrRoom.ErrByte = StatusBU[0].FlagErrRoom.ErrByte;
        // событие изменилился статус приборов в направлении
        SaveEvent(4);
        Interval._1min = 1;
    }
// изм. 25.03.22    
    if(StatusBU[dir].SerialNumber > 2111000){
        if(FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 != FildFlagsPcb2[0].ByteFlagRoom_Pcb2){
            FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 = FildFlagsPcb2[0].ByteFlagRoom_Pcb2;
            // событие изменились флаги приборов в pcb2
// здесь вставить запись события           
            SaveEvent(4);
            Interval._1min = 1;
        }
    }else{
        FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 = 0;
    }
//============    
// изм. 31.08.22 
    if(StatusBU[dir].StatusAuto.StatusAutoByte != StatusBU[0].StatusAuto.StatusAutoByte){
        UINT8 resultCheck;
        UINT8 CurrentModeAuto = StatusBU[dir].StatusAuto.StatusAutoByte & 0b11000000; // сохраняем текущее значение режима автоматики
        UINT8 NewModeAuto = StatusBU[0].StatusAuto.StatusAutoByte & 0b11000000; //сохраняем новое значение режима автоматики
        
        /*отладка*/if(LcdFlag.Debug)xprintf("dir %u current %x -> new %x\r",dir,CurrentModeAuto,NewModeAuto);
//        if(CurrentModeAuto != NewModeAuto){
            
        switch(NewModeAuto){ // переключатель в зависимости от нового режима работы
            case AUTO:
// выставляем флаги в соответствии с принятыми значениями режима работы  
                if(CurrentModeAuto == LOCK){ // если текущий режим блокировка
                    SetStatusIndictionDoor(0,dir);
                    /*отладка*/if(LcdFlag.Debug)xprintf("Com 4 cpopy\r");
                    CommandRs(4,dir);
                }else{ // если текущий режим ручной
                // Здесь включаем проверку наличия БУР с открытыми дверями 
                    /*отладка*/if(LcdFlag.Debug)xprintf("Com 9 copy\r");
                    CommandRs(9,dir);
                    SetStatusIndictionDoor(0,dir);
                    resultCheck = CheckStatusDoor(dir);
                    if(!resultCheck){
                       
                    }
                }
                break;
            case LOCK:
// выставляем флаги в соответствии с принятыми значениями режима работы                  
                SetStatusIndictionDoor(1,dir);
                /*отладка*/if(LcdFlag.Debug)xprintf("Com 5 copy\r");
                CommandRs(5,dir);
                break;
            case MANUAL:
                SetStatusIndictionDoor(1,dir);
                /*отладка*/if(LcdFlag.Debug)xprintf("Com 8 copy\r");
                CommandRs(8,dir);
                break;
        }
        // событие изменился режим работы автоматики
        SaveEvent(5);
    }
    
//++++++++++++++    
    StatusBU[dir].StatusAuto.StatusAutoByte = StatusBU[0].StatusAuto.StatusAutoByte;
    StatusBU[dir].FlagControlDirect.ControlByte = StatusBU[0].FlagControlDirect.ControlByte;
    CounterDelayStart[dir] = CounterDelayStart[0]; // изм. 28.07.21  сохраняем текущее значение времени задержки пуска
// изм. 29.08.22    CounterDelayDoor[dir] = CounterDelayDoor[0]; // изм. 28.07.21  сохраняем текущее значение времени задержки пуска
}
EventStruct BufferReadEvent;
void ReadEvent(UINT16 NumberEv){
    UINT16 pageNumber;
    UINT16 byteNumber;
    // по номеру события рассчитываем номер страницы памяти и номер байта на странице
    pageNumber = NumberEv / 12;
    byteNumber = (NumberEv % 12) * 22;
    MainMemoryArrayRead(pageNumber,byteNumber,22,(unsigned char*)&BufferReadEvent);
}
#define TIMING_BOS  1
static UINT8    StageCheckBU;// этап на котором в данный момент находится проверка
UnionBosStatus StatusBos[513];
void ClrStatusBos(void){
    UINT16 n;
    for(n = 1;n < 512;n ++){
        StatusBos[n].SerialNumber = 0;
        StatusBos[n].ShortAddres = 0;
        StatusBos[n].AlarmByte = 0;
        StatusBos[n].StatusByte = 0xFF;
        StatusBos[n].TemperaturaSensor[0] = 0;
        StatusBos[n].TemperaturaSensor[1] = 0;
    }
}
UINT8   CounterNoConnect[11];
UINT8   TempDir;
UINT8   CheckStatusBU201106(UINT8 direct){
    if(CounterDelayRs){
        return 0; // если мы ждем какие либо данные выходим
    }
    if(StatusBU[direct].SerialNumber){ // если БУ зарегистрирован
        if(ConnectBUR[direct]){ // если с БУ установлена связь
            if(direct != SelectedDirection || !Interval._1min){   // изм. 19.11.19 если запрашивается обобщенный пакет
                switch (StageCheckBU){
                    default:
                        StageCheckBU = 0;
                        break;
                    case 0:
                        RequestedSerial = StatusBU[direct].SerialNumber;
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                        RsTxLength = sprintf((char*)RsTxBuffer,"? %lu 2 ",StatusBU[direct].SerialNumber);
                        RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // вычисляем контрольную сумму 
                        RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                        TransmittRsPacket();
                        TempDirectRs = direct; // 30/09/22сохраняем номер направления
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                        CounterDelayRs = 50; // изм. 30.01.20 CounterDelayRs перенесн из 1 мс таймера в 10мс
                        WaitData = 0;   // сбрасывем флаг ответа
                        StageCheckBU ++;
                        break;
                    case 1:
                        TerminalRs();   // ожидаем ответа 1000 мс
                        if(!CounterDelayRs) {
                            StageCheckBU ++;
                        }
                        break;
                    case 2:
                        if(WaitData){
                            ConnectBUR[direct] = 1;
                            CounterNoConnect[direct] = MaxNumberRequests; // изм. 06.04.22;
                            CopyUnuonStatusBu(direct);/* если ответ получен
* сохраняем данные в регистры статуса соответсвующего направления */
                        }else{
                            if(CounterNoConnect[direct]){
                                CounterNoConnect[direct] --;
                            }else{
                                 ConnectBUR[direct] = 0;
                                // событие потеря связи с БУ
                                TempDir = direct;
                                SaveEvent(2); 
                                // здесь добавляем сброс БУР
                                CommandRs(128,direct);
                            }
                        }
                        StageCheckBU = 0;
                        Interval._CheckStatusBU = 0; // изменяем номер запрашиваемого БУ
                        break;
                }
                
                
            }else{      // если запрашивается подробный пакет
                switch (StageCheckBU){
                    default:
                        StageCheckBU = 0;
                        break;
                    case 0:
                        ClrStatusBos();
                        RequestedSerial = StatusBU[direct].SerialNumber;
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
                        RsTxLength = sprintf((char*)RsTxBuffer,"? %lu 3 ",StatusBU[direct].SerialNumber);
                        RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // вычисляем контрольную сумму 
                        RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                        TransmittRsPacket();
                        TempDirectRs = direct; // 30/09/22сохраняем номер направления
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                        CounterDelayRs = 50; // изм. 30.01.20 CounterDelayRs перенесн из 1 мс таймера в 10мс
                        WaitData = 0;   // сбрасывем флаг ответа
                        CounterErrBos = 0;
                        CounterAlarmBos = 0;
                        CounterFireDevice = 0;
                        CounterAttentionDevice = 0;
                        CounterStartDevice = 0;
                        StageCheckBU ++;
                        StatusBos[0].AlarmByte = 0; // ждем новые данные о БОС
                        NewEventLcdFlag.NewDataBos = 0; // снимаем флаг информация о БОС получена
                        break;
                    case 1:
                           // ожидаем ответа 100 мс
                        if(CounterDelayRs == 0) {
                            StageCheckBU ++;
                        }
                        break;
                    case 2:
                         /* флаг установленной связи приравниваем флагу 
                                                * ответа если ответ получе будет 1*/
                        if(WaitData){      /* если ответ получен
    * сохраняем данные в регистры статуса соответсвующего направления 
    * и рассчитываем время ожидания для приема информации о подключенных БОС*/
                            CopyUnuonStatusBu(direct);
                            CounterDelayRs = (StatusBU[direct].QuantityBos * TIMING_BOS) + 1;  /* */
                            StageCheckBU ++;
                        }else{
                            if(CounterNoConnect[direct]){
                                CounterNoConnect[direct] --;
                            }else{
                                ConnectBUR[direct] = 0;
                                // событие потеря связи с БУ
                                TempDir = direct;
                                SaveEvent(2); 
                                CommandRs(128,direct);
                            }
                            StageCheckBU = 0;
                            Interval._CheckStatusBU = 0; // изменяем номер запрашиваемого БУ
                        }
                        break;
                    case 3:
                        TempDir = direct;
                        TerminalRs();   // ожидаем информации о БОС
                        if(!CounterDelayRs ) {  // время ожидания закончилось
                            ConnectBUR[direct] = 1;
                            StageCheckBU = 0;
                            Interval._CheckStatusBU = 0; // изменяем номер запрашиваемого БУ
                            Interval._1min = 0;
                            NewEventLcdFlag.NewDataBos = 1; // выставляем флаг информация о БОС получена
                        }
                        break;
                }
            }
        }else{  // если с БУ связь не установлена
            switch (StageCheckBU){
                default:
                    StageCheckBU = 0;
                    break;
                case 0:
                    RequestedSerial = StatusBU[direct].SerialNumber;
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                    RsTxLength = sprintf((char*)RsTxBuffer,"? %lu 1 ",StatusBU[direct].SerialNumber);
                    RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // вычисляем контрольную сумму 
                    RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                    TransmittRsPacket();
//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
                    while(ENTX485 || !ENTX485_I);
                    CounterDelayRs = 50; // изм. 30.01.20 CounterDelayRs перенесн из 1 мс таймера в 10мс
                    WaitData = 0;   // сбрасывем флаг ответа
                    StageCheckBU ++;
                    break;
                case 1:
                       // ожидаем ответа 1000 мс
                    if(CounterDelayRs == 0) {
                        StageCheckBU ++;
                    }
                    break;
                case 2:
                     /* флаг установленной связи приравниваем флагу 
                                            * ответа если ответ получе будет 1*/
                    if(WaitData){      /* если ответ получен
* сохраняем данные в регистры статуса соответсвующего направления 
* и рассчитываем время ожидания для приема информации о подключенных БОС*/
                        StatusBU[direct].SerialNumber = StatusBU[0].SerialNumber;
                        StatusBU[direct].QuantityBos = StatusBU[0].QuantityBos;
                        // событие установлена связь с БУ
//                        NewEvent[direct] = 1;
                        SaveEvent(1);
                        StageCheckBU ++;
                        CounterDelayRs = (StatusBU[0].QuantityBos * TIMING_BOS) + 1;  /* */
                    }else{
                        ConnectBUR[direct] = 0;
                        // если ответ не получен
                        StageCheckBU = 0;
                        Interval._CheckStatusBU = 0; // изменяем номер запрашиваемого БУ
                    }
                    break;
                case 3:
                    TempDir = direct;
                    TerminalRs();   // ожидаем информации о БОС
                    if(!CounterDelayRs ) {  // время ожидания закончилось
                        ConnectBUR[direct] = 1;
                        CounterNoConnect[direct] = MaxNumberRequests; // изм. 06.04.22;
                        StageCheckBU = 0;
                        Interval._CheckStatusBU = 0; // изменяем номер запрашиваемого БУ
                    }
                    break;
            }
        }
    }else{
        Interval._CheckStatusBU = 0; // изменяем номер запрашиваемого БУ
    }
    if(!Interval._CheckStatusBU){
        CounterCheckBU ++;
        if(CounterCheckBU > 5){// изм. 19.11.19
            CounterCheckBU = 0;
        }
        return 1; // все БУ опрошены можно выводить информацию
    }
    return 0;
}
UINT8 kk;

void SetButtonColor(UINT8 page, UINT8 dir, UINT16 color) {
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.bco=%uЪЪЪ", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    
}
void SetButtonFont(UINT8 page, UINT8 dir, UINT16 color, UINT16 font) {
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.font=%uЪЪЪ", page, dir, font);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.pco=%uЪЪЪ", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
void SetTextColor(UINT8 page, UINT8 dir, UINT16 color) {// изм 22.11.2019
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.bco=%uЪЪЪ", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    
}
#define NO_REG_         0        //up   67 doun 66
#define NO_CONNECT_     1       //      71      72
#define NORMA_          2       //      75      74
#define ERROR_          3       //      79      78
#define ALARM_          4       //      83      82
void SetKeyPic (UINT8 page, UINT8 dir, UINT8 status){
    UINT8 picture;
    // выбираем картинку для печати
    switch(status){
        case NO_REG_:
            if(dir<6){
                picture = 67;
            }else{
                picture = 66;
            }
            break;
        case NO_CONNECT_:
            if(dir<6){
                picture = 71;
            }else{
                picture = 70;
            }
            break;
        case NORMA_:
            if(dir<6){
                picture = 75;
            }else{
                picture = 74;
            }
            break;
        case ERROR_:
            if(dir<6){
                picture = 79;
            }else{
                picture = 78;
            }
            break;
        case ALARM_:
            if(dir<6){
                picture = 83;
            }else{
                picture = 82;
            }
            break;
    }
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.pic=%uЪЪЪ", page, dir, picture);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
void SetTextPic(UINT8 page, UINT8 dir, UINT16 picture) {// изм 22.05.2020
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.pic=%uЪЪЪ", page, dir, picture);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
void SetTextFont(UINT8 page, UINT8 dir, UINT16 color, UINT16 font) {
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.font=%uЪЪЪ", page, dir, font);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.pco=%uЪЪЪ", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
#define MAIN_MENU 10

void  SetAutoDispley(UINT8 dir, UINT8 picture){
    if(dir > 5){
        SetTextPic(MAIN_MENU, dir, picture);
    }else{
        SetTextPic(MAIN_MENU, dir, picture + 1);
    }
    SetTextFont(MAIN_MENU, dir, BLACK, 0);
}

void PrintDirectionPage10(UINT8 dir){
    if(!StatusBU[dir].SerialNumber /*|| !NewEvent[dir]*/){// 05.10.22
        return;
    }
    if (ConnectBUR[dir]){
        // выводим на главный экран состояние автоматики
// изм. 01.09.22
        if(IndicationStatusAuto[dir].StatusAutoByte & 0b11000000){
            SetAutoDispley(dir,80);
        }else{
            SetAutoDispley(dir,76);
        }
        if(StatusBU[dir].Direct && ((StatusBU[dir].Situation & 0x03) != 2)){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page10.b%u.txt=\"%u\"ЪЪЪ", dir, StatusBU[dir].Direct); // печатаем номер помещения // изм. 24.03.22
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }
        switch (StatusBU[dir].Situation & 0x03) { // изм. 191115
            case 0: // ситуация норма
                if (StatusBU[dir].FlagErrRoom.ErrByte /* изм. 25.03.22*/ || (FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 & 0x07)) { // изм. 191115
                    SetKeyPic (MAIN_MENU, dir, ERROR_);  // изм.200522
                    SetButtonFont(MAIN_MENU, dir, BLACK, 4);
                } else {
                    SetKeyPic (MAIN_MENU, dir, NORMA_);  // изм.200521
                    SetButtonFont(MAIN_MENU, dir, BLACK, 4);
                }
                break;
            case 1: // ситуация внимание
                SetKeyPic (MAIN_MENU, dir, ALARM_);  // изм.200522
                SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                break;
            case 2: // ситуация пожар
                SetKeyPic (MAIN_MENU, dir, ALARM_);  // изм.200522
                // изм. 02.08.21 было SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                SetButtonFont(MAIN_MENU, dir, BLACK, 3);
                UINT16 tempCounterDelay = CounterDelayStart[dir];/* изм. 29.08.22 + CounterDelayDoor[dir];*/
                if(ClassAlgoritm[dir] < 11){
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.b%u.txt=\"%u\"ЪЪЪ", dir, tempCounterDelay); // печатаем время задержки пуска
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                }else{
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.b%u.txt=\"%u\"ЪЪЪ", dir, StatusBU[dir].Direct); // печатаем номер помещения 
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                // конец изм. 02.08.21
                break;
            case 3: // ситуация пуск
                SetKeyPic (MAIN_MENU, dir, ALARM_);  // изм.200522
                SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                break;
        }
    }else{
        SetAutoDispley(dir,72);
        SetKeyPic (MAIN_MENU, dir, NO_CONNECT_);  // изм.200521
        SetButtonFont(MAIN_MENU, dir, BLACK, 4);
    }
}
char lcdline[100];
// изм. 08.04.22+++++++++++++++++++++
void IndicationStatusRs(void){
    if(CurrentScreen != 10)return;
    if(LcdFlag.NewPage){
        if(ControlFlagCP.CurrentModeRs == 1){
            ControlFlagCP.RsBreakNew = 0;   // 16.09.22
            ControlFlagCP.RsBreakLast = 0;  // 16.09.22
            if(!ControlFlagCP.NoBurNew){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=96ЪЪЪ");// исправная линия
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=97ЪЪЪ");// неисправная линия
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }else{
            if(!ControlFlagCP.RsBreakNew){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=94ЪЪЪ");// исправное кольцо
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=95ЪЪЪ");// неисправное кольцо
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }
        return;
    }
    if(ControlFlagCP.CurrentModeRs == 1){
        ControlFlagCP.RsBreakNew = 0;   // 16.09.22
        ControlFlagCP.RsBreakLast = 0;  // 16.09.22
        if(ControlFlagCP.NoBurLast != ControlFlagCP.NoBurNew){
            ControlFlagCP.NoBurLast = ControlFlagCP.NoBurNew;
            if(!ControlFlagCP.NoBurLast){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=96ЪЪЪ");// исправная линия
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=97ЪЪЪ");// неисправная линия
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }
        return;
    }
    if(ControlFlagCP.RsBreakLast != ControlFlagCP.RsBreakNew){
        ControlFlagCP.RsBreakLast = ControlFlagCP.RsBreakNew;
        if(!ControlFlagCP.RsBreakLast){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"t17.pic=94ЪЪЪ");// исправное кольцо
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }else{
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"t17.pic=95ЪЪЪ");// неисправное кольцо
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }
    }
}
//+++++++++++++++++++++
void PrintDirectionNumber(UINT8 screen){
    while(TxRunRs || TxRunLcd);
    switch(screen){
        case 7: case 11: case 12: case 16: case 17:
            sprintf(LcdBufferData,"page%u.t4.txt=\"%u\"ЪЪЪ",screen,SelectedDirection);
//            printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s\r",LcdBufferData);
            break;
        case 14 :case 18:case 19:
            sprintf(LcdBufferData,"page%u.t4.txt=\"%u\"ЪЪЪ",screen,StatusBU[SelectedDirection].Direct);
//            printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s\r",LcdBufferData);
            break;
        default:
            return;
    }
     printf("%s",LcdBufferData); /*отладка*/if(LcdFlag.Debug)xprintf("%s\r",LcdBufferData);
}
UINT8   GroupNumber;
UINT16  DeviceNumber;
// выводим на экран информацию о направлении
void IndicatorDirection(UINT8 dir){
    UINT8 nn;
    switch(CurrentScreen){
        case MAIN_MENU:
// обновление всего экрана при первом выходе на него
            if(LcdFlag.NewPage){
                for(nn = 1;nn < 11;nn ++){
                    PrintDirectionPage10(nn);
                }
                while(TxRunRs || TxRunLcd);
                if(ControlFlagCP.CP_Sound_off){ //изм. 04.04.22
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=56ЪЪЪ");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }else{
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=57ЪЪЪ");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                if(ControlFlagCP.ErrorRip){     // 30.09.22
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t13.pic=56ЪЪЪ");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }else{
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t13.pic=57ЪЪЪ");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                printf("%s",LcdBufferData);
                IndicationStatusRs();   // изм. 08.04.22
                LcdFlag.NewPage = 0;
            }
            IndicationStatusRs();       // изм. 08.04.22
            PrintDirectionPage10(dir);
            break;
        case 11:
//..................................            
            if(LcdFlag.NewPage || LcdFlag.NewData || NewEventLcdFlag.NewStaus || NewEventLcdFlag.NewSituation){
// ВЫВОДИМ ИНФОВМАЦИЮ О СОСТОЯНИИ БУР                
// устанавливаем подложку бура
                if(LcdFlag.Debug)xprintf("LcdFlag ox%04X, NewEventLcd ox%04X\r",LcdFlag.LcdFlagsWord,NewEventLcdFlag.NewEventLcdWord);
                PrintDirectionNumber(CurrentScreen);
                while(TxRunRs || TxRunLcd);
                if(StatusBU[SelectedDirection].Situation & 0x03){// в направлении есть тревожное состояние 
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page11.t1.pic=35ЪЪЪ"); // подложка красная
                }else{
                    if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* изм. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page11.t1.pic=34ЪЪЪ"); // подложка желтая
                    }else{
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page11.t1.pic=33ЪЪЪ"); // подложка зеленая
                    }
                }
                printf("%s",LcdBufferData);
// выводим обстановку и серийный номер БУРа
                while(TxRunRs || TxRunLcd);
                switch(StatusBU[SelectedDirection].Situation & 0x03){
                    case 0:
                        if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* изм. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
                            while(TxRunRs || TxRunLcd);
                            sprintf(lcdline,"%s","НЕИСПРАВНОСТЬ");
                        }else{
                            sprintf(lcdline,"%s","НОРМА");
                        }
                        break;
                    case 1:
                        sprintf(lcdline,"%s","ВНИМАНИЕ");
                        break;
                    case 2:
                        sprintf(lcdline,"%s","ПОЖАР");
                        break;
                    case 3:
                        sprintf(lcdline,"%s","ПУСК");
                        break;
                }
                sprintf(LcdBufferData,"page11.t1.txt=\"АУПТ\\r%s\\r%s С.Н. %lu\\rзарегистрировано\\rБОС:  %u шт.\"ЪЪЪ"
                        ,lcdline,((StatusBU[SelectedDirection].SerialNumber%1000) < 501)? "БУР":"РКН",
                        StatusBU[SelectedDirection].SerialNumber,StatusBU[SelectedDirection].QuantityBos);
                printf("%s",LcdBufferData);
// ВЫВОДИМ ИНФОРМАЦИЮ О СОСТОЯНИИ ИПД
                if(StatusBU[SelectedDirection].QuantityBos){
                    while(TxRunRs || TxRunLcd);
                    if(StatusBos[0].AlarmByte){
                        // в направлении есть тревожное состояние 
                        sprintf(LcdBufferData,"page11.t2.pic=35ЪЪЪ"); // подложка красная
                    }else{
                        if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0x03) ){
                            sprintf(LcdBufferData,"page11.t2.pic=34ЪЪЪ"); // подложка желтая
                        }else{
                            sprintf(LcdBufferData,"page11.t2.pic=33ЪЪЪ"); // подложка зеленая
                        }
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// выводим количество БОС во внимании, пожаре и неисправности
                    while(TxRunRs || TxRunLcd);
//                    sprintf(LcdBufferData,"page11.t2.txt=\"БОС подробно:\\rв пожаре - %u\\rво внимании - %u\\rв неисправности - %u\"ЪЪЪ"
//                        ,CounterFireDevice,CounterAttentionDevice,CounterErrBos);
                    sprintf(LcdBufferData,"page11.t2.txt=\"БОС подробно:\\rв пуске - %u\\rв пожаре - %u\\rво внимании - %u\\rв неисправности - %u\"ЪЪЪ"
                        ,CounterStartDevice,CounterFireDevice,CounterAttentionDevice,CounterErrBos);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);

                }
//***************
//  t0 выводим только информацию об автоматике
// изм. 01.09.22                    
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                        case    AUTO:
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.pic=28ЪЪЪ"); // подложка зеленая
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.txt=\"АВТОМАТИКА ВКЛЮЧЕНА\"ЪЪЪ");
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            break;
                        case    LOCK:
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.pic=31ЪЪЪ"); // подложка желтая
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.txt=\"ПУСК ЗАБЛОКИРОВАН\"ЪЪЪ");
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            break;
                        case    MANUAL:
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.pic=31ЪЪЪt0.txt=\"РЕЖИМ РАБОТЫ РУЧНОЙ\"ЪЪЪ");
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            break;
                    }
//***************
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                if(LcdFlag.NewData){
                    LcdFlag.NewData = 0;
                }
                if(NewEventLcdFlag.NewSituation){
                    NewEventLcdFlag.NewSituation = 0;
                }
                if(NewEventLcdFlag.NewStaus){
                    NewEventLcdFlag.NewStaus = 0;
                }
//.................                
            }
            break;
        case 12:
            if(LcdFlag.NewPage){
                PrintDirectionNumber(CurrentScreen);
                LcdFlag.NewPage = 0;
            }
            if(StatusBU[SelectedDirection].SerialNumber /*&& NewEvent[SelectedDirection]*/){
// в направлении есть зарегистрированный БУ
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page12.t2.txt=\"%s\"ЪЪЪ",((StatusBU[SelectedDirection].SerialNumber%1000) < 501)?"БУР":"РКН");
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page12.t1.txt=\"%lu\"ЪЪЪ",StatusBU[SelectedDirection].SerialNumber);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                if(ConnectBUR[SelectedDirection]){// если соединение установлено
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page12.t1.pco=%uЪЪЪ",BLACK);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//  29.09.22 замена на рип                  printf("t10.pic=57ЪЪЪ"); // RS485 в норме
                    // отображение состояния авоматики
                    if(!(StatusBU[SelectedDirection].Situation & 0x0F)){
                        // обстановка норма и вся автоматика включена
                        if(/*!StatusBU[SelectedDirection].FlagErrRoom.ErrByte && 14.11.22*/
                        !StatusBU[SelectedDirection].StatusAuto.Manual &&
                        !StatusBU[SelectedDirection].StatusAuto.StopStart &&
                        !IndicationStatusAuto[SelectedDirection].Manual){
                            // все исправно
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.pic=28ЪЪЪ");
                        }else{
                            // есть неисправности или автоматика заблокирована или открыты двери
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.pic=31ЪЪЪ");
                        }
                    }else{
                    // внимание пожар или пуск
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page12.t0.pic=30ЪЪЪ");
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// изм. 01.09.22                    
                    while(TxRunRs || TxRunLcd);
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000 ){
                        case AUTO:
                            if(IndicationStatusAuto[SelectedDirection].Manual){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t0.txt=\"РЕЖИМ РАБОТЫ РУЧНОЙ\"ЪЪЪ");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t0.txt=\"ЗАДЕРЖКА ПУСКА %u СЕКУНД\"ЪЪЪ",(StatusBU[SelectedDirection].StatusAuto.CounterDelay10s + 1) * 10);
                            }
                            break;
                        case LOCK:
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.txt=\"ПУСК ЗАБЛОКИРОВАН\"ЪЪЪ");
                            break;
                        case MANUAL:
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.txt=\"РЕЖИМ РАБОТЫ РУЧНОЙ\"ЪЪЪ");
                            break;
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// отображаем неисправнеости  
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.Pwr2 && !StatusBU[SelectedDirection].FlagErrRoom.Pwr1){
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page12.t11.pic=57ЪЪЪ");
                    }else{
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page12.t11.pic=56ЪЪЪ");
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if((StatusBU[SelectedDirection].SerialNumber%1000) < 501){
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.noteALARM){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t12.pic=57ЪЪЪ");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t12.pic=56ЪЪЪ");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.IPR){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t13.pic=57ЪЪЪ");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t13.pic=56ЪЪЪ");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.SMK){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t14.pic=57ЪЪЪ");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t14.pic=56ЪЪЪ");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.noteAUTO){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t15.pic=57ЪЪЪ");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t15.pic=56ЪЪЪ");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// изм. 25.03.22        
                        if(StatusBU[SelectedDirection].SerialNumber > 2111000){
                            while(TxRunRs || TxRunLcd);
                            if(!FildFlagsPcb2[SelectedDirection].Err_Udp){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t16.pic=57ЪЪЪ");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t16.pic=56ЪЪЪ");
                            }
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            if(!FildFlagsPcb2[SelectedDirection].Err_Uvoa){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t17.pic=57ЪЪЪ");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t17.pic=56ЪЪЪ");
                            }
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            if(FildFlagsPcb2[SelectedDirection].Rip){ // 29.09.22 добавлена неисправность РИП на 12 экран
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t10.pic=56ЪЪЪ");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t10.pic=57ЪЪЪ");
                            }
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        }
//============                        
                    }
                }else{
//  29.09.22 замена на рип                                      printf("t10.pic=56ЪЪЪ");
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page12.t1.pco=%uЪЪЪ",GREY_B);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            break;
        case 13:
            DisplayStatusBOS1202(CurrentScreen,1);
            break;
        case 14:
            if(LcdFlag.NewPage){
                PrintDirectionNumber(CurrentScreen);
                LcdFlag.NewPage = 0;
            }
            if(StatusBU[SelectedDirection].SerialNumber){
        // в направлении есть зарегистрированный БУ
            if(ConnectBUR[SelectedDirection]){
                while(TxRunRs || TxRunLcd);
                switch(StatusBU[SelectedDirection].Situation){
                    case 0:
                        //выкладываем подкладку
                        if(StatusBU[SelectedDirection].StatusAuto.StopStart || StatusBU[SelectedDirection].StatusAuto.Manual || StatusBU[SelectedDirection].FlagErrRoom.ErrByte){
//                            printf("page14.t0.pic=31ЪЪЪ");
                            sprintf(LcdBufferData,"page14.t0.pic=31ЪЪЪ");
                        }else{
//                            printf("page14.t0.pic=28ЪЪЪ");
                            sprintf(LcdBufferData,"page14.t0.pic=28ЪЪЪ");
                        }
                        break;
                    case 1:case 2:case 3:
//                        printf("page14.t0.pic=30ЪЪЪ");
                        sprintf(LcdBufferData,"page14.t0.pic=30ЪЪЪ");
                        break;
                }
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// изм. 01.09.22
                while(TxRunRs || TxRunLcd);
                switch (StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                    case AUTO:
//                        printf("page14.t0.txt=\"АВТОМАТИКА ВКЛЮЧЕНА\"ЪЪЪ");
                        sprintf(LcdBufferData,"page14.t0.txt=\"АВТОМАТИКА ВКЛЮЧЕНА\"ЪЪЪ");
                        break;
                    case LOCK:
//                        printf("page14.t0.txt=\"ПУСК ЗАБЛОКИРОВАН\"ЪЪЪ");
                        sprintf(LcdBufferData,"page14.t0.txt=\"ПУСК ЗАБЛОКИРОВАН\"ЪЪЪ");
                        break;
                    case MANUAL:
//                        printf("page14.t0.txt=\"РЕЖИМ РАБОТЫ РУЧНОЙ\"ЪЪЪ");
                        sprintf(LcdBufferData,"page14.t0.txt=\"РЕЖИМ РАБОТЫ РУЧНОЙ\"ЪЪЪ");
                        break;
                }
                printf("%s",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
//                printf("page14.t0.pic=29ЪЪЪ");
                sprintf(LcdBufferData,"page14.t0.pic=29ЪЪЪ");
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
//                printf("page14.t0.txt=\"НЕТ ДАННЫХ\"ЪЪЪ");
                sprintf(LcdBufferData,"page14.t0.txt=\"НЕТ ДАННЫХ\"ЪЪЪ");
                printf("%s",LcdBufferData);
            }
        }
            break;
        case 15:
            if(LcdFlag.NewPage){
                LcdFlag.NewPage = 0;
            }
            break;
        case 16:
            if(LcdFlag.NewPage || LcdFlag.NewData || NewEventLcdFlag.NewStaus || NewEventLcdFlag.NewSituation){
// ВЫВОДИМ ИНФОВМАЦИЮ О СОСТОЯНИИ БУР                
// устанавливаем подложку бура
                if(LcdFlag.Debug)xprintf("LcdFlag ox%04X, NewEventLcd ox%04X\r",LcdFlag.LcdFlagsWord,NewEventLcdFlag.NewEventLcdWord);
                PrintDirectionNumber(CurrentScreen);
                while(TxRunRs || TxRunLcd);
                if(StatusBU[SelectedDirection].Situation & 0x03){// в направлении есть тревожное состояние 
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page16.t0.pic=105ЪЪЪ"); // подложка красная
                }else{
                    if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* изм. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page16.t0.pic=106ЪЪЪ"); // подложка желтая
                    }else{
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page16.t0.pic=104ЪЪЪ"); // подложка зеленая
                    }
                }
                printf("%s",LcdBufferData);
// выводим обстановку и серийный номер БУРа
                while(TxRunRs || TxRunLcd);
                switch(StatusBU[SelectedDirection].Situation & 0x03){
                        case 0:
                            if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* изм. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(lcdline,"%s","НЕИСПРАВНОСТЬ");
                            }else{
                                sprintf(lcdline,"%s","НОРМА");
                            }
                            break;
                        case 1:
                            sprintf(lcdline,"%s","ВНИМАНИЕ");
                            break;
                        case 2:
                            sprintf(lcdline,"%s","ПОЖАР");
                            break;
                        case 3:
                            sprintf(lcdline,"%s","ПУСК");
                            break;
                    }
                sprintf(LcdBufferData,"page16.t0.txt=\"СПС\\r%s\\r%s С.Н. %lu\\rзарегистрировано\\rИПД:  %u шт.\"ЪЪЪ"
                        ,lcdline,((StatusBU[SelectedDirection].SerialNumber%1000) < 501)? "БУР":"РКН",
                        StatusBU[SelectedDirection].SerialNumber,StatusBU[SelectedDirection].QuantityBos);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                // ВЫВОДИМ ИНФОРМАЦИЮ О СОСТОЯНИИ ИПД
                if(StatusBU[SelectedDirection].QuantityBos){
                    while(TxRunRs || TxRunLcd);
                    if(StatusBos[0].AlarmByte){
                        // в направлении есть тревожное состояние 
                        sprintf(LcdBufferData,"page16.t1.pic=105ЪЪЪ"); // подложка красная
                    }else{
                        if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0x03) ){
                            sprintf(LcdBufferData,"page16.t1.pic=106ЪЪЪ"); // подложка желтая
                        }else{
                            sprintf(LcdBufferData,"page16.t1.pic=104ЪЪЪ"); // подложка зеленая
                        }
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// выводим количество БОС во внимании, пожаре и неисправности
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page16.t1.txt=\"ИПД подробно:\\rв пожаре - %u\\rво внимании - %u\\rв неисправности - %u\"ЪЪЪ"
                        ,CounterFireDevice,CounterAttentionDevice,CounterErrBos);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);

                }
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                if(LcdFlag.NewData){
                    LcdFlag.NewData = 0;
                }
                if(NewEventLcdFlag.NewSituation){
                    NewEventLcdFlag.NewSituation = 0;
                }
                if(NewEventLcdFlag.NewStaus){
                    NewEventLcdFlag.NewStaus = 0;
                }
            }
            break;
        case 17:
            if(LcdFlag.NewPage || LcdFlag.NewData || NewEventLcdFlag.NewStaus || NewEventLcdFlag.NewSituation){
                PrintDirectionNumber(CurrentScreen);
                LcdFlag.NewPage = 0;
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page17.t2.txt=\"%s\"ЪЪЪ",((StatusBU[SelectedDirection].SerialNumber%1000) < 501)?"БУР":"РКН");
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page17.t1.txt=\"%lu\"ЪЪЪ",StatusBU[SelectedDirection].SerialNumber);
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page17.t1.pco=%uЪЪЪ",BLACK);
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                if(!StatusBU[SelectedDirection].Situation){
                        // обстановка норма и вся автоматика включена
                        if(!StatusBU[SelectedDirection].FlagErrRoom.ErrByte){
                            // все исправно
                            sprintf(LcdBufferData,"page17.t0.pic=28ЪЪЪ");
                        }else{
                            // есть неисправности
                            sprintf(LcdBufferData,"page17.t0.pic=31ЪЪЪ");
                        }
                }else{
                // внимание пожар или пуск
                    sprintf(LcdBufferData,"page17.t0.pic=30ЪЪЪ");
                }
    // отображаем неисправнеости            
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                if(!StatusBU[SelectedDirection].FlagErrRoom.Pwr2 && !StatusBU[SelectedDirection].FlagErrRoom.Pwr1){
                    sprintf(LcdBufferData,"page17.t11.pic=57ЪЪЪ");
                }else{
                    sprintf(LcdBufferData,"page17.t11.pic=56ЪЪЪ");
                }
                printf("%s",LcdBufferData);
                if((StatusBU[SelectedDirection].SerialNumber%1000) < 501){
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.noteALARM){
                        sprintf(LcdBufferData,"page17.t12.pic=57ЪЪЪ");
                    }else{
                        sprintf(LcdBufferData,"page17.t12.pic=56ЪЪЪ");
                    }
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.IPR){
                        sprintf(LcdBufferData,"page17.t13.pic=57ЪЪЪ");
                    }else{
                        sprintf(LcdBufferData,"page17.t13.pic=56ЪЪЪ");
                    }
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.SMK){
                        sprintf(LcdBufferData,"page17.t14.pic=57ЪЪЪ");
                    }else{
                        sprintf(LcdBufferData,"page17.t14.pic=56ЪЪЪ");
                    }
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.noteAUTO){
                        sprintf(LcdBufferData,"page17.t15.pic=57ЪЪЪ");
                    }else{
                        sprintf(LcdBufferData,"page17.t15.pic=56ЪЪЪ");
                    }
                    printf("%s",LcdBufferData);
                    if(StatusBU[SelectedDirection].SerialNumber > 2111000){
                        while(TxRunRs || TxRunLcd);
                        if(!FildFlagsPcb2[SelectedDirection].Err_Udp){
                            sprintf(LcdBufferData,"page17.t16.pic=57ЪЪЪ");
                        }else{
                            sprintf(LcdBufferData,"page17.t16.pic=56ЪЪЪ");
                        }
                        printf("%s",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!FildFlagsPcb2[SelectedDirection].Err_Uvoa){
                            sprintf(LcdBufferData,"page17.t17.pic=57ЪЪЪ");
                        }else{
                            sprintf(LcdBufferData,"page17.t17.pic=56ЪЪЪ");
                        }
                        printf("%s",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(FildFlagsPcb2[SelectedDirection].Rip){ // 29.09.22 добавлена неисправность РИП на 12 экран
                            sprintf(LcdBufferData,"page17.t10.pic=56ЪЪЪ");
                        }else{
                            sprintf(LcdBufferData,"page17.t10.pic=57ЪЪЪ");
                        }
                        printf("%s",LcdBufferData);
                    }
                }
                if(LcdFlag.NewPage){
                    LcdFlag.NewPage = 0;
                }
                if(LcdFlag.NewData){
                    LcdFlag.NewData = 0;
                }
                if(NewEventLcdFlag.NewSituation){
                    NewEventLcdFlag.NewSituation = 0;
                }
                if(NewEventLcdFlag.NewStaus){
                    NewEventLcdFlag.NewStaus = 0;
                }
            }
            break;
        case 18:
            if(LcdFlag.NewPage){
                PrintDirectionNumber(CurrentScreen);
                LcdFlag.NewPage = 0;
            }
            if(NewEventLcdFlag.NewDataBos ||  NewEventLcdFlag.RedyDataBos){
                UINT8 i;
                for(i = 1; i < 17; i ++){
                    UINT8 resulT;
                    resulT = Check32Device(i);
                    if(resulT){
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page18.b%u.pic=%uЪЪЪ",i,resulT);
                        printf("%s",LcdBufferData);
                    }
                }
                NewEventLcdFlag.NewDataBos = 0; // снимаем флаг информация о БОС получена
                NewEventLcdFlag.RedyDataBos = 0;
            }
            break;
        case 19:
            if(LcdFlag.NewPage){
                UINT8 i; 
                UINT16 number;
                PrintDirectionNumber(CurrentScreen);
                for(i = 1;i < 33; i ++){
                    while(TxRunRs || TxRunLcd);
                    number = ((GroupNumber - 1)*32 +i);
                    sprintf(LcdBufferData,"page19.b%u.txt=\"%u\"ЪЪЪ",i,number);
                    printf("%s",LcdBufferData);
//                    sprintf(LcdBufferData,"page19.t%u.txt=%uЪЪЪ",i,resulT);
                }
                LcdFlag.NewPage = 0;
                NewEventLcdFlag.NewDataBos = 1;
            }
            if(NewEventLcdFlag.NewDataBos ||  NewEventLcdFlag.RedyDataBos){
                UINT8 i;
                for(i = 1; i < 33; i ++){
                    UINT8 resulT;
                    resulT = CheckDevice((GroupNumber - 1)*32 +i);
                    if(resulT){
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page19.b%u.pic=%uЪЪЪ",i,resulT);
                        printf("%s",LcdBufferData);
                    }
                }
                NewEventLcdFlag.NewDataBos = 0; // снимаем флаг информация о БОС получена
                NewEventLcdFlag.RedyDataBos = 0;
            }
            break;
        case 20:
            DisplayStatusBOS1202(CurrentScreen,DeviceNumber);
            break;
        default:
            break;
    }
}
void SwitchSelect(UINT8 paGe,UINT16 index){
    switch(LcdFlag.Select){
        default: // передача только зарегистрированных бос
            if(StatusBos[index].SerialNumber){
                LcdFlag.Yes = 1;
            }
            break;
        case 2: // передача только тревожных бос
            if(!CounterAlarmBos) {
                LcdFlag.Select = 1;
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt3.val=0ЪЪЪ",paGe);// изм. 30.01.23
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt1.val=1ЪЪЪ",paGe);// изм. 30.01.23
                printf("%s",LcdBufferData);
                break;
            }else{
                if(StatusBos[index].AlarmByte && StatusBos[index].SerialNumber){
                    LcdFlag.Yes = 1;
                }
            }
            break;
        case 3: // передача только неисправных бос
            if(!CounterErrBos) {
                LcdFlag.Select = 1;
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt2.val=0ЪЪЪ",paGe);// изм. 30.01.23
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt1.val=1ЪЪЪ",paGe);// изм. 30.01.23
                printf("%s",LcdBufferData);
                break;
            }else{
                if(StatusBos[index].StatusByte && StatusBos[index].SerialNumber){
                    LcdFlag.Yes =1;
                }
            }
            break;
        case 4: // передача всех бос
            LcdFlag.Yes = 1;
            break;
    }
}
void DisplayStatusBOS1202(unsigned char page,UINT16 deviceNumber){
    UINT16 Quantity;
    if(CurrentScreen != page)return; // если мы не на том экране уходим
// если новый экран
    if (LcdFlag.NewPage){
        IndexBos = deviceNumber;
        while(TxRunRs || TxRunLcd);
        Quantity = StatusBU[SelectedDirection].QuantityBos;
        sprintf(LcdBufferData,"page%u.t13.txt=\"%u\"ЪЪЪ",page,StatusBU[SelectedDirection].QuantityBos);// изм. 24.03.22
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        if (!StatusBos[IndexBos].SerialNumber && StatusBU[SelectedDirection].QuantityBos){
            LcdFlag.HandUp = 1;
        }
        LcdFlag.NewPage = 0;
        if(CurrentScreen == 20){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"vis t2,1ЪЪЪ");
            printf("%s",LcdBufferData);
        }
        return;
    }
    /*отладка*/if(LcdFlag.Debug)xprintf("IndexBos = %u\r",IndexBos);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t14.txt=\"%u\"ЪЪЪ",page,CounterErrBos);// изм. 24.03.22
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t15.txt=\"%u\"ЪЪЪ",page,CounterAlarmBos);// изм. 24.03.22
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    //готовим данные для нового БОС
    if(LcdFlag.HandUp || LcdFlag.Play){
        while(!LcdFlag.Yes /*&& !LcdFlag.End*/){
            IndexBos++;
            if(IndexBos > 512) {
                IndexBos = 1;
            }
            SwitchSelect(page,IndexBos);
        }
    }
    if(LcdFlag.HandDoun){
        while(!LcdFlag.Yes /*&& !LcdFlag.End*/){
            IndexBos--;
            if(!IndexBos) {
                LcdFlag.End = 1;
                IndexBos = 512;
            }
            SwitchSelect(page,IndexBos);
        }
    }
    /*отладка*/if(LcdFlag.Debug)xprintf("NewEventLcdFlag.NewDataBos = %u\r",NewEventLcdFlag.NewDataBos);
    if(LcdFlag.Yes || NewEventLcdFlag.NewDataBos/**/){
        SwitchSelect(page,IndexBos);
        PrintStatusBosNew(page,IndexBos);
        LcdFlag.Yes = 0;
        LcdFlag.HandDoun = 0;
        LcdFlag.HandUp = 0;
        Interval._1s = 0;
        NewEventLcdFlag.NewDataBos = 0;
//        LcdFlag.NewData = 0;
    }
}
void PrintStatusBosNew(unsigned char page,unsigned int index) {
    // выводим номер ячейки
    while(TxRunRs || TxRunLcd);
    switch(page){
        default:
            return;
            break;
        case 13:
            sprintf(LcdBufferData,"page%u.t6.txt=\"БОС N %u\"ЪЪЪ",page,index);
            break;
        case 20:
            sprintf(LcdBufferData,"page%u.t6.txt=\"ИПД N %u\"ЪЪЪ",page,index);
            break;
    }
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    if (!StatusBos[index].SerialNumber) {
        // БОС НЕ ЗАРЕГИСТРИРОВАН
        // выводим С.Н.
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t5.txt=\"СЕРИЙНЫЙ N XXXXXXXXX\"ЪЪЪ",page);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // выводим состояние связи
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t0.pic=56ЪЪЪ",page); //значек ошибки
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t7.txt=\"Нет данных\"ЪЪЪ",page);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // выводим состояние питания
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t1.pic=56ЪЪЪ",page); //значек ошибки
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t8.txt=\"Нет данных\"ЪЪЪ",page);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
 //.........       
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t6.bco=%uЪЪЪ",page,GREY_B);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            // выводим состояние ИП1 или дымовой камеры
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t2.pic=56ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t9.txt=\"Нет данных\"ЪЪЪ",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//''''''''''      
        if(page == 13){
        // выводим состояние ИП2
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t3.pic=56ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t10.txt=\"Нет данных\"ЪЪЪ",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // выводим состояние активатора
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t4.pic=56ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t11.txt=\"Нет данных\"ЪЪЪ",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }
    }else {
        // БОС ЗАРЕГИСТРИРОВАН
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t5.txt=\"СЕРИЙНЫЙ N %lu\"ЪЪЪ",page, StatusBos[index].SerialNumber);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        if (StatusBos[index].StatusByte == 0xFF) {
    // С БОС НЕТ СВЯЗИ
//.........       
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t6.bco=%uЪЪЪ",page,GREY_BB);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//.........   
        // выводим состояние связи
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t0.pic=56ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t7.txt=\"Нет связи\"ЪЪЪ",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            // выводим состояние питания
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t1.pic=56ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t8.txt=\"Нет данных\"ЪЪЪ",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            // выводим состояние ИП1 или дымовой камеры       
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t2.pic=56ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t9.txt=\"Нет данных\"ЪЪЪ",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            if(page == 13){
        // выводим состояние ИП2
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t3.pic=56ЪЪЪ",page); //значек ошибки
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t10.txt=\"Нет данных\"ЪЪЪ",page);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // выводим состояние активатора
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t4.pic=56ЪЪЪ",page); //значек ошибки
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t11.txt=\"Нет данных\"ЪЪЪ",page);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        } else {
            // СВЯЗЬ С БОС В НОРМЕ
            // выводим состояние связи
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t0.pic=57ЪЪЪ",page); //значек ошибки
            printf("%s",LcdBufferData); 
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t7.txt=\"Связь в норме\"ЪЪЪ",page);
            printf("%s",LcdBufferData); 
            // описание источника питания
//......................... 
            while(TxRunRs || TxRunLcd);
            if(StatusBos[index].AlarmByte){
//                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t6.bco=%uЪЪЪ",page,RED);
//                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            } else {
                if(StatusBos[index].StatusByte){
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t6.bco=%uЪЪЪ",page,YELOW_B);
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }else{
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t6.bco=%uЪЪЪ",page,WHITE);
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//.........................
            if (StatusBos[index].StausPwr) {
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t1.pic=56ЪЪЪ",page); //значек ошибки
                printf("%s",LcdBufferData); 
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t8.txt=\"Низкое\"ЪЪЪ",page);
                printf("%s",LcdBufferData); 
            } else {
                if (StatusBos[index].StatusReservPwr) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t1.pic=56ЪЪЪ",page); //значек ошибки
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t8.txt=\"Резервное\"ЪЪЪ",page);
                    printf("%s",LcdBufferData); 
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t1.pic=57ЪЪЪ",page); //значек норма
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t8.txt=\"Основное\"ЪЪЪ",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            if(page == 20){
                // описание дымовой камеры
                if (StatusBos[index].StatusLoopRT0) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=56ЪЪЪ",page); //значек ошибки
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t9.txt=\"Дымовая камера\\rнеисправна\"ЪЪЪ",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=57ЪЪЪ",page); //значек норма
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t9.txt=\"Дымовая камера\\rисправна\"ЪЪЪ",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            if(page == 13){
            // описание термодатчика 1
                if (StatusBos[index].StatusLoopRT0) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=56ЪЪЪ",page); //значек ошибки
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if (StatusBos[index].StatusLoopRT0 == 1) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t9.txt=\"ИП1 обрыв\"ЪЪЪ",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                    if (StatusBos[index].StatusLoopRT0 == 2) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t9.txt=\"ИП1 к.з.\"ЪЪЪ",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=57ЪЪЪ",page); //значек норма
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t9.txt=\"ИП1 исправен\\rТЕМПЕРАТУРА %dC\"ЪЪЪ",page,StatusBos[index].TemperaturaSensor[0]/10);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            // описание термодатчика 2
                if (StatusBos[index].StatusLoopRT1) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t3.pic=56ЪЪЪ",page); //значек ошибки
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if (StatusBos[index].StatusLoopRT1 == 1) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t10.txt=\"ИП1 обрыв\"ЪЪЪ",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                    if (StatusBos[index].StatusLoopRT1 == 2) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t10.txt=\"ИП1 к.з.\"ЪЪЪ",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t3.pic=57ЪЪЪ",page); //значек норма
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t10.txt=\"ИП2 исправен\\rТЕМПЕРАТУРА %dC\"ЪЪЪ",page,StatusBos[index].TemperaturaSensor[1]/10);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                // описание активатора
                if (StatusBos[index].AlarmByte != 3) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.bco=%uЪЪЪ",page, WHITE);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.pco=%uЪЪЪ",page, BLACK);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if (StatusBos[index].Activator) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t4.pic=56ЪЪЪ",page); //значек ошибки
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        if (StatusBos[index].Activator == 1) {
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page%u.t11.txt=\"Обрыв активатора\"ЪЪЪ",page);
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        }
                        if (StatusBos[index].Activator == 2) {
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page%u.t11.txt=\"К.З. активатора\"ЪЪЪ",page);
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        }
                    } else {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t4.pic=57ЪЪЪ",page); //значек норма
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t11.txt=\"Активатор норма\"ЪЪЪ",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t4.pic=56ЪЪЪ",page); //значек ошибки
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.bco=%uЪЪЪ",page, BORDO);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.pco=%uЪЪЪ",page, WHITE);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.txt=\"Пуск прошел\"ЪЪЪ",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
        }
    }
}
UINT8 CheckDevice(UINT16 number){
    UINT8 result = 0;
        if(StatusBos[number].ShortAddres){
            if(!result)result = 1;            // green
            if(StatusBos[number].StatusByte & 0xFE){
                if(result < 2)result = 2;     //yelow
            }
            if(StatusBos[number].AlarmByte){
                if(result < 3)result = 3;     // red
            }
    }
    if(result)result += 115;
    return result;
}
UINT8 Check32Device(UINT8 GroupNumber){
    UINT16 i;
    UINT8 result = 0;
    for(i = ((GroupNumber - 1)*32 + 1);i < ((GroupNumber * 32) + 1); i ++){
        if(StatusBos[i].ShortAddres){
            if(!result)result = 1;            // green 110
            if(StatusBos[i].StatusByte & 0xFE){
                if(result < 2)result = 2;     //yelow 111
            }
            if(StatusBos[i].AlarmByte){
                if(result < 3)result = 3;     // red 112
            }
        }
    }
    if(result)result += 109;
    return result;
}

// функции чтения АРхива
double tempFloat;
static UINT16 NumberEvRead = 0;
static statusAuto TempAutoByte;
static FlagErrBits TempErrByte;
void PrintEventLCD(void) {
    // печатаем номер дату и время события
    while(TxRunRs || TxRunLcd);
    printf("page8.t5.txt=\"%u\"ЪЪЪ",NumberEvRead);
    while(TxRunRs || TxRunLcd);
    printf("page8.t100.txt=\"%02u.%02u.%u\"ЪЪЪ",BufferReadEvent.Day,BufferReadEvent.Month,BufferReadEvent.Year);
    while(TxRunRs || TxRunLcd);
    printf("page8.t103.txt=\"%02u:%02u:%02u\"ЪЪЪ",BufferReadEvent.Hour,BufferReadEvent.Minute,BufferReadEvent.Second);
    switch (BufferReadEvent.TypeEvent) {
        case 1:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Установлена связь\\rнаправление %u\\r%s С.Н. %lu\\rзарегистрированно БОС\\r%u шт.\"ЪЪЪ", BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН",BufferReadEvent.SerialNumber, BufferReadEvent.QuantityBos);
            break;
        case 2:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Потеряна связь\\rс устройством %u\\r%s С.Н. %lu\"ЪЪЪ", BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН",BufferReadEvent.SerialNumber);
            break;
        case 3:
            TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
            switch(BufferReadEvent.Situation & 0x03){
                case 0:
                    sprintf(lcdline,"%s","НОРМА");
                    break;
                case 1:
                    sprintf(lcdline,"%s","ВНИМАНИЕ");
                    break;
                case 2:
                    sprintf(lcdline,"%s","ПОЖАР");
                    break;
                case 3:
                    sprintf(lcdline,"%s","ПУСК");
                    break;
                default:
                    sprintf(lcdline,"%u",BufferReadEvent.Situation);
                    break;
            }
            while(TxRunRs || TxRunLcd);
            if(TempEventRead.classIP < 11){
                printf("page8.t3.txt=\"АУПТ Изменение обстановки\\rНаправление %u\\r%s С.Н. %lu\\r%s\"ЪЪЪ",
                BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН",BufferReadEvent.SerialNumber,&lcdline[0]);
            }else{
                printf("page8.t3.txt=\"СПС Изменение обстановки\\rНаправление %u\\r%s С.Н. %lu\\r%s\"ЪЪЪ",
                BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН",BufferReadEvent.SerialNumber,&lcdline[0]);
            }
           
            break;
        case 4:
            TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
            while(TxRunRs || TxRunLcd);
            if(TempEventRead.classIP < 11){
                printf("page8.t3.txt=\"АУПТ изменение состояния\\r\"ЪЪЪ");
            }else{
                printf("page8.t3.txt=\"СПС изменение состояния\\r\"ЪЪЪ");
            }
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=t3.txt+\"%s %lu направл. %u\\r\"ЪЪЪ",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
            TempErrByte.ErrByte = BufferReadEvent.ErrByte;
            if(TempErrByte.ErrByte || (BufferReadEvent.reserv & 0x07)){
                UINT16 Flag0D = 0;
                while(TxRunRs || TxRunLcd);
                printf("page8.t3.txt=t3.txt+\"Неисправности: \"ЪЪЪ");
                while(TxRunRs || TxRunLcd);
                if (TempErrByte.ConnectBos) {
                    printf("page8.t3.txt=t3.txt+\"Потеря связи.\\r\"ЪЪЪ");
                }else{
                    printf("page8.t3.txt=t3.txt+\"\\r\"ЪЪЪ");
                }
                if (TempErrByte.Bos) {
                    while(TxRunRs || TxRunLcd);
                    if(TempEventRead.classIP < 11){
                        printf("page8.t3.txt=t3.txt+\"БОС. \"ЪЪЪ");
                    }else{
                        printf("page8.t3.txt=t3.txt+\"ИПД. \"ЪЪЪ");
                    }
                    Flag0D = 1;
                }
                if (TempErrByte.IPR) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"ЛС ИП. \"ЪЪЪ");
                    Flag0D = 1;
                }
                if (TempErrByte.SMK) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"ЛС дверей.\"ЪЪЪ");
                    Flag0D = 1;
                }
                if(Flag0D){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"\\r\"ЪЪЪ");
                    Flag0D = 0;
                }
                if (TempErrByte.noteALARM){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"ЛС ОП ТР. \"ЪЪЪ");
                    Flag0D = 1;
                }
                if (TempErrByte.noteAUTO) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"ЛС ОП АВТО.\"ЪЪЪ");
                    Flag0D = 1;
                }
                if(Flag0D){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"\\r\"ЪЪЪ");
                    Flag0D = 0;
                }
                if(BufferReadEvent.reserv & 0x01){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"ЛС УДП. \"ЪЪЪ");
                }
                if(BufferReadEvent.reserv & 0x02){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"ЛС УВОА. \"ЪЪЪ");
                }
                if (TempErrByte.Pwr1 || TempErrByte.Pwr2) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"Ист. пит.\"ЪЪЪ");
                }
            }else{
                while(TxRunRs || TxRunLcd);
                printf("page8.t3.txt=t3.txt+\"Прибор исправен\"ЪЪЪ");
            }
            break;
        case 5: // изм. 28.01.20
            TempAutoByte.StatusAutoByte = BufferReadEvent.StatusAutoByte;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"%s %lu направл. %u\\r\"ЪЪЪ",(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
            while(TxRunRs || TxRunLcd);
            switch(TempAutoByte.StatusAutoByte & 0b11000000){
                case AUTO:
                    printf("page8.t3.txt=t3.txt+\"автоматика включена\"ЪЪЪ");
                    break;
                case LOCK:
                    printf("page8.t3.txt=t3.txt+\"автоматика заблокирована\"ЪЪЪ");
                    break;
                case MANUAL:
                    printf("page8.t3.txt=t3.txt+\"режим работы ручной\"ЪЪЪ");
                    break;
            }
            break;
        case 6:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Направление %u\\rПЕРЕДАНА КОМАНДА СБРОС\\rиспользован ключ N%u\"ЪЪЪ",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 7:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Направление %u\\rПЕРЕДАНА КОМАНДА СТАРТ\\rиспользован ключ N%u\"ЪЪЪ",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 8:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"ДЛЯ ВСЕХ\\rПЕРЕДАНА КОМАНДА СТАРТ\\rиспользован ключ N%u\"ЪЪЪ",BufferReadEvent.reserv);
            break;
        case 14: //  нажата кнопка управления автоматикой
            // автоматика разблокирована кнопкой на кп
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Направление %u\\rАвтоматика РАЗблокирована\\rиспользован ключ N%u\"ЪЪЪ",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 15:
            // автоматика заблокирована кнопкой на кп
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Направление %u\\rАвтоматика ЗАблокирована\\rиспользован ключ N%u\"ЪЪЪ",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 100:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Включено питания КП\"ЪЪЪ");// 19 сим.
        break;
        case 101:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Изменены время и дата\"ЪЪЪ");//21
            break;
        case 102:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Ячейка %u\\rДобавлен %s %lu\"ЪЪЪ",BufferReadEvent.Direct ,(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН",BufferReadEvent.SerialNumber);// 21 + 15
            break;
        case 103:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Ячейка %u\\rУдален %s %lu\"ЪЪЪ",BufferReadEvent.Direct ,(BufferReadEvent.SerialNumber%1000) < 501?"БУР":"РКН",BufferReadEvent.SerialNumber);// 21 + 15
            break;
        case 104:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Изменение питания КП\\r\"ЪЪЪ"); // 20
            tempFloat = (float) (BufferReadEvent.IdNet);
            tempFloat = tempFloat / 100;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt==t3.txt+\"PWR1 %0.2fВ\\r\"ЪЪЪ", tempFloat);
            tempFloat = (float) (BufferReadEvent.QuantityBos);
            tempFloat = tempFloat / 100;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=t3.txt+\"PWR2 %0.2fВ\"ЪЪЪ", tempFloat);
            break;
        case 105:
            OldAccessPassword =  BufferReadEvent.QuantityBos << 8;
            OldAccessPassword = OldAccessPassword + BufferReadEvent.IdNet;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"Изменение ПАРОЛЯ\\rстарый %lu\\rНОВЫЙ %lu\"ЪЪЪ",OldAccessPassword,BufferReadEvent.SerialNumber); // 20
            break;
        case 106:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"зарегистрирован КИД %u\\r%lu\"ЪЪЪ",BufferReadEvent.reserv,BufferReadEvent.SerialNumber);
            break;
        default:
            while(TxRunRs || TxRunLcd);
            if (BufferReadEvent.TypeEvent == 255) {
                printf("page8.t3.txt=\"нет события\"ЪЪЪ");
            } else {
                printf("page8.t3.txt=\"событие %u не опознано\"ЪЪЪ",BufferReadEvent.TypeEvent);
            }
            break;
    }
}
static UINT16 CounterInactiv;
void DisplayReadArhiv0525(void) {
    if(CurrentScreen != 8)return;
    if (LcdFlag.NewPage) {
        LcdFlag.NewPage = 0;
        CounterDelaySec = 2;
        NumberEvRead = CurrentEventWrite - 1;
        ReadEvent(NumberEvRead);    // читаем событие из AT45
        PrintEventLCD();            // печатаем сообщение на экран
        LcdFlag.Yes = 0;
        LcdFlag.HandDoun = 0;
        LcdFlag.HandUp = 0;
        LcdFlag.Play = 0;
    }
    if( !Interval._1s) return;//LcdFlag.Play &&
    if(LcdFlag.Play || LcdFlag.HandDoun || LcdFlag.HandUp){//
        if(LcdFlag.HandDoun || LcdFlag.Play){//
            NumberEvRead--;
            if (NumberEvRead == 65535) {
                NumberEvRead = MAX_QUANTITY_EVENT;
            }
        }
        if(LcdFlag.HandUp && !LcdFlag.Play){//
            NumberEvRead++;
            if (NumberEvRead > MAX_QUANTITY_EVENT) {
                NumberEvRead = 0;
            }
        }
        ReadEvent(NumberEvRead);    // читаем событие из AT45
        if(BufferReadEvent.TypeEvent != 255){
            // здесь вставляем фильтрацию по типу события
            switch(LcdFlag.Select){
                case 1:
                    LcdFlag.Yes = 1;
                    break;
                case 2:
                    if (BufferReadEvent.TypeEvent == 3){
                        LcdFlag.Yes = 1;
                    }
                    break;
                case 3:
                    if (BufferReadEvent.TypeEvent == 1 || BufferReadEvent.TypeEvent == 2 || BufferReadEvent.TypeEvent == 4){
                        LcdFlag.Yes = 1;
                    }
                    break;
            }
            while(TxRunRs || TxRunLcd);
            if(LcdFlag.Yes){
                if(LcdFlag.No){
                    LcdFlag.No = 0;
                    printf("page8.t4.bco=%uЪЪЪ", WHITE);
                }
            }else{
                if(!LcdFlag.No){
                    LcdFlag.No = 1;
                    printf("page8.t4.bco=%uЪЪЪ", YELOW);
                }
            }
        }else{
            if(!LcdFlag.No){
                LcdFlag.No = 1;
                printf("page8.t4.bco=%uЪЪЪ", YELOW);
            }
        }
    }
    if(LcdFlag.Yes ){
        PrintEventLCD();            // печатаем сообщение на экран
        CounterInactiv ++;
        if(!(CounterInactiv % 30)){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page0.Inactiv1.val=60ЪЪЪ"); // задаем время выхода на 0 экран при отсутствии актвности оператора
            printf("%s",LcdBufferData);
        }
        LcdFlag.Yes = 0;
        LcdFlag.HandDoun = 0;
        LcdFlag.HandUp = 0;
        Interval._1s = 0;
    }
}

void PrintEventUSB(void) {
    if (BufferReadEvent.TypeEvent != 255) {
        Interval.usb50ms = 0;
        xprintf("%u-%02u-%02u %02u:%02u:%02u ",
                BufferReadEvent.Year, BufferReadEvent.Month, BufferReadEvent.Day,
                BufferReadEvent.Hour, BufferReadEvent.Minute, BufferReadEvent.Second);
        switch (BufferReadEvent.TypeEvent) {
            case 100://включение питания КП
                xprintf("БЙКЧВЕМХЕ ОХРЮМХЪ йо\r");
                break;
            case 101://изменены время и дата
                xprintf("ХГЛЕМЕМШ БПЕЛЪ Х ДЮРЮ\r");
                break;
            case 102://добавлен БУ
                xprintf("ъВЕИЙЮ %u ", BufferReadEvent.Direct);
                xprintf("ДНАЮБКЕМ %s %lu\r", (BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм",BufferReadEvent.SerialNumber); // пйм
                break;
            case 103://удален БУ
                xprintf("ъВЕИЙЮ %u ", BufferReadEvent.Direct);
                xprintf("СДЮКЕМ %s %lu\r",(BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм", BufferReadEvent.SerialNumber);
                break;
            case 104://изм. напряжение питания КП
                xprintf("ХГЛ. МЮОПЪФЕМХЕ ОХРЮМХЪ йо ");
                xprintf("PWR1 %u,%uб ", BufferReadEvent.IdNet/100,BufferReadEvent.IdNet%100);
                xprintf("PWR2 %u,%uб\r", BufferReadEvent.QuantityBos/100,BufferReadEvent.QuantityBos%100);
                break;
            case 105: // 16.09.22
                OldAccessPassword =  BufferReadEvent.QuantityBos << 8;
                OldAccessPassword = OldAccessPassword + BufferReadEvent.IdNet;
                xprintf("хГЛЕМЕМХЕ оюпнкъ ЯРЮПШИ %lu МНБШИ %lu\r",OldAccessPassword,BufferReadEvent.SerialNumber);
                break;
            case 106:
                xprintf("гЮПЕЦХЯРПХПНБЮМ йхд-%u %lu\r",BufferReadEvent.reserv,BufferReadEvent.SerialNumber);
                break;
            case 1://уст. связь с БУ
                xprintf("СЯР. ЯБЪГЭ Я %s %lu ",(BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм", BufferReadEvent.SerialNumber);
                xprintf("МЮОПЮБКЕМХЕ %u ", BufferReadEvent.Direct);
                xprintf("ГЮПЕЦХЯРПХПНБЮММН %u аня\r", BufferReadEvent.QuantityBos);
                break;
            case 2://потеряна связь с БУ
                xprintf("ОНРЕПЪМЮ ЯБЪГЭ Я %s %lu ", (BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм",BufferReadEvent.SerialNumber);
                xprintf("МЮОПЮБКЕМХЕ %u\r", BufferReadEvent.Direct);
                break;
            case 3://изменение обстановки
                TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
                switch (BufferReadEvent.Situation) {
                    case 0:
                        sprintf(lcdline,"%s","мнплю");
                        break;
                    case 1:
                        sprintf(lcdline,"%s","бмхлюмхе");
                        break;
                    case 2:
                        sprintf(lcdline,"%s","онфюп");
                        break;
                    case 3:
                        sprintf(lcdline,"%s","осяй");
                        break;
                }
                if(TempEventRead.classIP < 11){
                    xprintf("юсор %s %lu МЮОПЮБК. %u НАЯРЮМНБЙЮ %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм", BufferReadEvent.SerialNumber, BufferReadEvent.Direct,&lcdline[0]);
                }else{
                    xprintf("яоя %s %lu МЮОПЮБК. %u НАЯРЮМНБЙЮ %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм", BufferReadEvent.SerialNumber, BufferReadEvent.Direct,&lcdline[0]);
                }
                break;
            case 4://изменение состояния оборудования
                TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
                xprintf("ХГЛЕМЕМХЕ ЯНЯРНЪМХЪ ");
                xprintf("%s %lu МЮОПЮБК. %u\r",(BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
                TempErrByte.ErrByte = BufferReadEvent.ErrByte;
                if(TempErrByte.ErrByte || BufferReadEvent.reserv){
                    if (TempErrByte.ConnectBos) {
                        if(TempEventRead.classIP < 11){
                            xprintf("ОНРЕПЪМЮ ЯБЪГЭ Я аня\r");
                        }else{
                            xprintf("ОНРЕПЪМЮ ЯБЪГЭ Я ход\r");
                        }
                    }
                    if (TempErrByte.Bos) {
                        if(TempEventRead.classIP < 11){
                            xprintf("МЕХЯОПЮБМНЯРЭ аня\r");
                        }else{
                            xprintf("МЕХЯОПЮБМНЯРЭ ход\r");
                        }
                    }
                    if (TempErrByte.IPR) {
                        xprintf("МЕХЯОПЮБМНЯРЭ кя хо\r");
                    }
                    if (TempErrByte.SMK) {
                        xprintf("МЕХЯОПЮБМНЯРЭ кя ДБЕПЕИ\r");
                    }
                    if (TempErrByte.noteALARM) {
                        xprintf("МЕХЯОПЮБМНЯРЭ кя но рпебнцю\r");
                    }
                    if (TempErrByte.noteAUTO) {
                        xprintf("МЕХЯОПЮБМНЯРЭ кя но юбрнлюрхйю\r");
                    }
                    if (TempErrByte.Pwr1) {
                        xprintf("МЕХЯОПЮБМНЯРЭ ХЯРНВМХЙЮ ОХРЮМХЪ асп\r");
                    }
// изм. 28.03.22                    
                    if(BufferReadEvent.SerialNumber > 2111000){
                        if(BufferReadEvent.reserv & 0x01){
                            xprintf("МЕХЯОПЮБМНЯРЭ кя сдо\r"); // неисправность ШС УДП
                        }
                        if(BufferReadEvent.reserv & 0x02){
                            xprintf("МЕХЯОПЮБМНЯРЭ кя сбню\r");// неисправность ШС УВОА
                        }
                    }
//================                    
                }else{
                     xprintf("НАНПСДНБЮМХЕ МНПЛЮ\r");
                }
                break;
            case 5:// изменилось состояние автомаьтики
                TempAutoByte.StatusAutoByte = BufferReadEvent.StatusAutoByte;
                if(!TempAutoByte.Manual && !TempAutoByte.StopStart){
                    sprintf(lcdline,"%s","ЮБРНЛЮРХЙЮ БЙКЧВМЕМЮ"); // изм. 01.09.22
                    // автоматика включена, двери закрыты
                }else{
                    if(TempAutoByte.StopStart){
                        sprintf(lcdline,"%s","ЮБРНЛЮРХЙЮ ГЮАКНЙХПНБЮМЮ");
                        // автоматика заблокированпа
                    }
                    if(TempAutoByte.Manual){
                        sprintf(lcdline,"%s","ПЕФХЛ ПЮАНРШ ПСВМНИ"); // изм. 01.09.22
                        // двери открыты
                    }
                }
                xprintf("%s %lu МЮОПЮБКЕМХЕ %u %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"асп":"пйм",
                            BufferReadEvent.SerialNumber,BufferReadEvent.Direct,&lcdline[0]);
                break;
        case 6:// команда сброс
            xprintf("мЮОПЮБКЕМХЕ %u, оепедюмю йнлюмдю яапня, ХЯОНКЭГНБЮМ ЙКЧВ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 7:// команда пуск по направлению
            xprintf("мЮОПЮБКЕМХЕ %u, оепедюмю йнлюмдю ярюпр, ХЯОНКЭГНБЮМ ЙКЧВ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 8:// ШВ команда пуск
            xprintf("дкъ бяеу, оепедюмю йнлюмдю ярюпр, ХЯОНКЭГНБЮМ ЙКЧВ N%u\r",BufferReadEvent.reserv);
            break;
        case 14: //  нажата кнопка управления автоматикой
            // автоматика разблокирована кнопкой на кп
            xprintf("мЮОПЮБКЕМХЕ %u, юБРНЛЮРХЙЮ пюгАКНЙХПНБЮМЮ, ХЯОНКЭГНБЮМ ЙКЧВ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 15:
            // автоматика заблокирована кнопкой на кп
            xprintf("мЮОПЮБКЕМХЕ %u, юБРНЛЮРХЙЮ гюАКНЙХПНБЮМЮ, ХЯОНКЭГНБЮМ ЙКЧВ N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        default:
            Nop();
            xprintf("ЯНАШРХЕ %u МЕ НОНГМЮМН\r", BufferReadEvent.TypeEvent);
            break;
        }
    }
}
void TransmittArhivUSB(void) {
    if(CurrentScreen != 9)return;
    if (LcdFlag.NewPage) {
        LcdFlag.NewPage = 0;
        CounterDelaySec = 2;
        NumberEvRead = CurrentEventWrite - 1;
        while(TxRunRs || TxRunLcd);
        if(!USB_EN){
            sprintf(LcdBufferData,"page9.t2.txt=\"USB НЕ готов\"ЪЪЪ");
            LED_USB_REDY = 1;
        }else{
            sprintf(LcdBufferData,"page9.t2.txt=\"USB готов\"ЪЪЪ");
            LED_USB_REDY = 0;
        }
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page0.Inactiv1.val=600ЪЪЪ"); // задаем время выхода на 0 экран при отсутствии актвности оператора
        printf("%s",LcdBufferData);
        LcdFlag.Select =0;
    }
    // изм. 27.01.20
    if (LED_USB_REDY == USB_EN) {
        // изменилось состояние подключения USB
        while(TxRunRs || TxRunLcd);
        if (USB_EN) {
            // USB подключен
            if (!LcdFlag.Select) {
                sprintf(LcdBufferData,"page9.t2.txt=\"USB готов\"ЪЪЪ");
            }
        } else {
            // USB не подключен
            if (!LcdFlag.Select) {
                sprintf(LcdBufferData,"page9.t2.txt=\"USB НЕ готов\"ЪЪЪ");
            } else {
                sprintf(LcdBufferData,"click bt0,1ЪЪЪ");
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page9.t2.txt=\"передача АРХИВА прервана\"ЪЪЪ");
                LcdFlag.Select = 0;
            }
            printf("%s",LcdBufferData);
        }
        LED_USB_REDY = !USB_EN;
    }
    //конец изм. 27.01.20
    if (LcdFlag.Select && !LED_USB_REDY) {
        if (NumberEvRead == (CurrentEventWrite - 1)) {
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page9.t2.txt=\"передача АРХИВА на ПК\"ЪЪЪ");
            printf("%s",LcdBufferData);
        }
        if(!ENTX485 && ENTX485_I){
            ReadEvent(NumberEvRead);
            while(!Interval.usb50ms)continue;
            CounterInactiv ++;
            if(!(CounterInactiv % 100)){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page0.Inactiv1.val=60ЪЪЪ"); // задаем время выхода на 0 экран при отсутствии актвности оператора
                printf("%s",LcdBufferData);
            }
            PrintEventUSB();// изм. 27.01.20
            if (NumberEvRead) {
                NumberEvRead--;
            } else {
                NumberEvRead = MAX_QUANTITY_EVENT;
            }
        }
    }
    if (NumberEvRead == CurrentEventWrite && LcdFlag.Select) {
        // память прочитана завершаем процесс чтения
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page9.bt0.val=0ЪЪЪ");
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page9.bt0.txt=\"ПЕРЕДАТЬ\"ЪЪЪ");
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page9.t2.txt=\"передача АРХИВА завершена\"ЪЪЪ");
        xprintf("оепедювю юпухбю гюбепьемю\r");
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page0.Inactiv1.val=60ЪЪЪ"); // задаем время выхода на 0 экран при отсутствии актвности оператора
        printf("%s",LcdBufferData);
        LcdFlag.Select = 0;
    }
}
static UINT16 LastIntegral;
IntegralUnion IntegralRegistr;
// изм. 06.04.22 +++++++++++++++
UINT8 CheckIntegralStatus(void){
    UINT8 k;
    UINT8 result = 0;
    ControlFlagCP.NoBurNew = 0;
// изм.06.04.22 проверка наличия связи со всеми приборами
    for(k = 1;k < 11; k ++){
        if(StatusBU[k].SerialNumber){
            if(!ConnectBUR[k]) {
                ControlFlagCP.NoBurNew = 1;
                result ++; // если потерян прибор
            }else{
                if(StatusBU[k].FlagErrRoom.ErrByte || (FildFlagsPcb2[k].ByteFlagRoom_Pcb2 & 0x07)) result ++; // если у прибора есть неисправности
            }
        }
    }
    return result;
}
//+++++++++++
UINT8 CheckIntegralSituation(void){
    UINT8 k;
    UINT8 temp = 0;
    for(k = 1;(k < 11); k ++){
        if(temp < (StatusBU[k].Situation&0x03)){
            temp = StatusBU[k].Situation&0x03;
        }
    }
    return temp;
}
// изм.12.07.22
void CheckIntegralAuto(void){
    UINT8 k;
    LED_AUTO = 0;
    for(k = 1; k < 11; k ++){
        if(StatusBU[k].StatusAuto.Manual || StatusBU[k].StatusAuto.StopStart){
            LED_AUTO = 1;
        }
    }
}
//конец изм.12.07.22     
void IndicationControl(void){
    static UINT8    CounterIndication;
    static UINT8    PeriodIndication;
    
    if(!Interval._SoundCtrl)return;
    IntegralRegistr.IntegralStatus = CheckIntegralStatus();
    IntegralRegistr.IntegralSituation = CheckIntegralSituation();
    CheckIntegralAuto(); // изм. 12.07.22
// изм. 18.10.22
    if(LastIntegral != IntegralRegistr.Integral){
        /*отладка*/if(LcdFlag.Debug)xprintf("last %u new %u\r",LastIntegral,IntegralRegistr.Integral);
        if(IntegralRegistr.Integral > LastIntegral){ // если появились новые неисправности
            /*отладка*/if(LcdFlag.Debug)xprintf("sound on\r");
            ControlFlagCP.CP_Sound_off = 0;
            if(CurrentScreen == 10){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t12.pic=57ЪЪЪ");
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }           
        LastIntegral = IntegralRegistr.Integral;
        
    }
    Interval._SoundCtrl = 0;
    if(!IntegralRegistr.Integral && !ControlFlagCP.RsBreakLast/* изм. 08.04.22 */&& !ControlFlagCP.ErrorRip/* изм. 13.04.22*/&& !ErrPwr){
        EXT_SOUND = SOUND = 0;
        LED_ERROR = 1;
        OUT_ERROR = 1;
        LED_FIRE = 0;
        OUT_FIRE = 0;
        LED_START = 0;
        OUT_START = 0;
        return;
    }
// изм 25.03.22
    if(IntegralRegistr.IntegralStatus || ControlFlagCP.RsBreakLast/* изм. 08.04.22 */ || ControlFlagCP.ErrorRip/* изм. 13.04.22*/|| ErrPwr){
        LED_ERROR = 0;
        OUT_ERROR = 0;
    }else{
        LED_ERROR = 1;
        OUT_ERROR = 1;
    }
//=============    
    
    switch(IntegralRegistr.IntegralSituation){
        case 0:
            PeriodIndication = 80;
// изм 25.03.22            LED_ERROR = 0;
            LED_FIRE = 0;   // 29.09.22
            OUT_FIRE = 0;   // 29.09.22
            LED_START = 0;  // 29.09.22
            OUT_START = 0;  // 29.09.22
            if(!CounterIndication){
                if(!ControlFlagCP.CP_Sound_off)SOUND = 1;
            }else{
                SOUND = 0;
            }
            EXT_SOUND = 0;
            break;
        case 1:
            PeriodIndication = 20;
            if(!CounterIndication || CounterIndication == 12 || CounterIndication == 16){
                if(!ControlFlagCP.CP_Sound_off)SOUND = 1;
            }
            if(CounterIndication == 10 || CounterIndication == 14 || CounterIndication == 18){
                SOUND = 0;
            }
            break;
        case 2:
            PeriodIndication = 20;
            LED_FIRE = 1;
            OUT_FIRE = 1;
            if(!CounterIndication || CounterIndication == 4 || CounterIndication == 8){
                if(!ControlFlagCP.CP_Sound_off) SOUND = 1;
            }
            if(CounterIndication == 2 || CounterIndication == 6 || CounterIndication == 10){
                SOUND = 0;
            }
            EXT_SOUND = 1;
            break;
        case 3:
            LED_START = 1;
            OUT_START = 1;
            PeriodIndication = 0;
            if(!ControlFlagCP.CP_Sound_off){
                SOUND = 1;
            }else{
                SOUND = 0;
            }
            EXT_SOUND = 1;
            break;
    }
    CounterIndication ++;
    if(CounterIndication >= PeriodIndication){
        CounterIndication = 0;
    }
}
//++++++++++++++++++++
static UINT16   CounterKey[4];
#define COUNTER_KEY_START   CounterKey[0]
#define COUNTER_KEY_AUTO    CounterKey[1]
#define COUNTER_KEY_SOUND   CounterKey[2]
FlagKey_t FlagKey;
void CheckKeyAuto(void){ // изм. 22.08.22 
    if(!KEY_AUTO ){// если нажата кнопка АВТОМАТИКА&& COUNTER_KEY_AUTO < 300
        if(COUNTER_KEY_AUTO < 300){
            COUNTER_KEY_AUTO ++;
        }else{
            if(!FlagKey.KeyAutoLong){
                if(COUNTER_KEY_AUTO == 300){
                    COUNTER_KEY_AUTO ++;
                    FlagKey.KeyAutoLong = 1;
                    /*отладка*/if(LcdFlag.Debug)xprintf("Long auto\r");
                }
            }
        }
    }else{
        if(COUNTER_KEY_AUTO > 10 && COUNTER_KEY_AUTO < 300){
            if(!FlagKey.KeyAutoShort){
                FlagKey.KeyAutoShort = 1; // флаг длительность нажатия 100...3000 мс
                /*отладка*/if(LcdFlag.Debug)xprintf("Short auto\r");
            }
        }
        if(COUNTER_KEY_AUTO){
            COUNTER_KEY_AUTO = 0;
        }
    }
}
void CheckKeySound(void){ // изм. 25.08.22 
    if(!KEY_SOUND ){// если нажата кнопка ЗВУК && COUNTER_KEY_SOUND < 300
        if(COUNTER_KEY_SOUND < 300){
            COUNTER_KEY_SOUND ++;
        }else{
            if(!FlagKey.KeySoundLong){
                if(COUNTER_KEY_SOUND == 300){
                    COUNTER_KEY_SOUND ++;
                    FlagKey.KeySoundLong = 1;
                }
            }
        }
    }else{
        if(COUNTER_KEY_SOUND > 10 && COUNTER_KEY_SOUND < 300){
            if(!FlagKey.KeySoundShort){
                FlagKey.KeySoundShort = 1; // флаг длительность нажатия 100...3000 мс
//                xprintf("Short\r");
            }
        }
        if(COUNTER_KEY_SOUND){
            COUNTER_KEY_SOUND = 0;
        }
    }
}
void CheckKeyStart(void){ // изм. 25.08.22 
    if(!KEY_START ){// если нажата кнопка ЗВУК && COUNTER_KEY_START < 300
        if(COUNTER_KEY_START < 300){
            COUNTER_KEY_START ++;
        }else{
            if(!FlagKey.KeyStartLong){
                if(COUNTER_KEY_START == 300){
                    COUNTER_KEY_START ++;
                    FlagKey.KeyStartLong = 1;
                    /*отладка*/if(LcdFlag.Debug)xprintf("Long start\r");
                }
            }
        }
    }else{
        if(COUNTER_KEY_START > 10 && COUNTER_KEY_START < 300){
            if(!FlagKey.KeyStartShort){
                FlagKey.KeyStartShort = 1; // флаг длительность нажатия 100...3000 мс
                 /*отладка*/if(LcdFlag.Debug)xprintf("Short start\r");
            }
        }
        if(COUNTER_KEY_START){
            COUNTER_KEY_START = 0;
        }
    }
}

void KeyCheckMain(void){
    if(!Interval.Key_10ms)return;
//===================== обработчик кнопки СТАРТ
    CheckKeyStart();    // изм. 25.08.22
//===================== обработчик кнопки АВТОМАТИКА    
    CheckKeyAuto();     // изм. 22.08.22  
//===================== обработчик кнопки ЗВУК 
    CheckKeySound();    // изм. 25.08.22
    Interval.Key_10ms = 0;
}
void KeyHandler(void){
    if(!FlagKey.FlagKeyStatus)return;
    NumberKID = SearchNumberKid();
    if(NumberKID > 0 && NumberKID < 11){ // если ключ приложен
        if(SelectedDirection > 0 && SelectedDirection < 11){// если направление выбрано
            if(FlagKey.KeyStartShort || FlagKey.KeyStartLong || FlagKey.KeyAutoLong || FlagKey.KeyAutoShort || FlagKey.KeySoundLong){ //если нажата кнопка старт или кнопка автоматик
                TempDirectRs = SelectedDirection; // сохраняем номер направления
                if(CurrentScreen == 11 || CurrentScreen == 12 || CurrentScreen == 14){
//                    printf("page%u.t0.txt=\"ОЖИДАНИЕ РЕАКЦИИ\"ЪЪЪ",CurrentScreen);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t0.txt=\"ОЖИДАНИЕ РЕАКЦИИ\"ЪЪЪ",CurrentScreen);
                    printf("%s",LcdBufferData);
                }
                if(FlagKey.KeyStartLong){ // длинное нажатие кнопки старт
                    if((StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000) != LOCK){
             /*отладка*/if(LcdFlag.Debug)xprintf("Com StartAll\r");
                        TempCommandRs = 7;  // команда групповой старт по направлению обработку в БУ надо прописать
                        SaveEvent(7); //
                    }else{
//                        printf("page%u.t0.txt=\"ПУСК НЕВОЗМОЖЕН\"ЪЪЪ",CurrentScreen);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t0.txt=\"ПУСК НЕВОЗМОЖЕН\"ЪЪЪ",CurrentScreen);
                        printf("%s",LcdBufferData);
                    }
                }
                if(FlagKey.KeyStartShort){ // короткое нажатие кнопки старт
                    if((StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000) != LOCK){
             /*отладка*/if(LcdFlag.Debug)xprintf("Com StartLocal\r");
                        TempCommandRs = 10;  // команда локальный старт по направлению обработку в БУ надо прописать
                        SaveEvent(7); //
                    }else{
//                        printf("page%u.t0.txt=\"ПУСК НЕВОЗМОЖЕН\"ЪЪЪ",CurrentScreen);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t0.txt=\"ПУСК НЕВОЗМОЖЕН\"ЪЪЪ",CurrentScreen);
                        printf("%s",LcdBufferData);
                    }
                }
                if(FlagKey.KeyAutoLong){ // длинное нажатие кнопки автоматика
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                        case AUTO:
                 /*отладка*/if(LcdFlag.Debug)xprintf("Com Locking\r");
                            TempCommandRs = 5;
                            SaveEvent(15); // событие нажата кнока управления автоматикой на КП 
                            break;
                        case MANUAL:
                            // формируем команду автоматика заблокирована
                 /*отладка*/if(LcdFlag.Debug)xprintf("Com Locking\r");
                            TempCommandRs = 5;
                            SaveEvent(15); // событие нажата кнока управления автоматикой на КП 
                            break;
                        case LOCK:
                            // формируем команду автоматика разблокирована
                 /*отладка*/if(LcdFlag.Debug)xprintf("Com Unlocking\r");
                            TempCommandRs = 4;
                            SaveEvent(14); // событие нажата кнока управления автоматикой на КП 
                            break;
                    }
                }
                if(FlagKey.KeyAutoShort){ // короткое нажатие кнопки автоматика
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                        case AUTO:
                 /*отладка*/if(LcdFlag.Debug)xprintf("Com Manual\r");
                            TempCommandRs = 8;
                            SaveEvent(15); // событие нажата кнока управления автоматикой на КП 
                            break;
                        case MANUAL:
                 /*отладка*/if(LcdFlag.Debug)xprintf("Com Auto\r");
                            TempCommandRs = 9;
                            SaveEvent(14); // событие нажата кнока управления автоматикой на КП 
                            break;
                        case LOCK:
                            // формируем команду автоматика разблокирована
                 /*отладка*/if(LcdFlag.Debug)xprintf("Com Unlocking\r");
                            TempCommandRs = 4;
                            SaveEvent(14); // событие нажата кнока управления автоматикой на КП 
                            break;
                    }
                }
                if(FlagKey.KeySoundLong){ // длинное нажатие кнопки сброс
                    TempCommandRs = 6;
         /*отладка*/if(LcdFlag.Debug)xprintf("Com Reset direct\r");
                }
            }
        }else{// если направление НЕ выбрано
            if(FlagKey.KeySoundShort || FlagKey.KeySoundLong){ // если нажата кнопка звук
     /*отладка*/if(LcdFlag.Debug)xprintf("Press test\r");
                if(CurrentScreen == 10 && !IntegralRegistr.IntegralSituation){
                    // 10.10.22 тест
                    while(TxRunRs || TxRunLcd);
                    WaitingScreen = 15;
                    sprintf(LcdBufferData,"page 15ЪЪЪ");    // переход на стр. тест
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
        }
    }else{ // если ключ не приложен
        if(FlagKey.KeySoundShort || FlagKey.KeySoundLong){ // если нажата кнопка звук
            ControlFlagCP.CP_Sound_off = !ControlFlagCP.CP_Sound_off;
            /*отладка*/if(LcdFlag.Debug)xprintf("Change sound\r");
            if(CurrentScreen == 10){    // выводим состояние звуковой сигнализации
                if(ControlFlagCP.CP_Sound_off){
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=56ЪЪЪ");
                }else{
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=57ЪЪЪ");
                }
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }
    }
    FlagKey.FlagKeyStatus = 0; // обработка кнопок завершена сброс всех флагов
}

unsigned int ReadADC50(unsigned char AN){
    unsigned int    TempResult = 0;
    unsigned char ii = 0;
    AD1CHS = AN;
    AD1CON1bits.ADON = 1; // включить АЦП
    while(ii < 50){
        AD1CON1bits.SAMP = 1; // начать выборку ...
        while (!AD1CON1bits.DONE); // преобразование выполнено?
        TempResult += ADC1BUF0;
        ii ++;
    }
    AD1CON1bits.ADON = 0; // выключить АЦП
    return TempResult/50;
}
UINT8 CheckMyPwr(unsigned char ch){
    UINT16  tempAdc;
    if(!ch){
        tempAdc = ReadADC50(6);
    }else{
        tempAdc = ReadADC50(7);
    }
    // изм. 13.04.22
    tempAdc = (tempAdc*9/5);
    ValueVdd[ch] = tempAdc;
    if(tempAdc < 900){
        return 1;
    }else{
        if(tempAdc > 1650){
            return 2;
        }else{
            return 0;
        }
    }
}
void CheckStausPWR(void){
    UINT8 TempStatusPwr = 0;
    TempStatusPwr = CheckMyPwr(0);
    if(TempStatusPwr){
        LED_PWR1 = 0;
    }else{
        LED_PWR1 = 1;
    }
    if(TempStatusPwr != StatusPwr[0]){
        StatusPwr[0] = TempStatusPwr;
        SaveEvent(104);
    }
    TempStatusPwr = CheckMyPwr(1);
    if(TempStatusPwr){
        LED_PWR2 = 0;
    }else{
        LED_PWR2 = 1;
    }
    if(TempStatusPwr != StatusPwr[1]){
         // событие изменилось напряжение питания по 1 входу
        StatusPwr[1] = TempStatusPwr;
        SaveEvent(104);
    }
    ErrPwr = StatusPwr[1]|StatusPwr[0];
}
void TestKey(void){
    if(!KEY_AUTO || !KEY_START || !KEY_SOUND){
        SOUND = 1;
    }else{
        SOUND = 0;
    }
}
// изм. 01.04.22

void SavingRegisteredKid(UINT8 num){
    UINT8 pos;
    MainMemoryPageToBuffer(2,2045); // читаем страницу с установочными данными из памяти AT45
    // рассчитываем позицию с которой пишим номер
    pos = ((num - 1) * 4)/* +26*/;
    WriteToBufferAt45(2,pos,4,&SerialNumberKid[0].BufferKid[0]); // записываем данные в буфер
    BufferToMainMemoryPageE(2,2045); // переносим буфер в страницу памяти
}
void ReadRegisteredKid(UINT8 num){
    UINT8 pos;
    pos = ((num - 1) * 4)/* +26*/;
    MainMemoryArrayRead(2045,pos,4,&SerialNumberKid[num].BufferKid[0]);
    if(SerialNumberKid[num].SerialKid == 0xFFFFFFFF){
        SerialNumberKid[num].SerialKid = 0;
    }
}
UINT8 SearchNumberKid(void){
    UINT8 jj = 0;
    SerialNumberKid[0].SerialKid = 0;
    FlagMFRC522._ReadCart = 1;
    FlagMFRC522._newCart = 0;
    counterReadCart = 3;
    while(!FlagMFRC522._newCart && counterReadCart){
        ReadSerialNumberKID(SerialNumberKid[0].BufferKid);
        counterReadCart --;
    }
    if(FlagMFRC522._newCart){
        
        for(jj = 1; ( jj < 11 && SerialNumberKid[jj].SerialKid != SerialNumberKid[0].SerialKid); jj++);
        return jj;
    }
    return jj;
}
