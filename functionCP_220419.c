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


fildFlagBitsPcb2 FildFlagsPcb2[12];   // ���. 25.03.22

fildFlagBitsPcb2 TempEventRead;
fildFlagBitsPcb2 TempEventWrite;

UINT16  ValueVdd[2] = {0,0};
UINT8   StatusPwr[2] = {0,0};

UINT16    CurrentEventPage = 0;   // ������� �������� ������ ��� ������ �������
UINT16    CurrentEventByte = 0;   // ������� ���� �� �������� ������ ��� ������ �������
UINT16  CurrentEventWrite;
void SaveCurrentPosition(void){
    MainMemoryPageToBuffer(2,2046);
    WriteToBufferAt45(2,ADDR_CURRENT_PAGE,2,(unsigned char*)&CurrentEventPage);
    WriteToBufferAt45(2,ADDR_CURRENT_BYTE,2,(unsigned char*)&CurrentEventByte);
    BufferToMainMemoryPageE(2,2046);
}
unsigned int ReadCurrentPositionAt45(void){
    // ��������� �������� � ������������� ����������� � �����
    MainMemoryPageToBuffer(2,2046);
    // ������ ����� �������� 
    ReadFromBufferAt45(2,ADDR_CURRENT_PAGE,2,(unsigned char*)&CurrentEventPage);
    // ������ ����� ���������� ������������ �����
    ReadFromBufferAt45(2,ADDR_CURRENT_BYTE,2,(unsigned char*)&CurrentEventByte);
    if(CurrentEventPage >= MAX_EVENT_PAGE){
        CurrentEventPage = 0;
        CurrentEventByte = 0;
        WriteToBufferAt45(2,ADDR_CURRENT_PAGE,2,(unsigned char*)&CurrentEventPage);
        WriteToBufferAt45(2,ADDR_CURRENT_BYTE,2,(unsigned char*)&CurrentEventByte);
        BufferToMainMemoryPageE(2,2046);
    }
    // ������������ ������� �������� ������ � �����
    MainMemoryPageToBuffer(1,CurrentEventPage);
    // ����������� CurrentEventWrite
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
// ���. 07.04.22++++++++++++++
void SaveModeRs485(unsigned char mode485){
    MainMemoryPageToBuffer(2,2047);
    WriteToBufferAt45(2,250,1,&mode485);
    BufferToMainMemoryPageE(2,2047);
    ControlFlagCP.CurrentModeRs = ReadCharFromAt45(2047,250);//
}
//++++++++++++++++++++++++++++++++
// ���. 29.03.22#include    "iButton.h"
EventStruct BufferWriteEvent;
UINT8 tempDirect; // ��������� ���������� � ������� ����������� �������� �����������
void SaveEvent(UINT8 TypeEvent) {
    unsigned char *pcc; // ��������� �� ���
    unsigned int*   pii; // ��������� �� uint
    if(!TypeEvent) return; // ��� ������ ������� ������ �� ���������
    // ��������� ����� ������� � ������ ������ 7 ����
    BufferWriteEvent.Year = CurrentTime.Year;     //0,1 ����
    BufferWriteEvent.Month = CurrentTime.Month;   //2 ����
    BufferWriteEvent.Day = CurrentTime.Day;       //3 ����
    BufferWriteEvent.Hour = CurrentTime.Hour;     //4 ����
    BufferWriteEvent.Minute = CurrentTime.Minute; //5 ����
    BufferWriteEvent.Second = CurrentTime.Second; //6 ����
    // ��������� ������ � ������� � ������ ������
    switch(TypeEvent){
        case 100:case 101:// ������� ��������� ������� ��
            BufferWriteEvent.SerialNumber = 0;    //8...11 ����
            BufferWriteEvent.IdNet = 0;                  //12,13 ����
            BufferWriteEvent.QuantityBos = 0; // 14,15 ����
            BufferWriteEvent.Direct = 0;                    //7 ����
            BufferWriteEvent.Situation = 0;          //16 ����
            BufferWriteEvent.ErrByte = 0;  //17 ����
            BufferWriteEvent.StatusAutoByte = 0;   //18 ����
            BufferWriteEvent.ControlByte = 0;  //19 ����
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 102:   // ������� �������� ����� �� � ������ ��
            BufferWriteEvent.Direct = SelectedDirection;                    //7 ����
            BufferWriteEvent.SerialNumber = StatusBU[SelectedDirection].SerialNumber;    //8...11 ����
            BufferWriteEvent.IdNet = 0;                  //12,13 ����
            BufferWriteEvent.QuantityBos = 0; // 14,15 ����
            BufferWriteEvent.Situation = 0;          //16 ����
            BufferWriteEvent.ErrByte = 0;  //17 ����
            BufferWriteEvent.StatusAutoByte = 0;   //18 ����
            BufferWriteEvent.ControlByte = 0;  //19 ����
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 103: // ������� ������ �� �� ������ ��
            BufferWriteEvent.Direct = SelectedDirection;                    //7 ����
            BufferWriteEvent.SerialNumber = TempSerialNumberBUR;//StatusBU[SelectedDirection].SerialNumber;    //8...11 ����
            BufferWriteEvent.IdNet = 0;                  //12,13 ����
            BufferWriteEvent.QuantityBos = 0; // 14,15 ����
            BufferWriteEvent.Situation = 0;          //16 ����
            BufferWriteEvent.ErrByte = 0;  //17 ����
            BufferWriteEvent.StatusAutoByte = 0;   //18 ����
            BufferWriteEvent.ControlByte = 0;  //19 ����
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 104: //  ������� ���������� ���������� ������� ��
            BufferWriteEvent.SerialNumber = 0;    //8...11 ����
            BufferWriteEvent.IdNet = ValueVdd[0];                  //12,13 ����
            BufferWriteEvent.QuantityBos = ValueVdd[1]; // 14,15 ����
            BufferWriteEvent.Direct = 0;                    //7 ����
            BufferWriteEvent.Situation = 0;          //16 ����
            BufferWriteEvent.ErrByte = 0;  //17 ����
            BufferWriteEvent.StatusAutoByte = 0;   //18 ����
            BufferWriteEvent.ControlByte = 0;  //19 ����
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 105:    //������� ����� ������
            BufferWriteEvent.SerialNumber = AccessPassword; // ����� ������
            pii = (unsigned int*)&OldAccessPassword;        
            BufferWriteEvent.IdNet = *pii++;                // ������ ������
            BufferWriteEvent.QuantityBos = *pii;
            break;
        case 106: // ������� ������� ����� ���
            BufferWriteEvent.reserv = TempNumberKid;
            pcc = (unsigned char*)&BufferWriteEvent.SerialNumber;// ���������� ��������� �� ������ ������ ������
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[0];
            pcc ++;
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[1];
            pcc ++;
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[2];
            pcc ++;
            *pcc = SerialNumberKid[TempNumberKid].BufferKid[3];
            break;
        case 1: // ����������� ����� � ��
            BufferWriteEvent.Direct = StatusBU[0].Direct;                    //7 ����
            BufferWriteEvent.SerialNumber = StatusBU[0].SerialNumber;    //8...11 ����
            BufferWriteEvent.IdNet = 0;                  //12,13 ����
            BufferWriteEvent.QuantityBos = StatusBU[0].QuantityBos; // 14,15 ����
            BufferWriteEvent.Situation = 0;          //16 ����
            BufferWriteEvent.ErrByte = 0;  //17 ����
            BufferWriteEvent.StatusAutoByte = 0;   //18 ����
            BufferWriteEvent.ControlByte = 0;  //19 ����
// ��������� ���������� � ���� ���/����
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 2: // �������� ����� � ��
            BufferWriteEvent.Direct = TempDir;// ����k;                    //7 ����
            BufferWriteEvent.SerialNumber = StatusBU[TempDir/* ���� k*/].SerialNumber;    //8...11 ����
            BufferWriteEvent.IdNet = 0;                  //12,13 ����
            BufferWriteEvent.QuantityBos = 0; // 14,15 ����
            BufferWriteEvent.Situation = 0;          //16 ����
            BufferWriteEvent.ErrByte = 0;  //17 ����
            BufferWriteEvent.StatusAutoByte = 0;   //18 ����
            BufferWriteEvent.ControlByte = 0;  //19 ����
            BufferWriteEvent.reserv = 0;            // 21
            break;
        case 3:case 4:case 5:// ������� �� ���������� ������������ ��
            BufferWriteEvent.Direct = StatusBU[0].Direct;                    //7 ����
            BufferWriteEvent.SerialNumber = StatusBU[0].SerialNumber;    //8...11 ����
            BufferWriteEvent.IdNet = StatusBU[0].IdNet;                  //12,13 ����
            BufferWriteEvent.QuantityBos = StatusBU[StatusBU[0].Direct].QuantityBos; // 14,15 ����
            BufferWriteEvent.Situation = StatusBU[0].Situation;          //16 ����
            BufferWriteEvent.ErrByte = StatusBU[0].FlagErrRoom.ErrByte;  //17 ����
            BufferWriteEvent.StatusAutoByte =StatusBU[0].StatusAuto.StatusAutoByte;   //18 ����
            BufferWriteEvent.ControlByte = StatusBU[0].FlagControlDirect.ControlByte;  //19 ����
// ���. 28.03.22    // ��������� ���������� � ���� ���/����                
            BufferWriteEvent.reserv = FildFlagsPcb2[0].ByteFlagRoom_Pcb2; 
            
 //====================
            break;
        case 6:             // �������� ������� "�����"
        case 7: case 8:     // �������� ������ "����"
        case 14: case 15:   // ������� ������ ������ ��������������������� �� ��
            // ��������� �����������
            BufferWriteEvent.Direct = StatusBU[TempDirectRs].Direct;
            // ��������� ����� ������������ �����
            BufferWriteEvent.reserv = TempNumberKid;
            break;
    }
    BufferWriteEvent.TypeEvent  = TypeEvent;     // 20
    T3CONbits.TON = 0; // ���. 18.08.20 ��������� ������� �� ����� ������ �������
    // ���������� ����� ������� � ��������������� ������� � ������ AT45
    WriteToBufferAt45(1,CurrentEventByte,SIZE_EVENT,(unsigned char *)&BufferWriteEvent);
//==============
    BufferToMainMemoryPageE(1,CurrentEventPage);// ����� ���������� ���������� ��� � �������� �������� ������
//================
    // �������� ����� �������  � ������ AT45
    CurrentEventByte += SIZE_EVENT;
    if(CurrentEventByte >= SIZE_PAGE){
        CurrentEventByte = 0;
        BufferToMainMemoryPageE(1,CurrentEventPage);// ����� ���������� ���������� ��� � �������� �������� ������
        CurrentEventPage ++;// �������� ����� �������� ������ � ������� ����� ������������� ������
        if(CurrentEventPage >= MAX_EVENT_PAGE){
            CurrentEventPage = 0;
        }
        SaveCurrentPosition();//========= ���������� ������ ��� � ������ ����� � ������ ���. 191003
        MainMemoryPageToBuffer(1,CurrentEventPage);// ������ ����� �������� � �����
    }
//=============
    SaveCurrentPosition();//========= ���������� ������ ��� � ������ ����� � ������ ���. 191003
//==============
    T3CONbits.TON = 1; // ���. 18.08.20 ��������� ������� ����� ������ �������
    CurrentEventWrite = (CurrentEventPage * 12) + (CurrentEventByte / 22); // ������ ������ �������� ������� ��� ������
    return;   // ����� ������� ���� ��������� � ������
}
// ������� ������������ ������� ���������� ��� ��������� � ��� �� �����������
// � TempCommandRs � TempDirectRs ����������� �������� ������ � �����������
void CommandRs( UINT8 Cmnd,UINT8 dir){
    TempCommandRs = Cmnd;
    TempDirectRs = dir;
}
// ���������� � ������� ������� ������������ ��
statusAuto IndicationStatusAuto[11];
void SetStatusIndictionDoor(UINT8 status,UINT8 dir){   // �������� ��������� ����� ��� �������� �� �������
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
                /*�������*/if(LcdFlag.Debug)xprintf("dir %u hands\r",dir);
                return 1;
            }
        }
    }
    /*�������*/if(LcdFlag.Debug)xprintf("dir %u auto\r", dir);
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
        // ������� ���������� ����������
        SaveEvent(3);
        Interval._1min = 1;
    }
    if(StatusBU[dir].FlagErrRoom.ErrByte != StatusBU[0].FlagErrRoom.ErrByte){
        StatusBU[dir].FlagErrRoom.ErrByte = StatusBU[0].FlagErrRoom.ErrByte;
        // ������� ����������� ������ �������� � �����������
        SaveEvent(4);
        Interval._1min = 1;
    }
// ���. 25.03.22    
    if(StatusBU[dir].SerialNumber > 2111000){
        if(FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 != FildFlagsPcb2[0].ByteFlagRoom_Pcb2){
            FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 = FildFlagsPcb2[0].ByteFlagRoom_Pcb2;
            // ������� ���������� ����� �������� � pcb2
// ����� �������� ������ �������           
            SaveEvent(4);
            Interval._1min = 1;
        }
    }else{
        FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 = 0;
    }
//============    
// ���. 31.08.22 
    if(StatusBU[dir].StatusAuto.StatusAutoByte != StatusBU[0].StatusAuto.StatusAutoByte){
        UINT8 resultCheck;
        UINT8 CurrentModeAuto = StatusBU[dir].StatusAuto.StatusAutoByte & 0b11000000; // ��������� ������� �������� ������ ����������
        UINT8 NewModeAuto = StatusBU[0].StatusAuto.StatusAutoByte & 0b11000000; //��������� ����� �������� ������ ����������
        
        /*�������*/if(LcdFlag.Debug)xprintf("dir %u current %x -> new %x\r",dir,CurrentModeAuto,NewModeAuto);
//        if(CurrentModeAuto != NewModeAuto){
            
        switch(NewModeAuto){ // ������������� � ����������� �� ������ ������ ������
            case AUTO:
// ���������� ����� � ������������ � ��������� ���������� ������ ������  
                if(CurrentModeAuto == LOCK){ // ���� ������� ����� ����������
                    SetStatusIndictionDoor(0,dir);
                    /*�������*/if(LcdFlag.Debug)xprintf("Com 4 cpopy\r");
                    CommandRs(4,dir);
                }else{ // ���� ������� ����� ������
                // ����� �������� �������� ������� ��� � ��������� ������� 
                    /*�������*/if(LcdFlag.Debug)xprintf("Com 9 copy\r");
                    CommandRs(9,dir);
                    SetStatusIndictionDoor(0,dir);
                    resultCheck = CheckStatusDoor(dir);
                    if(!resultCheck){
                       
                    }
                }
                break;
            case LOCK:
// ���������� ����� � ������������ � ��������� ���������� ������ ������                  
                SetStatusIndictionDoor(1,dir);
                /*�������*/if(LcdFlag.Debug)xprintf("Com 5 copy\r");
                CommandRs(5,dir);
                break;
            case MANUAL:
                SetStatusIndictionDoor(1,dir);
                /*�������*/if(LcdFlag.Debug)xprintf("Com 8 copy\r");
                CommandRs(8,dir);
                break;
        }
        // ������� ��������� ����� ������ ����������
        SaveEvent(5);
    }
    
//++++++++++++++    
    StatusBU[dir].StatusAuto.StatusAutoByte = StatusBU[0].StatusAuto.StatusAutoByte;
    StatusBU[dir].FlagControlDirect.ControlByte = StatusBU[0].FlagControlDirect.ControlByte;
    CounterDelayStart[dir] = CounterDelayStart[0]; // ���. 28.07.21  ��������� ������� �������� ������� �������� �����
// ���. 29.08.22    CounterDelayDoor[dir] = CounterDelayDoor[0]; // ���. 28.07.21  ��������� ������� �������� ������� �������� �����
}
EventStruct BufferReadEvent;
void ReadEvent(UINT16 NumberEv){
    UINT16 pageNumber;
    UINT16 byteNumber;
    // �� ������ ������� ������������ ����� �������� ������ � ����� ����� �� ��������
    pageNumber = NumberEv / 12;
    byteNumber = (NumberEv % 12) * 22;
    MainMemoryArrayRead(pageNumber,byteNumber,22,(unsigned char*)&BufferReadEvent);
}
#define TIMING_BOS  1
static UINT8    StageCheckBU;// ���� �� ������� � ������ ������ ��������� ��������
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
        return 0; // ���� �� ���� ����� ���� ������ �������
    }
    if(StatusBU[direct].SerialNumber){ // ���� �� ���������������
        if(ConnectBUR[direct]){ // ���� � �� ����������� �����
            if(direct != SelectedDirection || !Interval._1min){   // ���. 19.11.19 ���� ������������� ���������� �����
                switch (StageCheckBU){
                    default:
                        StageCheckBU = 0;
                        break;
                    case 0:
                        RequestedSerial = StatusBU[direct].SerialNumber;
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                        RsTxLength = sprintf((char*)RsTxBuffer,"? %lu 2 ",StatusBU[direct].SerialNumber);
                        RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // ��������� ����������� ����� 
                        RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                        TransmittRsPacket();
                        TempDirectRs = direct; // 30/09/22��������� ����� �����������
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                        CounterDelayRs = 50; // ���. 30.01.20 CounterDelayRs �������� �� 1 �� ������� � 10��
                        WaitData = 0;   // ��������� ���� ������
                        StageCheckBU ++;
                        break;
                    case 1:
                        TerminalRs();   // ������� ������ 1000 ��
                        if(!CounterDelayRs) {
                            StageCheckBU ++;
                        }
                        break;
                    case 2:
                        if(WaitData){
                            ConnectBUR[direct] = 1;
                            CounterNoConnect[direct] = MaxNumberRequests; // ���. 06.04.22;
                            CopyUnuonStatusBu(direct);/* ���� ����� �������
* ��������� ������ � �������� ������� ��������������� ����������� */
                        }else{
                            if(CounterNoConnect[direct]){
                                CounterNoConnect[direct] --;
                            }else{
                                 ConnectBUR[direct] = 0;
                                // ������� ������ ����� � ��
                                TempDir = direct;
                                SaveEvent(2); 
                                // ����� ��������� ����� ���
                                CommandRs(128,direct);
                            }
                        }
                        StageCheckBU = 0;
                        Interval._CheckStatusBU = 0; // �������� ����� �������������� ��
                        break;
                }
                
                
            }else{      // ���� ������������� ��������� �����
                switch (StageCheckBU){
                    default:
                        StageCheckBU = 0;
                        break;
                    case 0:
                        ClrStatusBos();
                        RequestedSerial = StatusBU[direct].SerialNumber;
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
                        RsTxLength = sprintf((char*)RsTxBuffer,"? %lu 3 ",StatusBU[direct].SerialNumber);
                        RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // ��������� ����������� ����� 
                        RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                        TransmittRsPacket();
                        TempDirectRs = direct; // 30/09/22��������� ����� �����������
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                        CounterDelayRs = 50; // ���. 30.01.20 CounterDelayRs �������� �� 1 �� ������� � 10��
                        WaitData = 0;   // ��������� ���� ������
                        CounterErrBos = 0;
                        CounterAlarmBos = 0;
                        CounterFireDevice = 0;
                        CounterAttentionDevice = 0;
                        CounterStartDevice = 0;
                        StageCheckBU ++;
                        StatusBos[0].AlarmByte = 0; // ���� ����� ������ � ���
                        NewEventLcdFlag.NewDataBos = 0; // ������� ���� ���������� � ��� ��������
                        break;
                    case 1:
                           // ������� ������ 100 ��
                        if(CounterDelayRs == 0) {
                            StageCheckBU ++;
                        }
                        break;
                    case 2:
                         /* ���� ������������� ����� ������������ ����� 
                                                * ������ ���� ����� ������ ����� 1*/
                        if(WaitData){      /* ���� ����� �������
    * ��������� ������ � �������� ������� ��������������� ����������� 
    * � ������������ ����� �������� ��� ������ ���������� � ������������ ���*/
                            CopyUnuonStatusBu(direct);
                            CounterDelayRs = (StatusBU[direct].QuantityBos * TIMING_BOS) + 1;  /* */
                            StageCheckBU ++;
                        }else{
                            if(CounterNoConnect[direct]){
                                CounterNoConnect[direct] --;
                            }else{
                                ConnectBUR[direct] = 0;
                                // ������� ������ ����� � ��
                                TempDir = direct;
                                SaveEvent(2); 
                                CommandRs(128,direct);
                            }
                            StageCheckBU = 0;
                            Interval._CheckStatusBU = 0; // �������� ����� �������������� ��
                        }
                        break;
                    case 3:
                        TempDir = direct;
                        TerminalRs();   // ������� ���������� � ���
                        if(!CounterDelayRs ) {  // ����� �������� �����������
                            ConnectBUR[direct] = 1;
                            StageCheckBU = 0;
                            Interval._CheckStatusBU = 0; // �������� ����� �������������� ��
                            Interval._1min = 0;
                            NewEventLcdFlag.NewDataBos = 1; // ���������� ���� ���������� � ��� ��������
                        }
                        break;
                }
            }
        }else{  // ���� � �� ����� �� �����������
            switch (StageCheckBU){
                default:
                    StageCheckBU = 0;
                    break;
                case 0:
                    RequestedSerial = StatusBU[direct].SerialNumber;
//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss                        
                    RsTxLength = sprintf((char*)RsTxBuffer,"? %lu 1 ",StatusBU[direct].SerialNumber);
                    RsTxCrc = Crc8((unsigned char*)RsTxBuffer,RsTxLength); // ��������� ����������� ����� 
                    RsTxLength = RsTxLength + sprintf((char*)&RsTxBuffer[RsTxLength],"%u\r",RsTxCrc);
                    TransmittRsPacket();
//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
                    while(ENTX485 || !ENTX485_I);
                    CounterDelayRs = 50; // ���. 30.01.20 CounterDelayRs �������� �� 1 �� ������� � 10��
                    WaitData = 0;   // ��������� ���� ������
                    StageCheckBU ++;
                    break;
                case 1:
                       // ������� ������ 1000 ��
                    if(CounterDelayRs == 0) {
                        StageCheckBU ++;
                    }
                    break;
                case 2:
                     /* ���� ������������� ����� ������������ ����� 
                                            * ������ ���� ����� ������ ����� 1*/
                    if(WaitData){      /* ���� ����� �������
* ��������� ������ � �������� ������� ��������������� ����������� 
* � ������������ ����� �������� ��� ������ ���������� � ������������ ���*/
                        StatusBU[direct].SerialNumber = StatusBU[0].SerialNumber;
                        StatusBU[direct].QuantityBos = StatusBU[0].QuantityBos;
                        // ������� ����������� ����� � ��
//                        NewEvent[direct] = 1;
                        SaveEvent(1);
                        StageCheckBU ++;
                        CounterDelayRs = (StatusBU[0].QuantityBos * TIMING_BOS) + 1;  /* */
                    }else{
                        ConnectBUR[direct] = 0;
                        // ���� ����� �� �������
                        StageCheckBU = 0;
                        Interval._CheckStatusBU = 0; // �������� ����� �������������� ��
                    }
                    break;
                case 3:
                    TempDir = direct;
                    TerminalRs();   // ������� ���������� � ���
                    if(!CounterDelayRs ) {  // ����� �������� �����������
                        ConnectBUR[direct] = 1;
                        CounterNoConnect[direct] = MaxNumberRequests; // ���. 06.04.22;
                        StageCheckBU = 0;
                        Interval._CheckStatusBU = 0; // �������� ����� �������������� ��
                    }
                    break;
            }
        }
    }else{
        Interval._CheckStatusBU = 0; // �������� ����� �������������� ��
    }
    if(!Interval._CheckStatusBU){
        CounterCheckBU ++;
        if(CounterCheckBU > 5){// ���. 19.11.19
            CounterCheckBU = 0;
        }
        return 1; // ��� �� �������� ����� �������� ����������
    }
    return 0;
}
UINT8 kk;

void SetButtonColor(UINT8 page, UINT8 dir, UINT16 color) {
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.bco=%u���", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    
}
void SetButtonFont(UINT8 page, UINT8 dir, UINT16 color, UINT16 font) {
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.font=%u���", page, dir, font);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.b%u.pco=%u���", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
void SetTextColor(UINT8 page, UINT8 dir, UINT16 color) {// ��� 22.11.2019
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.bco=%u���", page, dir, color);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    
}
#define NO_REG_         0        //up   67 doun 66
#define NO_CONNECT_     1       //      71      72
#define NORMA_          2       //      75      74
#define ERROR_          3       //      79      78
#define ALARM_          4       //      83      82
void SetKeyPic (UINT8 page, UINT8 dir, UINT8 status){
    UINT8 picture;
    // �������� �������� ��� ������
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
    sprintf(LcdBufferData,"page%u.b%u.pic=%u���", page, dir, picture);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
void SetTextPic(UINT8 page, UINT8 dir, UINT16 picture) {// ��� 22.05.2020
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.pic=%u���", page, dir, picture);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
}
void SetTextFont(UINT8 page, UINT8 dir, UINT16 color, UINT16 font) {
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.font=%u���", page, dir, font);
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t%u.pco=%u���", page, dir, color);
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
        // ������� �� ������� ����� ��������� ����������
// ���. 01.09.22
        if(IndicationStatusAuto[dir].StatusAutoByte & 0b11000000){
            SetAutoDispley(dir,80);
        }else{
            SetAutoDispley(dir,76);
        }
        if(StatusBU[dir].Direct && ((StatusBU[dir].Situation & 0x03) != 2)){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page10.b%u.txt=\"%u\"���", dir, StatusBU[dir].Direct); // �������� ����� ��������� // ���. 24.03.22
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }
        switch (StatusBU[dir].Situation & 0x03) { // ���. 191115
            case 0: // �������� �����
                if (StatusBU[dir].FlagErrRoom.ErrByte /* ���. 25.03.22*/ || (FildFlagsPcb2[dir].ByteFlagRoom_Pcb2 & 0x07)) { // ���. 191115
                    SetKeyPic (MAIN_MENU, dir, ERROR_);  // ���.200522
                    SetButtonFont(MAIN_MENU, dir, BLACK, 4);
                } else {
                    SetKeyPic (MAIN_MENU, dir, NORMA_);  // ���.200521
                    SetButtonFont(MAIN_MENU, dir, BLACK, 4);
                }
                break;
            case 1: // �������� ��������
                SetKeyPic (MAIN_MENU, dir, ALARM_);  // ���.200522
                SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                break;
            case 2: // �������� �����
                SetKeyPic (MAIN_MENU, dir, ALARM_);  // ���.200522
                // ���. 02.08.21 ���� SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                SetButtonFont(MAIN_MENU, dir, BLACK, 3);
                UINT16 tempCounterDelay = CounterDelayStart[dir];/* ���. 29.08.22 + CounterDelayDoor[dir];*/
                if(ClassAlgoritm[dir] < 11){
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.b%u.txt=\"%u\"���", dir, tempCounterDelay); // �������� ����� �������� �����
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                }else{
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.b%u.txt=\"%u\"���", dir, StatusBU[dir].Direct); // �������� ����� ��������� 
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                // ����� ���. 02.08.21
                break;
            case 3: // �������� ����
                SetKeyPic (MAIN_MENU, dir, ALARM_);  // ���.200522
                SetButtonFont(MAIN_MENU, dir, BLACK, 6);
                break;
        }
    }else{
        SetAutoDispley(dir,72);
        SetKeyPic (MAIN_MENU, dir, NO_CONNECT_);  // ���.200521
        SetButtonFont(MAIN_MENU, dir, BLACK, 4);
    }
}
char lcdline[100];
// ���. 08.04.22+++++++++++++++++++++
void IndicationStatusRs(void){
    if(CurrentScreen != 10)return;
    if(LcdFlag.NewPage){
        if(ControlFlagCP.CurrentModeRs == 1){
            ControlFlagCP.RsBreakNew = 0;   // 16.09.22
            ControlFlagCP.RsBreakLast = 0;  // 16.09.22
            if(!ControlFlagCP.NoBurNew){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=96���");// ��������� �����
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=97���");// ����������� �����
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }else{
            if(!ControlFlagCP.RsBreakNew){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=94���");// ��������� ������
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=95���");// ����������� ������
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
                sprintf(LcdBufferData,"page10.t17.pic=96���");// ��������� �����
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t17.pic=97���");// ����������� �����
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }
        return;
    }
    if(ControlFlagCP.RsBreakLast != ControlFlagCP.RsBreakNew){
        ControlFlagCP.RsBreakLast = ControlFlagCP.RsBreakNew;
        if(!ControlFlagCP.RsBreakLast){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"t17.pic=94���");// ��������� ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }else{
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"t17.pic=95���");// ����������� ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }
    }
}
//+++++++++++++++++++++
void PrintDirectionNumber(UINT8 screen){
    while(TxRunRs || TxRunLcd);
    switch(screen){
        case 7: case 11: case 12: case 16: case 17:
            sprintf(LcdBufferData,"page%u.t4.txt=\"%u\"���",screen,SelectedDirection);
//            printf("%s",LcdBufferData); /*�������*/if(LcdFlag.Debug)xprintf("%s\r",LcdBufferData);
            break;
        case 14 :case 18:case 19:
            sprintf(LcdBufferData,"page%u.t4.txt=\"%u\"���",screen,StatusBU[SelectedDirection].Direct);
//            printf("%s",LcdBufferData); /*�������*/if(LcdFlag.Debug)xprintf("%s\r",LcdBufferData);
            break;
        default:
            return;
    }
     printf("%s",LcdBufferData); /*�������*/if(LcdFlag.Debug)xprintf("%s\r",LcdBufferData);
}
UINT8   GroupNumber;
UINT16  DeviceNumber;
// ������� �� ����� ���������� � �����������
void IndicatorDirection(UINT8 dir){
    UINT8 nn;
    switch(CurrentScreen){
        case MAIN_MENU:
// ���������� ����� ������ ��� ������ ������ �� ����
            if(LcdFlag.NewPage){
                for(nn = 1;nn < 11;nn ++){
                    PrintDirectionPage10(nn);
                }
                while(TxRunRs || TxRunLcd);
                if(ControlFlagCP.CP_Sound_off){ //���. 04.04.22
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=56���");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }else{
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=57���");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                if(ControlFlagCP.ErrorRip){     // 30.09.22
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t13.pic=56���");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }else{
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t13.pic=57���");
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                printf("%s",LcdBufferData);
                IndicationStatusRs();   // ���. 08.04.22
                LcdFlag.NewPage = 0;
            }
            IndicationStatusRs();       // ���. 08.04.22
            PrintDirectionPage10(dir);
            break;
        case 11:
//..................................            
            if(LcdFlag.NewPage || LcdFlag.NewData || NewEventLcdFlag.NewStaus || NewEventLcdFlag.NewSituation){
// ������� ���������� � ��������� ���                
// ������������� �������� ����
                if(LcdFlag.Debug)xprintf("LcdFlag ox%04X, NewEventLcd ox%04X\r",LcdFlag.LcdFlagsWord,NewEventLcdFlag.NewEventLcdWord);
                PrintDirectionNumber(CurrentScreen);
                while(TxRunRs || TxRunLcd);
                if(StatusBU[SelectedDirection].Situation & 0x03){// � ����������� ���� ��������� ��������� 
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page11.t1.pic=35���"); // �������� �������
                }else{
                    if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* ���. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page11.t1.pic=34���"); // �������� ������
                    }else{
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page11.t1.pic=33���"); // �������� �������
                    }
                }
                printf("%s",LcdBufferData);
// ������� ���������� � �������� ����� ����
                while(TxRunRs || TxRunLcd);
                switch(StatusBU[SelectedDirection].Situation & 0x03){
                    case 0:
                        if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* ���. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
                            while(TxRunRs || TxRunLcd);
                            sprintf(lcdline,"%s","�������������");
                        }else{
                            sprintf(lcdline,"%s","�����");
                        }
                        break;
                    case 1:
                        sprintf(lcdline,"%s","��������");
                        break;
                    case 2:
                        sprintf(lcdline,"%s","�����");
                        break;
                    case 3:
                        sprintf(lcdline,"%s","����");
                        break;
                }
                sprintf(LcdBufferData,"page11.t1.txt=\"����\\r%s\\r%s �.�. %lu\\r����������������\\r���:  %u ��.\"���"
                        ,lcdline,((StatusBU[SelectedDirection].SerialNumber%1000) < 501)? "���":"���",
                        StatusBU[SelectedDirection].SerialNumber,StatusBU[SelectedDirection].QuantityBos);
                printf("%s",LcdBufferData);
// ������� ���������� � ��������� ���
                if(StatusBU[SelectedDirection].QuantityBos){
                    while(TxRunRs || TxRunLcd);
                    if(StatusBos[0].AlarmByte){
                        // � ����������� ���� ��������� ��������� 
                        sprintf(LcdBufferData,"page11.t2.pic=35���"); // �������� �������
                    }else{
                        if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0x03) ){
                            sprintf(LcdBufferData,"page11.t2.pic=34���"); // �������� ������
                        }else{
                            sprintf(LcdBufferData,"page11.t2.pic=33���"); // �������� �������
                        }
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// ������� ���������� ��� �� ��������, ������ � �������������
                    while(TxRunRs || TxRunLcd);
//                    sprintf(LcdBufferData,"page11.t2.txt=\"��� ��������:\\r� ������ - %u\\r�� �������� - %u\\r� ������������� - %u\"���"
//                        ,CounterFireDevice,CounterAttentionDevice,CounterErrBos);
                    sprintf(LcdBufferData,"page11.t2.txt=\"��� ��������:\\r� ����� - %u\\r� ������ - %u\\r�� �������� - %u\\r� ������������� - %u\"���"
                        ,CounterStartDevice,CounterFireDevice,CounterAttentionDevice,CounterErrBos);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);

                }
//***************
//  t0 ������� ������ ���������� �� ����������
// ���. 01.09.22                    
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                        case    AUTO:
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.pic=28���"); // �������� �������
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.txt=\"���������� ��������\"���");
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            break;
                        case    LOCK:
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.pic=31���"); // �������� ������
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.txt=\"���� ������������\"���");
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            break;
                        case    MANUAL:
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page11.t0.pic=31���t0.txt=\"����� ������ ������\"���");
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
// � ����������� ���� ������������������ ��
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page12.t2.txt=\"%s\"���",((StatusBU[SelectedDirection].SerialNumber%1000) < 501)?"���":"���");
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page12.t1.txt=\"%lu\"���",StatusBU[SelectedDirection].SerialNumber);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                if(ConnectBUR[SelectedDirection]){// ���� ���������� �����������
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page12.t1.pco=%u���",BLACK);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//  29.09.22 ������ �� ���                  printf("t10.pic=57���"); // RS485 � �����
                    // ����������� ��������� ���������
                    if(!(StatusBU[SelectedDirection].Situation & 0x0F)){
                        // ���������� ����� � ��� ���������� ��������
                        if(/*!StatusBU[SelectedDirection].FlagErrRoom.ErrByte && 14.11.22*/
                        !StatusBU[SelectedDirection].StatusAuto.Manual &&
                        !StatusBU[SelectedDirection].StatusAuto.StopStart &&
                        !IndicationStatusAuto[SelectedDirection].Manual){
                            // ��� ��������
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.pic=28���");
                        }else{
                            // ���� ������������� ��� ���������� ������������� ��� ������� �����
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.pic=31���");
                        }
                    }else{
                    // �������� ����� ��� ����
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page12.t0.pic=30���");
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// ���. 01.09.22                    
                    while(TxRunRs || TxRunLcd);
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000 ){
                        case AUTO:
                            if(IndicationStatusAuto[SelectedDirection].Manual){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t0.txt=\"����� ������ ������\"���");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t0.txt=\"�������� ����� %u ������\"���",(StatusBU[SelectedDirection].StatusAuto.CounterDelay10s + 1) * 10);
                            }
                            break;
                        case LOCK:
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.txt=\"���� ������������\"���");
                            break;
                        case MANUAL:
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t0.txt=\"����� ������ ������\"���");
                            break;
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// ���������� ��������������  
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.Pwr2 && !StatusBU[SelectedDirection].FlagErrRoom.Pwr1){
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page12.t11.pic=57���");
                    }else{
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page12.t11.pic=56���");
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if((StatusBU[SelectedDirection].SerialNumber%1000) < 501){
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.noteALARM){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t12.pic=57���");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t12.pic=56���");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.IPR){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t13.pic=57���");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t13.pic=56���");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.SMK){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t14.pic=57���");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t14.pic=56���");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!StatusBU[SelectedDirection].FlagErrRoom.noteAUTO){
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t15.pic=57���");
                        }else{
//                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page12.t15.pic=56���");
                        }
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// ���. 25.03.22        
                        if(StatusBU[SelectedDirection].SerialNumber > 2111000){
                            while(TxRunRs || TxRunLcd);
                            if(!FildFlagsPcb2[SelectedDirection].Err_Udp){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t16.pic=57���");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t16.pic=56���");
                            }
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            if(!FildFlagsPcb2[SelectedDirection].Err_Uvoa){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t17.pic=57���");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t17.pic=56���");
                            }
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                            while(TxRunRs || TxRunLcd);
                            if(FildFlagsPcb2[SelectedDirection].Rip){ // 29.09.22 ��������� ������������� ��� �� 12 �����
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t10.pic=56���");
                            }else{
//                                while(TxRunRs || TxRunLcd);
                                sprintf(LcdBufferData,"page12.t10.pic=57���");
                            }
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        }
//============                        
                    }
                }else{
//  29.09.22 ������ �� ���                                      printf("t10.pic=56���");
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page12.t1.pco=%u���",GREY_B);
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
        // � ����������� ���� ������������������ ��
            if(ConnectBUR[SelectedDirection]){
                while(TxRunRs || TxRunLcd);
                switch(StatusBU[SelectedDirection].Situation){
                    case 0:
                        //����������� ���������
                        if(StatusBU[SelectedDirection].StatusAuto.StopStart || StatusBU[SelectedDirection].StatusAuto.Manual || StatusBU[SelectedDirection].FlagErrRoom.ErrByte){
//                            printf("page14.t0.pic=31���");
                            sprintf(LcdBufferData,"page14.t0.pic=31���");
                        }else{
//                            printf("page14.t0.pic=28���");
                            sprintf(LcdBufferData,"page14.t0.pic=28���");
                        }
                        break;
                    case 1:case 2:case 3:
//                        printf("page14.t0.pic=30���");
                        sprintf(LcdBufferData,"page14.t0.pic=30���");
                        break;
                }
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// ���. 01.09.22
                while(TxRunRs || TxRunLcd);
                switch (StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                    case AUTO:
//                        printf("page14.t0.txt=\"���������� ��������\"���");
                        sprintf(LcdBufferData,"page14.t0.txt=\"���������� ��������\"���");
                        break;
                    case LOCK:
//                        printf("page14.t0.txt=\"���� ������������\"���");
                        sprintf(LcdBufferData,"page14.t0.txt=\"���� ������������\"���");
                        break;
                    case MANUAL:
//                        printf("page14.t0.txt=\"����� ������ ������\"���");
                        sprintf(LcdBufferData,"page14.t0.txt=\"����� ������ ������\"���");
                        break;
                }
                printf("%s",LcdBufferData);
            }else{
                while(TxRunRs || TxRunLcd);
//                printf("page14.t0.pic=29���");
                sprintf(LcdBufferData,"page14.t0.pic=29���");
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
//                printf("page14.t0.txt=\"��� ������\"���");
                sprintf(LcdBufferData,"page14.t0.txt=\"��� ������\"���");
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
// ������� ���������� � ��������� ���                
// ������������� �������� ����
                if(LcdFlag.Debug)xprintf("LcdFlag ox%04X, NewEventLcd ox%04X\r",LcdFlag.LcdFlagsWord,NewEventLcdFlag.NewEventLcdWord);
                PrintDirectionNumber(CurrentScreen);
                while(TxRunRs || TxRunLcd);
                if(StatusBU[SelectedDirection].Situation & 0x03){// � ����������� ���� ��������� ��������� 
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page16.t0.pic=105���"); // �������� �������
                }else{
                    if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* ���. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page16.t0.pic=106���"); // �������� ������
                    }else{
//                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page16.t0.pic=104���"); // �������� �������
                    }
                }
                printf("%s",LcdBufferData);
// ������� ���������� � �������� ����� ����
                while(TxRunRs || TxRunLcd);
                switch(StatusBU[SelectedDirection].Situation & 0x03){
                        case 0:
                            if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0xFC)/* ���. 25.03.22*/|| (FildFlagsPcb2[SelectedDirection].ByteFlagRoom_Pcb2 & 0x07)){
//                                while(TxRunRs || TxRunLcd);
                                sprintf(lcdline,"%s","�������������");
                            }else{
                                sprintf(lcdline,"%s","�����");
                            }
                            break;
                        case 1:
                            sprintf(lcdline,"%s","��������");
                            break;
                        case 2:
                            sprintf(lcdline,"%s","�����");
                            break;
                        case 3:
                            sprintf(lcdline,"%s","����");
                            break;
                    }
                sprintf(LcdBufferData,"page16.t0.txt=\"���\\r%s\\r%s �.�. %lu\\r����������������\\r���:  %u ��.\"���"
                        ,lcdline,((StatusBU[SelectedDirection].SerialNumber%1000) < 501)? "���":"���",
                        StatusBU[SelectedDirection].SerialNumber,StatusBU[SelectedDirection].QuantityBos);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                // ������� ���������� � ��������� ���
                if(StatusBU[SelectedDirection].QuantityBos){
                    while(TxRunRs || TxRunLcd);
                    if(StatusBos[0].AlarmByte){
                        // � ����������� ���� ��������� ��������� 
                        sprintf(LcdBufferData,"page16.t1.pic=105���"); // �������� �������
                    }else{
                        if((StatusBU[SelectedDirection].FlagErrRoom.ErrByte & 0x03) ){
                            sprintf(LcdBufferData,"page16.t1.pic=106���"); // �������� ������
                        }else{
                            sprintf(LcdBufferData,"page16.t1.pic=104���"); // �������� �������
                        }
                    }
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
// ������� ���������� ��� �� ��������, ������ � �������������
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page16.t1.txt=\"��� ��������:\\r� ������ - %u\\r�� �������� - %u\\r� ������������� - %u\"���"
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
                sprintf(LcdBufferData,"page17.t2.txt=\"%s\"���",((StatusBU[SelectedDirection].SerialNumber%1000) < 501)?"���":"���");
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page17.t1.txt=\"%lu\"���",StatusBU[SelectedDirection].SerialNumber);
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page17.t1.pco=%u���",BLACK);
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                if(!StatusBU[SelectedDirection].Situation){
                        // ���������� ����� � ��� ���������� ��������
                        if(!StatusBU[SelectedDirection].FlagErrRoom.ErrByte){
                            // ��� ��������
                            sprintf(LcdBufferData,"page17.t0.pic=28���");
                        }else{
                            // ���� �������������
                            sprintf(LcdBufferData,"page17.t0.pic=31���");
                        }
                }else{
                // �������� ����� ��� ����
                    sprintf(LcdBufferData,"page17.t0.pic=30���");
                }
    // ���������� ��������������            
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                if(!StatusBU[SelectedDirection].FlagErrRoom.Pwr2 && !StatusBU[SelectedDirection].FlagErrRoom.Pwr1){
                    sprintf(LcdBufferData,"page17.t11.pic=57���");
                }else{
                    sprintf(LcdBufferData,"page17.t11.pic=56���");
                }
                printf("%s",LcdBufferData);
                if((StatusBU[SelectedDirection].SerialNumber%1000) < 501){
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.noteALARM){
                        sprintf(LcdBufferData,"page17.t12.pic=57���");
                    }else{
                        sprintf(LcdBufferData,"page17.t12.pic=56���");
                    }
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.IPR){
                        sprintf(LcdBufferData,"page17.t13.pic=57���");
                    }else{
                        sprintf(LcdBufferData,"page17.t13.pic=56���");
                    }
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.SMK){
                        sprintf(LcdBufferData,"page17.t14.pic=57���");
                    }else{
                        sprintf(LcdBufferData,"page17.t14.pic=56���");
                    }
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    if(!StatusBU[SelectedDirection].FlagErrRoom.noteAUTO){
                        sprintf(LcdBufferData,"page17.t15.pic=57���");
                    }else{
                        sprintf(LcdBufferData,"page17.t15.pic=56���");
                    }
                    printf("%s",LcdBufferData);
                    if(StatusBU[SelectedDirection].SerialNumber > 2111000){
                        while(TxRunRs || TxRunLcd);
                        if(!FildFlagsPcb2[SelectedDirection].Err_Udp){
                            sprintf(LcdBufferData,"page17.t16.pic=57���");
                        }else{
                            sprintf(LcdBufferData,"page17.t16.pic=56���");
                        }
                        printf("%s",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(!FildFlagsPcb2[SelectedDirection].Err_Uvoa){
                            sprintf(LcdBufferData,"page17.t17.pic=57���");
                        }else{
                            sprintf(LcdBufferData,"page17.t17.pic=56���");
                        }
                        printf("%s",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        if(FildFlagsPcb2[SelectedDirection].Rip){ // 29.09.22 ��������� ������������� ��� �� 12 �����
                            sprintf(LcdBufferData,"page17.t10.pic=56���");
                        }else{
                            sprintf(LcdBufferData,"page17.t10.pic=57���");
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
                        sprintf(LcdBufferData,"page18.b%u.pic=%u���",i,resulT);
                        printf("%s",LcdBufferData);
                    }
                }
                NewEventLcdFlag.NewDataBos = 0; // ������� ���� ���������� � ��� ��������
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
                    sprintf(LcdBufferData,"page19.b%u.txt=\"%u\"���",i,number);
                    printf("%s",LcdBufferData);
//                    sprintf(LcdBufferData,"page19.t%u.txt=%u���",i,resulT);
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
                        sprintf(LcdBufferData,"page19.b%u.pic=%u���",i,resulT);
                        printf("%s",LcdBufferData);
                    }
                }
                NewEventLcdFlag.NewDataBos = 0; // ������� ���� ���������� � ��� ��������
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
        default: // �������� ������ ������������������ ���
            if(StatusBos[index].SerialNumber){
                LcdFlag.Yes = 1;
            }
            break;
        case 2: // �������� ������ ��������� ���
            if(!CounterAlarmBos) {
                LcdFlag.Select = 1;
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt3.val=0���",paGe);// ���. 30.01.23
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt1.val=1���",paGe);// ���. 30.01.23
                printf("%s",LcdBufferData);
                break;
            }else{
                if(StatusBos[index].AlarmByte && StatusBos[index].SerialNumber){
                    LcdFlag.Yes = 1;
                }
            }
            break;
        case 3: // �������� ������ ����������� ���
            if(!CounterErrBos) {
                LcdFlag.Select = 1;
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt2.val=0���",paGe);// ���. 30.01.23
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.bt1.val=1���",paGe);// ���. 30.01.23
                printf("%s",LcdBufferData);
                break;
            }else{
                if(StatusBos[index].StatusByte && StatusBos[index].SerialNumber){
                    LcdFlag.Yes =1;
                }
            }
            break;
        case 4: // �������� ���� ���
            LcdFlag.Yes = 1;
            break;
    }
}
void DisplayStatusBOS1202(unsigned char page,UINT16 deviceNumber){
    UINT16 Quantity;
    if(CurrentScreen != page)return; // ���� �� �� �� ��� ������ ������
// ���� ����� �����
    if (LcdFlag.NewPage){
        IndexBos = deviceNumber;
        while(TxRunRs || TxRunLcd);
        Quantity = StatusBU[SelectedDirection].QuantityBos;
        sprintf(LcdBufferData,"page%u.t13.txt=\"%u\"���",page,StatusBU[SelectedDirection].QuantityBos);// ���. 24.03.22
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        if (!StatusBos[IndexBos].SerialNumber && StatusBU[SelectedDirection].QuantityBos){
            LcdFlag.HandUp = 1;
        }
        LcdFlag.NewPage = 0;
        if(CurrentScreen == 20){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"vis t2,1���");
            printf("%s",LcdBufferData);
        }
        return;
    }
    /*�������*/if(LcdFlag.Debug)xprintf("IndexBos = %u\r",IndexBos);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t14.txt=\"%u\"���",page,CounterErrBos);// ���. 24.03.22
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    while(TxRunRs || TxRunLcd);
    sprintf(LcdBufferData,"page%u.t15.txt=\"%u\"���",page,CounterAlarmBos);// ���. 24.03.22
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    //������� ������ ��� ������ ���
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
    /*�������*/if(LcdFlag.Debug)xprintf("NewEventLcdFlag.NewDataBos = %u\r",NewEventLcdFlag.NewDataBos);
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
    // ������� ����� ������
    while(TxRunRs || TxRunLcd);
    switch(page){
        default:
            return;
            break;
        case 13:
            sprintf(LcdBufferData,"page%u.t6.txt=\"��� N %u\"���",page,index);
            break;
        case 20:
            sprintf(LcdBufferData,"page%u.t6.txt=\"��� N %u\"���",page,index);
            break;
    }
    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
    if (!StatusBos[index].SerialNumber) {
        // ��� �� ���������������
        // ������� �.�.
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t5.txt=\"�������� N XXXXXXXXX\"���",page);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // ������� ��������� �����
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t0.pic=56���",page); //������ ������
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t7.txt=\"��� ������\"���",page);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // ������� ��������� �������
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t1.pic=56���",page); //������ ������
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t8.txt=\"��� ������\"���",page);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
 //.........       
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t6.bco=%u���",page,GREY_B);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            // ������� ��������� ��1 ��� ������� ������
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t2.pic=56���",page); //������ ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t9.txt=\"��� ������\"���",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//''''''''''      
        if(page == 13){
        // ������� ��������� ��2
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t3.pic=56���",page); //������ ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t10.txt=\"��� ������\"���",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // ������� ��������� ����������
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t4.pic=56���",page); //������ ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t11.txt=\"��� ������\"���",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        }
    }else {
        // ��� ���������������
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page%u.t5.txt=\"�������� N %lu\"���",page, StatusBos[index].SerialNumber);
        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        if (StatusBos[index].StatusByte == 0xFF) {
    // � ��� ��� �����
//.........       
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t6.bco=%u���",page,GREY_BB);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//.........   
        // ������� ��������� �����
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t0.pic=56���",page); //������ ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t7.txt=\"��� �����\"���",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            // ������� ��������� �������
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t1.pic=56���",page); //������ ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t8.txt=\"��� ������\"���",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            // ������� ��������� ��1 ��� ������� ������       
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t2.pic=56���",page); //������ ������
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t9.txt=\"��� ������\"���",page);
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            if(page == 13){
        // ������� ��������� ��2
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t3.pic=56���",page); //������ ������
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t10.txt=\"��� ������\"���",page);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
        // ������� ��������� ����������
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t4.pic=56���",page); //������ ������
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t11.txt=\"��� ������\"���",page);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        } else {
            // ����� � ��� � �����
            // ������� ��������� �����
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t0.pic=57���",page); //������ ������
            printf("%s",LcdBufferData); 
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page%u.t7.txt=\"����� � �����\"���",page);
            printf("%s",LcdBufferData); 
            // �������� ��������� �������
//......................... 
            while(TxRunRs || TxRunLcd);
            if(StatusBos[index].AlarmByte){
//                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t6.bco=%u���",page,RED);
//                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            } else {
                if(StatusBos[index].StatusByte){
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t6.bco=%u���",page,YELOW_B);
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }else{
//                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t6.bco=%u���",page,WHITE);
//                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
//.........................
            if (StatusBos[index].StausPwr) {
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t1.pic=56���",page); //������ ������
                printf("%s",LcdBufferData); 
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page%u.t8.txt=\"������\"���",page);
                printf("%s",LcdBufferData); 
            } else {
                if (StatusBos[index].StatusReservPwr) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t1.pic=56���",page); //������ ������
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t8.txt=\"���������\"���",page);
                    printf("%s",LcdBufferData); 
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t1.pic=57���",page); //������ �����
                    printf("%s",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t8.txt=\"��������\"���",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            if(page == 20){
                // �������� ������� ������
                if (StatusBos[index].StatusLoopRT0) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=56���",page); //������ ������
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t9.txt=\"������� ������\\r����������\"���",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=57���",page); //������ �����
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t9.txt=\"������� ������\\r��������\"���",page);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
            if(page == 13){
            // �������� ������������ 1
                if (StatusBos[index].StatusLoopRT0) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=56���",page); //������ ������
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if (StatusBos[index].StatusLoopRT0 == 1) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t9.txt=\"��1 �����\"���",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                    if (StatusBos[index].StatusLoopRT0 == 2) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t9.txt=\"��1 �.�.\"���",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t2.pic=57���",page); //������ �����
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t9.txt=\"��1 ��������\\r����������� %dC\"���",page,StatusBos[index].TemperaturaSensor[0]/10);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            // �������� ������������ 2
                if (StatusBos[index].StatusLoopRT1) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t3.pic=56���",page); //������ ������
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if (StatusBos[index].StatusLoopRT1 == 1) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t10.txt=\"��1 �����\"���",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                    if (StatusBos[index].StatusLoopRT1 == 2) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t10.txt=\"��1 �.�.\"���",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t3.pic=57���",page); //������ �����
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t10.txt=\"��2 ��������\\r����������� %dC\"���",page,StatusBos[index].TemperaturaSensor[1]/10);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
                // �������� ����������
                if (StatusBos[index].AlarmByte != 3) {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.bco=%u���",page, WHITE);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.pco=%u���",page, BLACK);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    if (StatusBos[index].Activator) {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t4.pic=56���",page); //������ ������
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        if (StatusBos[index].Activator == 1) {
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page%u.t11.txt=\"����� ����������\"���",page);
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        }
                        if (StatusBos[index].Activator == 2) {
                            while(TxRunRs || TxRunLcd);
                            sprintf(LcdBufferData,"page%u.t11.txt=\"�.�. ����������\"���",page);
                            printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        }
                    } else {
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t4.pic=57���",page); //������ �����
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t11.txt=\"��������� �����\"���",page);
                        printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    }
                } else {
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t4.pic=56���",page); //������ ������
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.bco=%u���",page, BORDO);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.pco=%u���",page, WHITE);
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t11.txt=\"���� ������\"���",page);
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

// ������� ������ ������
double tempFloat;
static UINT16 NumberEvRead = 0;
static statusAuto TempAutoByte;
static FlagErrBits TempErrByte;
void PrintEventLCD(void) {
    // �������� ����� ���� � ����� �������
    while(TxRunRs || TxRunLcd);
    printf("page8.t5.txt=\"%u\"���",NumberEvRead);
    while(TxRunRs || TxRunLcd);
    printf("page8.t100.txt=\"%02u.%02u.%u\"���",BufferReadEvent.Day,BufferReadEvent.Month,BufferReadEvent.Year);
    while(TxRunRs || TxRunLcd);
    printf("page8.t103.txt=\"%02u:%02u:%02u\"���",BufferReadEvent.Hour,BufferReadEvent.Minute,BufferReadEvent.Second);
    switch (BufferReadEvent.TypeEvent) {
        case 1:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"����������� �����\\r����������� %u\\r%s �.�. %lu\\r����������������� ���\\r%u ��.\"���", BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber, BufferReadEvent.QuantityBos);
            break;
        case 2:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"�������� �����\\r� ����������� %u\\r%s �.�. %lu\"���", BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber);
            break;
        case 3:
            TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
            switch(BufferReadEvent.Situation & 0x03){
                case 0:
                    sprintf(lcdline,"%s","�����");
                    break;
                case 1:
                    sprintf(lcdline,"%s","��������");
                    break;
                case 2:
                    sprintf(lcdline,"%s","�����");
                    break;
                case 3:
                    sprintf(lcdline,"%s","����");
                    break;
                default:
                    sprintf(lcdline,"%u",BufferReadEvent.Situation);
                    break;
            }
            while(TxRunRs || TxRunLcd);
            if(TempEventRead.classIP < 11){
                printf("page8.t3.txt=\"���� ��������� ����������\\r����������� %u\\r%s �.�. %lu\\r%s\"���",
                BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber,&lcdline[0]);
            }else{
                printf("page8.t3.txt=\"��� ��������� ����������\\r����������� %u\\r%s �.�. %lu\\r%s\"���",
                BufferReadEvent.Direct,(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber,&lcdline[0]);
            }
           
            break;
        case 4:
            TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
            while(TxRunRs || TxRunLcd);
            if(TempEventRead.classIP < 11){
                printf("page8.t3.txt=\"���� ��������� ���������\\r\"���");
            }else{
                printf("page8.t3.txt=\"��� ��������� ���������\\r\"���");
            }
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=t3.txt+\"%s %lu �������. %u\\r\"���",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
            TempErrByte.ErrByte = BufferReadEvent.ErrByte;
            if(TempErrByte.ErrByte || (BufferReadEvent.reserv & 0x07)){
                UINT16 Flag0D = 0;
                while(TxRunRs || TxRunLcd);
                printf("page8.t3.txt=t3.txt+\"�������������: \"���");
                while(TxRunRs || TxRunLcd);
                if (TempErrByte.ConnectBos) {
                    printf("page8.t3.txt=t3.txt+\"������ �����.\\r\"���");
                }else{
                    printf("page8.t3.txt=t3.txt+\"\\r\"���");
                }
                if (TempErrByte.Bos) {
                    while(TxRunRs || TxRunLcd);
                    if(TempEventRead.classIP < 11){
                        printf("page8.t3.txt=t3.txt+\"���. \"���");
                    }else{
                        printf("page8.t3.txt=t3.txt+\"���. \"���");
                    }
                    Flag0D = 1;
                }
                if (TempErrByte.IPR) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"�� ��. \"���");
                    Flag0D = 1;
                }
                if (TempErrByte.SMK) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"�� ������.\"���");
                    Flag0D = 1;
                }
                if(Flag0D){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"\\r\"���");
                    Flag0D = 0;
                }
                if (TempErrByte.noteALARM){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"�� �� ��. \"���");
                    Flag0D = 1;
                }
                if (TempErrByte.noteAUTO) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"�� �� ����.\"���");
                    Flag0D = 1;
                }
                if(Flag0D){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"\\r\"���");
                    Flag0D = 0;
                }
                if(BufferReadEvent.reserv & 0x01){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"�� ���. \"���");
                }
                if(BufferReadEvent.reserv & 0x02){
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"�� ����. \"���");
                }
                if (TempErrByte.Pwr1 || TempErrByte.Pwr2) {
                    while(TxRunRs || TxRunLcd);
                    printf("page8.t3.txt=t3.txt+\"���. ���.\"���");
                }
            }else{
                while(TxRunRs || TxRunLcd);
                printf("page8.t3.txt=t3.txt+\"������ ��������\"���");
            }
            break;
        case 5: // ���. 28.01.20
            TempAutoByte.StatusAutoByte = BufferReadEvent.StatusAutoByte;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"%s %lu �������. %u\\r\"���",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
            while(TxRunRs || TxRunLcd);
            switch(TempAutoByte.StatusAutoByte & 0b11000000){
                case AUTO:
                    printf("page8.t3.txt=t3.txt+\"���������� ��������\"���");
                    break;
                case LOCK:
                    printf("page8.t3.txt=t3.txt+\"���������� �������������\"���");
                    break;
                case MANUAL:
                    printf("page8.t3.txt=t3.txt+\"����� ������ ������\"���");
                    break;
            }
            break;
        case 6:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"����������� %u\\r�������� ������� �����\\r����������� ���� N%u\"���",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 7:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"����������� %u\\r�������� ������� �����\\r����������� ���� N%u\"���",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 8:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"��� ����\\r�������� ������� �����\\r����������� ���� N%u\"���",BufferReadEvent.reserv);
            break;
        case 14: //  ������ ������ ���������� �����������
            // ���������� �������������� ������� �� ��
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"����������� %u\\r���������� ��������������\\r����������� ���� N%u\"���",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 15:
            // ���������� ������������� ������� �� ��
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"����������� %u\\r���������� �������������\\r����������� ���� N%u\"���",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 100:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"�������� ������� ��\"���");// 19 ���.
        break;
        case 101:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"�������� ����� � ����\"���");//21
            break;
        case 102:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"������ %u\\r�������� %s %lu\"���",BufferReadEvent.Direct ,(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber);// 21 + 15
            break;
        case 103:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"������ %u\\r������ %s %lu\"���",BufferReadEvent.Direct ,(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber);// 21 + 15
            break;
        case 104:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"��������� ������� ��\\r\"���"); // 20
            tempFloat = (float) (BufferReadEvent.IdNet);
            tempFloat = tempFloat / 100;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt==t3.txt+\"PWR1 %0.2f�\\r\"���", tempFloat);
            tempFloat = (float) (BufferReadEvent.QuantityBos);
            tempFloat = tempFloat / 100;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=t3.txt+\"PWR2 %0.2f�\"���", tempFloat);
            break;
        case 105:
            OldAccessPassword =  BufferReadEvent.QuantityBos << 8;
            OldAccessPassword = OldAccessPassword + BufferReadEvent.IdNet;
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"��������� ������\\r������ %lu\\r����� %lu\"���",OldAccessPassword,BufferReadEvent.SerialNumber); // 20
            break;
        case 106:
            while(TxRunRs || TxRunLcd);
            printf("page8.t3.txt=\"��������������� ��� %u\\r%lu\"���",BufferReadEvent.reserv,BufferReadEvent.SerialNumber);
            break;
        default:
            while(TxRunRs || TxRunLcd);
            if (BufferReadEvent.TypeEvent == 255) {
                printf("page8.t3.txt=\"��� �������\"���");
            } else {
                printf("page8.t3.txt=\"������� %u �� ��������\"���",BufferReadEvent.TypeEvent);
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
        ReadEvent(NumberEvRead);    // ������ ������� �� AT45
        PrintEventLCD();            // �������� ��������� �� �����
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
        ReadEvent(NumberEvRead);    // ������ ������� �� AT45
        if(BufferReadEvent.TypeEvent != 255){
            // ����� ��������� ���������� �� ���� �������
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
                    printf("page8.t4.bco=%u���", WHITE);
                }
            }else{
                if(!LcdFlag.No){
                    LcdFlag.No = 1;
                    printf("page8.t4.bco=%u���", YELOW);
                }
            }
        }else{
            if(!LcdFlag.No){
                LcdFlag.No = 1;
                printf("page8.t4.bco=%u���", YELOW);
            }
        }
    }
    if(LcdFlag.Yes ){
        PrintEventLCD();            // �������� ��������� �� �����
        CounterInactiv ++;
        if(!(CounterInactiv % 30)){
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page0.Inactiv1.val=60���"); // ������ ����� ������ �� 0 ����� ��� ���������� ��������� ���������
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
            case 100://��������� ������� ��
                xprintf("��������� ������� ��\r");
                break;
            case 101://�������� ����� � ����
                xprintf("�������� ����� � ����\r");
                break;
            case 102://�������� ��
                xprintf("������ %u ", BufferReadEvent.Direct);
                xprintf("�������� %s %lu\r", (BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber); // ���
                break;
            case 103://������ ��
                xprintf("������ %u ", BufferReadEvent.Direct);
                xprintf("������ %s %lu\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber);
                break;
            case 104://���. ���������� ������� ��
                xprintf("���. ���������� ������� �� ");
                xprintf("PWR1 %u,%u� ", BufferReadEvent.IdNet/100,BufferReadEvent.IdNet%100);
                xprintf("PWR2 %u,%u�\r", BufferReadEvent.QuantityBos/100,BufferReadEvent.QuantityBos%100);
                break;
            case 105: // 16.09.22
                OldAccessPassword =  BufferReadEvent.QuantityBos << 8;
                OldAccessPassword = OldAccessPassword + BufferReadEvent.IdNet;
                xprintf("��������� ������ ������ %lu ����� %lu\r",OldAccessPassword,BufferReadEvent.SerialNumber);
                break;
            case 106:
                xprintf("��������������� ���-%u %lu\r",BufferReadEvent.reserv,BufferReadEvent.SerialNumber);
                break;
            case 1://���. ����� � ��
                xprintf("���. ����� � %s %lu ",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber);
                xprintf("����������� %u ", BufferReadEvent.Direct);
                xprintf("����������������� %u ���\r", BufferReadEvent.QuantityBos);
                break;
            case 2://�������� ����� � ��
                xprintf("�������� ����� � %s %lu ", (BufferReadEvent.SerialNumber%1000) < 501?"���":"���",BufferReadEvent.SerialNumber);
                xprintf("����������� %u\r", BufferReadEvent.Direct);
                break;
            case 3://��������� ����������
                TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
                switch (BufferReadEvent.Situation) {
                    case 0:
                        sprintf(lcdline,"%s","�����");
                        break;
                    case 1:
                        sprintf(lcdline,"%s","��������");
                        break;
                    case 2:
                        sprintf(lcdline,"%s","�����");
                        break;
                    case 3:
                        sprintf(lcdline,"%s","����");
                        break;
                }
                if(TempEventRead.classIP < 11){
                    xprintf("���� %s %lu �������. %u ���������� %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct,&lcdline[0]);
                }else{
                    xprintf("��� %s %lu �������. %u ���������� %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct,&lcdline[0]);
                }
                break;
            case 4://��������� ��������� ������������
                TempEventRead.ByteFlagRoom_Pcb2 = BufferReadEvent.reserv;
                xprintf("��������� ��������� ");
                xprintf("%s %lu �������. %u\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���", BufferReadEvent.SerialNumber, BufferReadEvent.Direct);
                TempErrByte.ErrByte = BufferReadEvent.ErrByte;
                if(TempErrByte.ErrByte || BufferReadEvent.reserv){
                    if (TempErrByte.ConnectBos) {
                        if(TempEventRead.classIP < 11){
                            xprintf("�������� ����� � ���\r");
                        }else{
                            xprintf("�������� ����� � ���\r");
                        }
                    }
                    if (TempErrByte.Bos) {
                        if(TempEventRead.classIP < 11){
                            xprintf("������������� ���\r");
                        }else{
                            xprintf("������������� ���\r");
                        }
                    }
                    if (TempErrByte.IPR) {
                        xprintf("������������� �� ��\r");
                    }
                    if (TempErrByte.SMK) {
                        xprintf("������������� �� ������\r");
                    }
                    if (TempErrByte.noteALARM) {
                        xprintf("������������� �� �� �������\r");
                    }
                    if (TempErrByte.noteAUTO) {
                        xprintf("������������� �� �� ����������\r");
                    }
                    if (TempErrByte.Pwr1) {
                        xprintf("������������� ��������� ������� ���\r");
                    }
// ���. 28.03.22                    
                    if(BufferReadEvent.SerialNumber > 2111000){
                        if(BufferReadEvent.reserv & 0x01){
                            xprintf("������������� �� ���\r"); // ������������� �� ���
                        }
                        if(BufferReadEvent.reserv & 0x02){
                            xprintf("������������� �� ����\r");// ������������� �� ����
                        }
                    }
//================                    
                }else{
                     xprintf("������������ �����\r");
                }
                break;
            case 5:// ���������� ��������� �����������
                TempAutoByte.StatusAutoByte = BufferReadEvent.StatusAutoByte;
                if(!TempAutoByte.Manual && !TempAutoByte.StopStart){
                    sprintf(lcdline,"%s","���������� ���������"); // ���. 01.09.22
                    // ���������� ��������, ����� �������
                }else{
                    if(TempAutoByte.StopStart){
                        sprintf(lcdline,"%s","���������� �������������");
                        // ���������� ��������������
                    }
                    if(TempAutoByte.Manual){
                        sprintf(lcdline,"%s","����� ������ ������"); // ���. 01.09.22
                        // ����� �������
                    }
                }
                xprintf("%s %lu ����������� %u %s\r",(BufferReadEvent.SerialNumber%1000) < 501?"���":"���",
                            BufferReadEvent.SerialNumber,BufferReadEvent.Direct,&lcdline[0]);
                break;
        case 6:// ������� �����
            xprintf("����������� %u, �������� ������� �����, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 7:// ������� ���� �� �����������
            xprintf("����������� %u, �������� ������� �����, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 8:// �� ������� ����
            xprintf("��� ����, �������� ������� �����, ����������� ���� N%u\r",BufferReadEvent.reserv);
            break;
        case 14: //  ������ ������ ���������� �����������
            // ���������� �������������� ������� �� ��
            xprintf("����������� %u, ���������� ��������������, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        case 15:
            // ���������� ������������� ������� �� ��
            xprintf("����������� %u, ���������� �������������, ����������� ���� N%u\r",BufferReadEvent.Direct,BufferReadEvent.reserv);
            break;
        default:
            Nop();
            xprintf("������� %u �� ��������\r", BufferReadEvent.TypeEvent);
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
            sprintf(LcdBufferData,"page9.t2.txt=\"USB �� �����\"���");
            LED_USB_REDY = 1;
        }else{
            sprintf(LcdBufferData,"page9.t2.txt=\"USB �����\"���");
            LED_USB_REDY = 0;
        }
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page0.Inactiv1.val=600���"); // ������ ����� ������ �� 0 ����� ��� ���������� ��������� ���������
        printf("%s",LcdBufferData);
        LcdFlag.Select =0;
    }
    // ���. 27.01.20
    if (LED_USB_REDY == USB_EN) {
        // ���������� ��������� ����������� USB
        while(TxRunRs || TxRunLcd);
        if (USB_EN) {
            // USB ���������
            if (!LcdFlag.Select) {
                sprintf(LcdBufferData,"page9.t2.txt=\"USB �����\"���");
            }
        } else {
            // USB �� ���������
            if (!LcdFlag.Select) {
                sprintf(LcdBufferData,"page9.t2.txt=\"USB �� �����\"���");
            } else {
                sprintf(LcdBufferData,"click bt0,1���");
                printf("%s",LcdBufferData);
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page9.t2.txt=\"�������� ������ ��������\"���");
                LcdFlag.Select = 0;
            }
            printf("%s",LcdBufferData);
        }
        LED_USB_REDY = !USB_EN;
    }
    //����� ���. 27.01.20
    if (LcdFlag.Select && !LED_USB_REDY) {
        if (NumberEvRead == (CurrentEventWrite - 1)) {
            while(TxRunRs || TxRunLcd);
            sprintf(LcdBufferData,"page9.t2.txt=\"�������� ������ �� ��\"���");
            printf("%s",LcdBufferData);
        }
        if(!ENTX485 && ENTX485_I){
            ReadEvent(NumberEvRead);
            while(!Interval.usb50ms)continue;
            CounterInactiv ++;
            if(!(CounterInactiv % 100)){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page0.Inactiv1.val=60���"); // ������ ����� ������ �� 0 ����� ��� ���������� ��������� ���������
                printf("%s",LcdBufferData);
            }
            PrintEventUSB();// ���. 27.01.20
            if (NumberEvRead) {
                NumberEvRead--;
            } else {
                NumberEvRead = MAX_QUANTITY_EVENT;
            }
        }
    }
    if (NumberEvRead == CurrentEventWrite && LcdFlag.Select) {
        // ������ ��������� ��������� ������� ������
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page9.bt0.val=0���");
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page9.bt0.txt=\"��������\"���");
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page9.t2.txt=\"�������� ������ ���������\"���");
        xprintf("�������� ������ ���������\r");
        printf("%s",LcdBufferData);
        while(TxRunRs || TxRunLcd);
        sprintf(LcdBufferData,"page0.Inactiv1.val=60���"); // ������ ����� ������ �� 0 ����� ��� ���������� ��������� ���������
        printf("%s",LcdBufferData);
        LcdFlag.Select = 0;
    }
}
static UINT16 LastIntegral;
IntegralUnion IntegralRegistr;
// ���. 06.04.22 +++++++++++++++
UINT8 CheckIntegralStatus(void){
    UINT8 k;
    UINT8 result = 0;
    ControlFlagCP.NoBurNew = 0;
// ���.06.04.22 �������� ������� ����� �� ����� ���������
    for(k = 1;k < 11; k ++){
        if(StatusBU[k].SerialNumber){
            if(!ConnectBUR[k]) {
                ControlFlagCP.NoBurNew = 1;
                result ++; // ���� ������� ������
            }else{
                if(StatusBU[k].FlagErrRoom.ErrByte || (FildFlagsPcb2[k].ByteFlagRoom_Pcb2 & 0x07)) result ++; // ���� � ������� ���� �������������
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
// ���.12.07.22
void CheckIntegralAuto(void){
    UINT8 k;
    LED_AUTO = 0;
    for(k = 1; k < 11; k ++){
        if(StatusBU[k].StatusAuto.Manual || StatusBU[k].StatusAuto.StopStart){
            LED_AUTO = 1;
        }
    }
}
//����� ���.12.07.22     
void IndicationControl(void){
    static UINT8    CounterIndication;
    static UINT8    PeriodIndication;
    
    if(!Interval._SoundCtrl)return;
    IntegralRegistr.IntegralStatus = CheckIntegralStatus();
    IntegralRegistr.IntegralSituation = CheckIntegralSituation();
    CheckIntegralAuto(); // ���. 12.07.22
// ���. 18.10.22
    if(LastIntegral != IntegralRegistr.Integral){
        /*�������*/if(LcdFlag.Debug)xprintf("last %u new %u\r",LastIntegral,IntegralRegistr.Integral);
        if(IntegralRegistr.Integral > LastIntegral){ // ���� ��������� ����� �������������
            /*�������*/if(LcdFlag.Debug)xprintf("sound on\r");
            ControlFlagCP.CP_Sound_off = 0;
            if(CurrentScreen == 10){
                while(TxRunRs || TxRunLcd);
                sprintf(LcdBufferData,"page10.t12.pic=57���");
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }           
        LastIntegral = IntegralRegistr.Integral;
        
    }
    Interval._SoundCtrl = 0;
    if(!IntegralRegistr.Integral && !ControlFlagCP.RsBreakLast/* ���. 08.04.22 */&& !ControlFlagCP.ErrorRip/* ���. 13.04.22*/&& !ErrPwr){
        EXT_SOUND = SOUND = 0;
        LED_ERROR = 1;
        OUT_ERROR = 1;
        LED_FIRE = 0;
        OUT_FIRE = 0;
        LED_START = 0;
        OUT_START = 0;
        return;
    }
// ��� 25.03.22
    if(IntegralRegistr.IntegralStatus || ControlFlagCP.RsBreakLast/* ���. 08.04.22 */ || ControlFlagCP.ErrorRip/* ���. 13.04.22*/|| ErrPwr){
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
// ��� 25.03.22            LED_ERROR = 0;
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
void CheckKeyAuto(void){ // ���. 22.08.22 
    if(!KEY_AUTO ){// ���� ������ ������ ����������&& COUNTER_KEY_AUTO < 300
        if(COUNTER_KEY_AUTO < 300){
            COUNTER_KEY_AUTO ++;
        }else{
            if(!FlagKey.KeyAutoLong){
                if(COUNTER_KEY_AUTO == 300){
                    COUNTER_KEY_AUTO ++;
                    FlagKey.KeyAutoLong = 1;
                    /*�������*/if(LcdFlag.Debug)xprintf("Long auto\r");
                }
            }
        }
    }else{
        if(COUNTER_KEY_AUTO > 10 && COUNTER_KEY_AUTO < 300){
            if(!FlagKey.KeyAutoShort){
                FlagKey.KeyAutoShort = 1; // ���� ������������ ������� 100...3000 ��
                /*�������*/if(LcdFlag.Debug)xprintf("Short auto\r");
            }
        }
        if(COUNTER_KEY_AUTO){
            COUNTER_KEY_AUTO = 0;
        }
    }
}
void CheckKeySound(void){ // ���. 25.08.22 
    if(!KEY_SOUND ){// ���� ������ ������ ���� && COUNTER_KEY_SOUND < 300
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
                FlagKey.KeySoundShort = 1; // ���� ������������ ������� 100...3000 ��
//                xprintf("Short\r");
            }
        }
        if(COUNTER_KEY_SOUND){
            COUNTER_KEY_SOUND = 0;
        }
    }
}
void CheckKeyStart(void){ // ���. 25.08.22 
    if(!KEY_START ){// ���� ������ ������ ���� && COUNTER_KEY_START < 300
        if(COUNTER_KEY_START < 300){
            COUNTER_KEY_START ++;
        }else{
            if(!FlagKey.KeyStartLong){
                if(COUNTER_KEY_START == 300){
                    COUNTER_KEY_START ++;
                    FlagKey.KeyStartLong = 1;
                    /*�������*/if(LcdFlag.Debug)xprintf("Long start\r");
                }
            }
        }
    }else{
        if(COUNTER_KEY_START > 10 && COUNTER_KEY_START < 300){
            if(!FlagKey.KeyStartShort){
                FlagKey.KeyStartShort = 1; // ���� ������������ ������� 100...3000 ��
                 /*�������*/if(LcdFlag.Debug)xprintf("Short start\r");
            }
        }
        if(COUNTER_KEY_START){
            COUNTER_KEY_START = 0;
        }
    }
}

void KeyCheckMain(void){
    if(!Interval.Key_10ms)return;
//===================== ���������� ������ �����
    CheckKeyStart();    // ���. 25.08.22
//===================== ���������� ������ ����������    
    CheckKeyAuto();     // ���. 22.08.22  
//===================== ���������� ������ ���� 
    CheckKeySound();    // ���. 25.08.22
    Interval.Key_10ms = 0;
}
void KeyHandler(void){
    if(!FlagKey.FlagKeyStatus)return;
    NumberKID = SearchNumberKid();
    if(NumberKID > 0 && NumberKID < 11){ // ���� ���� ��������
        if(SelectedDirection > 0 && SelectedDirection < 11){// ���� ����������� �������
            if(FlagKey.KeyStartShort || FlagKey.KeyStartLong || FlagKey.KeyAutoLong || FlagKey.KeyAutoShort || FlagKey.KeySoundLong){ //���� ������ ������ ����� ��� ������ ���������
                TempDirectRs = SelectedDirection; // ��������� ����� �����������
                if(CurrentScreen == 11 || CurrentScreen == 12 || CurrentScreen == 14){
//                    printf("page%u.t0.txt=\"�������� �������\"���",CurrentScreen);
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page%u.t0.txt=\"�������� �������\"���",CurrentScreen);
                    printf("%s",LcdBufferData);
                }
                if(FlagKey.KeyStartLong){ // ������� ������� ������ �����
                    if((StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000) != LOCK){
             /*�������*/if(LcdFlag.Debug)xprintf("Com StartAll\r");
                        TempCommandRs = 7;  // ������� ��������� ����� �� ����������� ��������� � �� ���� ���������
                        SaveEvent(7); //
                    }else{
//                        printf("page%u.t0.txt=\"���� ����������\"���",CurrentScreen);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t0.txt=\"���� ����������\"���",CurrentScreen);
                        printf("%s",LcdBufferData);
                    }
                }
                if(FlagKey.KeyStartShort){ // �������� ������� ������ �����
                    if((StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000) != LOCK){
             /*�������*/if(LcdFlag.Debug)xprintf("Com StartLocal\r");
                        TempCommandRs = 10;  // ������� ��������� ����� �� ����������� ��������� � �� ���� ���������
                        SaveEvent(7); //
                    }else{
//                        printf("page%u.t0.txt=\"���� ����������\"���",CurrentScreen);
                        while(TxRunRs || TxRunLcd);
                        sprintf(LcdBufferData,"page%u.t0.txt=\"���� ����������\"���",CurrentScreen);
                        printf("%s",LcdBufferData);
                    }
                }
                if(FlagKey.KeyAutoLong){ // ������� ������� ������ ����������
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                        case AUTO:
                 /*�������*/if(LcdFlag.Debug)xprintf("Com Locking\r");
                            TempCommandRs = 5;
                            SaveEvent(15); // ������� ������ ����� ���������� ����������� �� �� 
                            break;
                        case MANUAL:
                            // ��������� ������� ���������� �������������
                 /*�������*/if(LcdFlag.Debug)xprintf("Com Locking\r");
                            TempCommandRs = 5;
                            SaveEvent(15); // ������� ������ ����� ���������� ����������� �� �� 
                            break;
                        case LOCK:
                            // ��������� ������� ���������� ��������������
                 /*�������*/if(LcdFlag.Debug)xprintf("Com Unlocking\r");
                            TempCommandRs = 4;
                            SaveEvent(14); // ������� ������ ����� ���������� ����������� �� �� 
                            break;
                    }
                }
                if(FlagKey.KeyAutoShort){ // �������� ������� ������ ����������
                    switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                        case AUTO:
                 /*�������*/if(LcdFlag.Debug)xprintf("Com Manual\r");
                            TempCommandRs = 8;
                            SaveEvent(15); // ������� ������ ����� ���������� ����������� �� �� 
                            break;
                        case MANUAL:
                 /*�������*/if(LcdFlag.Debug)xprintf("Com Auto\r");
                            TempCommandRs = 9;
                            SaveEvent(14); // ������� ������ ����� ���������� ����������� �� �� 
                            break;
                        case LOCK:
                            // ��������� ������� ���������� ��������������
                 /*�������*/if(LcdFlag.Debug)xprintf("Com Unlocking\r");
                            TempCommandRs = 4;
                            SaveEvent(14); // ������� ������ ����� ���������� ����������� �� �� 
                            break;
                    }
                }
                if(FlagKey.KeySoundLong){ // ������� ������� ������ �����
                    TempCommandRs = 6;
         /*�������*/if(LcdFlag.Debug)xprintf("Com Reset direct\r");
                }
            }
        }else{// ���� ����������� �� �������
            if(FlagKey.KeySoundShort || FlagKey.KeySoundLong){ // ���� ������ ������ ����
     /*�������*/if(LcdFlag.Debug)xprintf("Press test\r");
                if(CurrentScreen == 10 && !IntegralRegistr.IntegralSituation){
                    // 10.10.22 ����
                    while(TxRunRs || TxRunLcd);
                    WaitingScreen = 15;
                    sprintf(LcdBufferData,"page 15���");    // ������� �� ���. ����
                    printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                }
            }
        }
    }else{ // ���� ���� �� ��������
        if(FlagKey.KeySoundShort || FlagKey.KeySoundLong){ // ���� ������ ������ ����
            ControlFlagCP.CP_Sound_off = !ControlFlagCP.CP_Sound_off;
            /*�������*/if(LcdFlag.Debug)xprintf("Change sound\r");
            if(CurrentScreen == 10){    // ������� ��������� �������� ������������
                if(ControlFlagCP.CP_Sound_off){
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=56���");
                }else{
                    while(TxRunRs || TxRunLcd);
                    sprintf(LcdBufferData,"page10.t12.pic=57���");
                }
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
            }
        }
    }
    FlagKey.FlagKeyStatus = 0; // ��������� ������ ��������� ����� ���� ������
}

unsigned int ReadADC50(unsigned char AN){
    unsigned int    TempResult = 0;
    unsigned char ii = 0;
    AD1CHS = AN;
    AD1CON1bits.ADON = 1; // �������� ���
    while(ii < 50){
        AD1CON1bits.SAMP = 1; // ������ ������� ...
        while (!AD1CON1bits.DONE); // �������������� ���������?
        TempResult += ADC1BUF0;
        ii ++;
    }
    AD1CON1bits.ADON = 0; // ��������� ���
    return TempResult/50;
}
UINT8 CheckMyPwr(unsigned char ch){
    UINT16  tempAdc;
    if(!ch){
        tempAdc = ReadADC50(6);
    }else{
        tempAdc = ReadADC50(7);
    }
    // ���. 13.04.22
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
         // ������� ���������� ���������� ������� �� 1 �����
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
// ���. 01.04.22

void SavingRegisteredKid(UINT8 num){
    UINT8 pos;
    MainMemoryPageToBuffer(2,2045); // ������ �������� � ������������� ������� �� ������ AT45
    // ������������ ������� � ������� ����� �����
    pos = ((num - 1) * 4)/* +26*/;
    WriteToBufferAt45(2,pos,4,&SerialNumberKid[0].BufferKid[0]); // ���������� ������ � �����
    BufferToMainMemoryPageE(2,2045); // ��������� ����� � �������� ������
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
