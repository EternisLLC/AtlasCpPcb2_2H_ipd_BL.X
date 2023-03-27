#include    <xc.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include    <string.h>
#include    <libpic30.h>
#include    "alwaysCP_210728.h"
#include    "functionUART24.h"
#include    "FunctionRS485_K.h"
#include    "terminal.h"
#include    "InterruptTimer24.h"
#include    "crc8.h"
#include    "DriverAt45.h"
#include    "functionCP.h"
#include "DriverMFRC522cp.h"
UINT16 CounterErrRxPacket = 0;  //  ������� ������� �������� � ��������  
char LineLcd[128]={0,0,0,0,0,0,0};     //������� ����� ��� ���������
char LineRs[256];     //������� ����� ��� ���������
#define MaxCommands  30                    //������ ������� ������ (���������� ������)
unsigned int cnt,cnt2;                           //��������� �������
//���� ������ - ����� ��� ���������� ������������� ������� (��� ��������� � case)
typedef enum COMMAND_LIST_U4 {
    Unk_Nown,        //���. 27.11.19
    PAGE_CURRENT,
    CHECK_PSW,
    PAGE_NEW,
    CHANGE_PSW,
    TIME_VRF,
    TIME_CHANGE,
    NEXT_DIR,
    ADD_BU,
    DEL_BU,
    KID_READ,
    KID_SAVE,
    INC,
    DEC,
    PLAY,
    AUTO_DIR,
    RESET_DIR,
    START_DIR,
    READ_ARHIV_LCD,
    READ_ARHIV_USB,
    UPDATE,
    BRIGHT,         // ���. 24.03.22
    CHANGE_MODE_RS, // ���. 07.04.22
}CommandListU4;

//��������� ������� ��������, ������� ��������� � ��������
//� ���������, ����� ������ ����� ������� ��������� ���� �������
//������ ������ ��������� � �� ���
const CommandType   CommandsU4[MaxCommands]=
{//"�������",       "��� �������"
    {   "PageC",        PAGE_CURRENT    },
    {   "CheckPsw",     CHECK_PSW       },
    {   "NewPg",        PAGE_NEW        },
    {   "ChangePsw",    CHANGE_PSW      },
    {   "TimeVrf",      TIME_VRF        },
    {   "TimeSet",      TIME_CHANGE     },
    {   "NextDir",      NEXT_DIR        },
    {   "AddBu",        ADD_BU          },
    {   "DelBu",        DEL_BU          },
    {   "Kid",          KID_READ        },
    {   "SaveKid",      KID_SAVE        },
    {   "Inc",          INC             },
    {   "Dec",          DEC             },
    {   "Play",         PLAY            },
    {   "Auto",         AUTO_DIR        },
    {   "Rst",          RESET_DIR       },
    {   "StartMP",      START_DIR       },
    {   "Read",         READ_ARHIV_LCD  },
    {   "ReadAll",      READ_ARHIV_USB  },
    {   "Update",       UPDATE          },
    {   "Dim",          BRIGHT          },  // ���. 24.03.22
    {   "ChangeModeRs", CHANGE_MODE_RS  },  // ���. 07.04.22
};
/*
void InitTerminal (void){
    InitUart1(38400);
}*/
/*----------------------------------------------*/
/* ��������� �������� �� �������� ������        */
/*----------------------------------------------*/

int xgetsU4 (		/* 0:��� ������ ��� ���������, 1:������ �������� */
	char* buff,	/* ��������� �� ����� */
	int len		/* ������ ������ */
)
{
static int c, i;

	for (;;) {
        if (!uart4_testRx()) {
            return 0;
        }
		c = uart4_getc();				/* ������ ������ ����� �� �������� ������ */
		if (!c) return 0;               /* ����� ������? */
		if (c == '\r') break;           /* ����� ������? */
		if (c >= ' ' && i < len - 1) {	/*������� ������� */
			buff[i++] = c;
		}
	}
	buff[i] = 0;	/* ��������� � ����� ������ \0 */
    i = 0;
	return 1;
}

/*----------------------------------------------*/
/* ��������� ����� �� ������                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
	    ^                           1st call returns 123 and next ptr
	       ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/

int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;


	*res = 0;

	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */

	if (c == '-') {		/* negative? */
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x':		/* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b':		/* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1;	/* single zero */
			if (c < '0' || c > '9') return 0;	/* invalid char */
			r = 8;		/* octal */
		}
	} else {
		if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
		r = 10;			/* decimal */
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;	/* invalid char */
		}
		if (c >= r) return 0;		/* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;			/* apply sign if needed */

	*res = val;
	return 1;
}
//******************************************************************************
//**************************** M A I N *****************************************
//******************************************************************************
UINT8   TempDirectRs = 0;
UINT8   TempCommandRs = 0;
UINT8   TempNumberKid = 0;
char   LcdBufferData[64];
static UINT16 COUNTER_COMMAND;
static UINT8  redy; // ���. 01.04.22
// ���. 29.03.22 #include    "iButton.h"
void TerminalLcd (void){
unsigned int CounterCommands=0;                                                 //������� ������ ������
unsigned int CounterParam=0;                                                    //������� ����������
unsigned int    CounterData;
unsigned int NameCommand=0;                                                     //��� �������
char *ptr;
unsigned char dir;
long param[10]={0,0,0,0,0,0,0,0,0,0};
if (!xgetsU4(LineLcd, sizeof LineLcd)){
    return;                                          //���� ������ �� ������, �� �������
}
ptr = LineLcd;                                                                     //����������� ��������� �� ���������� ������ ��������� ����������
//------------------ ���������� �������� ������ --------------------------------
while (CounterCommands<MaxCommands){                                            //���������� ������ �������
   // printf("������� ������:%d\n",CounterCommands);
    if (!strncmp(ptr,CommandsU4[CounterCommands].Command,strlen(CommandsU4[CounterCommands].Command))){     //���� ������� ���� � ������,��
        ptr=strchr(ptr,' ');                                                    //������� �� ������� �������
        while(xatoi(&ptr,&param[CounterParam])){                                //���������� ��������� �������
            CounterParam++;                                                     //����������� �������� ����������
           // printf("������� ����������:%d\n",CounterParam);
        }                              
        break;
    }
    CounterCommands++;                                                          //����������� ������� ������ ������
}//����� while (i<MaxCommands)
NameCommand = CommandsU4[CounterCommands].Name;                                     //���������� ����� �������� �������
//============
COUNTER_COMMAND ++;
/*�������*/if(LcdFlag.Debug){
    if(NameCommand > 0 && NameCommand < 24){
        xprintf("%u Command %u -> %s ",COUNTER_COMMAND,NameCommand,CommandsU4[CounterCommands].Command );
    }else{
        xprintf("%u ERROR %u \r",COUNTER_COMMAND,NameCommand);
    //    WaitingScreen = 255;
        LcdFlag.WaitNewScreen1 = 0;
        return;
    }
    CounterData = 0;
    if(CounterParam){
        while(CounterData < CounterParam){
            xprintf("%lu ",param[CounterData]);
            CounterData ++;
        }
    }
    xprintf("\r");
}
//xprintf("wi - %u\r",RxFifoU4.wi);
//xprintf("ri - %u\r",RxFifoU4.ri);
//xprintf("ct - %u\r",RxFifoU4.ct);
//===============
//printf("�������:"); printf(Commands[CounterCommands].Command);//printf("\n");
//printf(" �����:%d ����������:%d\n",strlen(Commands[CounterCommands].Command),CounterParam); 
//printf("������ ����������: %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld \n",param[0],param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8],param[9]);

//******************************************************************************
//*******             ��������� �������� ������       **************************
//******* ������� ���������: 
//******* NameCommand - ��� ������� (������� �  COMMANDLIST ������������ � case)
//******* CounterParam - ���������� ���������� � �������
//******* long param[x] - ������ ���������� �������, ��� param[0]-��� ������ ��������
//******* 
//******************************************************************************
    switch (NameCommand) {
        UINT8 index;
//------------------------------------------------------------------------------       
//--- ���� ������� ���� � ������, �� ��� �� ��� �� ������� ���������� ---------
//------------------------------------------------------------------------------       
        default :
//            xprintf("LineLcd -> ");
//            xprintf("%s",LineLcd);
////            for(index = 0; index < 128; index ++){
////                xprintf("%c",LineLcd[index]);
////            }
//            xprintf("\r");
//            xprintf("RxFifoU4 -> ");
//            for(index = 0; index < 128; index ++){
//                xprintf("%c",RxFifoU4.buff[index]);
//            }
//            xprintf("\r");
//            xprintf("wi - %u\r",RxFifoU4.wi);
//            xprintf("ri - %u\r",RxFifoU4.ri);
//            xprintf("ct - %u\r",RxFifoU4.ct);
//            xprintf("TxFifoU4 -> ");
//            for(index = 0; index < 128; index ++){
//                xprintf("%c",TxFifoU4.buff[index]);
//            }
//            xprintf("\r");
//            xprintf("wi - %u\r",TxFifoU4.wi);
//            xprintf("ri - %u\r",TxFifoU4.ri);
//            xprintf("ct - %u\r",TxFifoU4.ct);
//            Nop();
            break;
        case    PAGE_CURRENT :
            if(!param[0] && CurrentScreen == 255){
                SaveEvent(100);
            }
            if(!param[0]){ // ������� �� 0 ��������
                printf("t0.txt=\"ver%02lu.%02lu.%02lu\"���",VerD,VerM,VerY);
            }
//----------------
            if(!CurrentScreen && param[0] == 10){
                redy = 0;
                // ��������� �� ������� ������������������ ��
                for(index = 1; (index < 11 && !redy); index ++){
                    if(StatusBU[index].SerialNumber){
                       redy = 1; 
                    }
                }
            }
//---------------            
            CurrentScreen = param[0];
            if(CurrentScreen == WaitingScreen){ // �������� ������� �� ������ ��������?
                LcdFlag.WaitNewScreen1 = 0;      // ������� ���� �������� ��������
                CounterWaitingNewScreen = 0;
                WaitingScreen = 255;
                /*�������*/ if(LcdFlag.Debug)xprintf("jump ok %lu ms\r",Counter1ms);
                LcdFlag.NewPage = 1;
                switch(CurrentScreen){
                    case 5:
                        Nop();
                        ModeRs = ReadCharFromAt45(2047,250); // 16.09.22
                        SelectModeRs485(ModeRs);
                        switch(ControlFlagCP.CurrentModeRs){
                            case 0:
                                break;
                            case 1:
                                printf("page5.b1.pic=92���");
                                break;
                            case 2: case 3:
                                printf("page5.b1.pic=91���");
                            break;
                        }
                        break;
                    case 7:
                        PrintDirectionNumber(CurrentScreen);
                        StatusBU[SelectedDirection].SerialNumber = ReadLongFromAT45(2047,(SelectedDirection * 4));
                        if(StatusBU[SelectedDirection].SerialNumber==0x0){
                            printf("page7.show.txt=\"0\"���");
                            printf("page7.b10.txt=\"��������\"���");
                        }else{
                            printf("page7.show.txt=\"%lu\"���", StatusBU[SelectedDirection].SerialNumber);
                            printf("page7.b10.txt=\"�������\"���");
                        }
                        break;
                    case 10:
                        if(!redy){
                            WaitingScreen = 0;              // ���������� WaitingScreen ����������� 0 ����� �������� �� ������� ����������� �������
                            LcdFlag.WaitNewScreen1 = 1;      // ���������� ���� ������ ��������
                            sprintf(LcdBufferData,"page %u���",WaitingScreen);
                            LcdFlag.NewPage = 1;
                            CurrentScreen = 0;
                            printf("%s",LcdBufferData); /*�������*/if(LcdFlag.Debug)xprintf("%s 1a\r",LcdBufferData);
                            /*�������*/if(LcdFlag.Debug)xprintf("Device no redy\r");
                        }else{
                            /*�������*/if(LcdFlag.Debug)xprintf("Device redy\r");
                        }
                        break;
                    case 11:
                        CounterCheckBU = 0;
                        break;
                    case 12:
                        break;
                    case 13:
                        break;
                    case 14:
                        break;
                    case 15:
                       TempSound  = SOUND;
                        TempLedError = LED_ERROR;
                        TempLedFire = LED_FIRE;
                        TempLedStart = LED_START;
                        TempLedAuto = LED_AUTO; 
                        break;
                    case 18: case 19:
                        NewEventLcdFlag.RedyDataBos = 1;
                        break;
                    case 20:
                        NewEventLcdFlag.RedyDataBos = 1;
                        LcdFlag.Yes = 1;
                        break;
                        
                }
            }else{
                /*�������*/if(LcdFlag.Debug)xprintf("error jump\r");
            }
            break;
        case    CHECK_PSW:
            if(param[0] == AccessPassword){
                WaitingScreen = 2;
                LcdFlag.WaitNewScreen1 = 1;
                LcdFlag.NewPage = 0;
            }else{
//                printf("page 0���"); // ������� �� ��������� �����
                WaitingScreen = 0;
                LcdFlag.WaitNewScreen1 = 1;
                LcdFlag.NewPage = 0;
            }
            break;
            
//++++++++++++=            
        case PAGE_NEW:
            
            if(CounterParam < 2){
                if(LcdFlag.Debug)/*�������*/xprintf("ERR coomand\r");
                break;
            }
            if(LcdFlag.WaitNewScreen1){
                if(LcdFlag.Debug)/*�������*/xprintf("ERROR jump\r");
// ������ ��������
                break;
            }
            Bright = 80;    // ���. 24.03.22
            if(WaitingScreen == 255){
                WaitingScreen = (UINT8)param[0]; // ���������� WaitingScreen ����������� ����� �������� �� ������� ����������� �������
                LcdFlag.WaitNewScreen1 = 1;      // ���������� ���� ������ ��������
                if(param[1] != 255){
                    SelectedDirection = (UINT8) param[1];    // ���������� SelectDirection  ����������� ����� ����������� � ������� ���������� ��������
                    CounterCheckBU = 0;
                    Interval._1min = 1;
                }
                if(WaitingScreen == 111){
                    if(ClassAlgoritm[SelectedDirection] < 11){
                        WaitingScreen = 11;
                    }else{
                        WaitingScreen = 16;
                    }
                }
                if(WaitingScreen == 19){
                    if(CurrentScreen !=20)GroupNumber = (UINT8) param[2];
                }
                if(WaitingScreen == 20){
                    DeviceNumber = (UINT16) param[2];
                }
                if(CurrentScreen == 15){
                    SOUND = TempSound;
                    LED_ERROR = TempLedError;
                    LED_FIRE = TempLedFire;
                    LED_START = TempLedStart;
                    LED_AUTO = TempLedAuto;
                }
                PreviousScreen = CurrentScreen;     // ��������� ����� ������ � �������� ����������� �������
            }
            break;
        case    CHANGE_PSW:
            if(CurrentScreen == 6){
                OldAccessPassword = AccessPassword;;
                AccessPassword = param[0];
                SaveAccessPassword(AccessPassword);
                SaveEvent(105); // ������� ��������� ������
                sprintf(LcdBufferData,"page6.show.bco=%u���", GREEN);
                printf("%s",LcdBufferData); //xprintf("%s\r",LcdBufferData);
                
            }
            break;
        case    TIME_VRF:
            CurrentTime.Year = (unsigned int)param[0];
            CurrentTime.Month = (unsigned char)param[1];
            CurrentTime.Day = (unsigned char)param[2];
            CurrentTime.Hour = (unsigned char)param[3];
            CurrentTime.Minute = (unsigned char)param[4];
            CurrentTime.Second = (unsigned char)param[5];
            break;
        case    TIME_CHANGE:
            if(CurrentScreen == 3){
                CurrentTime.Year = (unsigned int)param[0];
                CurrentTime.Month = (unsigned char)param[1];
                CurrentTime.Day = (unsigned char)param[2];
                CurrentTime.Hour = (unsigned char)param[3];
                CurrentTime.Minute = (unsigned char)param[4];
                CurrentTime.Second = (unsigned char)param[5];
                SaveEvent(101);
                printf("t20.pco=%u���",GREEN);
                CounterDelaySec = 1;
                while (CounterDelaySec);
                printf("t20.pco=%u���", BLACK);
            }
            break;
        case NEXT_DIR:
            SelectedDirection ++;
            if(SelectedDirection > 10){
                SelectedDirection = 1;
            }
            
            StatusBU[SelectedDirection].SerialNumber = ReadLongFromAT45(2047,(SelectedDirection * 4));
            PrintDirectionNumber(CurrentScreen);
            if(StatusBU[SelectedDirection].SerialNumber==0x0){
                printf("page7.show.txt=\"0\"���");
                printf("page7.b10.txt=\"��������\"���");
            }else{
                printf("page7.show.txt=\"%lu\"���", StatusBU[SelectedDirection].SerialNumber);
                printf("page7.b10.txt=\"�������\"���");
            }
            break;
        case    ADD_BU:
            dir = (unsigned char)param[0];
            if (StatusBU[dir].SerialNumber) {
                // ������� ������ � ������� ������
                printf("page7.show.bco=%u���", YELOW);
                CounterDelaySec = 1;
                while (CounterDelaySec);
                printf("page7.show.bco=%u���", WHITE);
                return;
            }
            if (!StatusBU[dir].SerialNumber) { // ���� ������ �� ������
                UINT8 ii;
                for (ii = 1; ii < 11; ii++) {
                    // �������� �� ���������� �������� �������
                    if (param[1] == StatusBU[ii].SerialNumber) {
                        printf("page7.show.bco=%u���", YELOW);
                        CounterDelaySec = 1;
                        while (CounterDelaySec);
                        printf("page7.show.txt=\"%lu\"���", StatusBU[dir].SerialNumber);
                        printf("page7.show.bco=%u���", WHITE);
                        LcdFlag.NewSn = 0;
                        return;
                    }
                }
                // ������ ������ ��������� ������
                printf("page7.show.bco=%u���", GREEN);
                CounterDelaySec = 1;
                StatusBU[dir].SerialNumber = param[1];
                SaveSerialNumberBU(StatusBU[dir].SerialNumber, (dir * 4));
                while (CounterDelaySec);
                SaveEvent(102);
                printf("page7.show.txt=\"%lu\"���", StatusBU[dir].SerialNumber);
                printf("page7.show.bco=%u���", WHITE);
                printf("page7.b10.txt=\"�������\"���");
            }
            Nop();
            break;
        case    DEL_BU:
            // �������� ����������� ��������� ������
            dir = (unsigned char)param[0];
            printf("page7.show.bco=%u���", GREEN);
            CounterDelaySec = 2;
            TempSerialNumberBUR = StatusBU[dir].SerialNumber;
            StatusBU[dir].SerialNumber = 0;
            SaveEvent(103);
            SaveSerialNumberBU(StatusBU[dir].SerialNumber, (dir * 4));
            while (CounterDelaySec);
            printf("page7.show.txt=\"%lu\"���", StatusBU[dir].SerialNumber);
            printf("page7.show.bco=%u���", WHITE);
            printf("page7.b10.txt=\"��������\"���");
            Nop();
            break;
        case    KID_READ:   // ���. 01.04.22
            NumberKID = (UINT8)param[0];
            ReadRegisteredKid(NumberKID); // ������ ������������ ����� � ����������
            printf("page4.t4.txt=\"%lu\"���", SerialNumberKid[NumberKID].SerialKid); // ���. 20.06.22
            printf("page4.t14.txt=\"\"���");
            printf("page4.t13.pic=27���");
            if(SerialNumberKid[NumberKID].SerialKid){
                printf("page4.b0.txt=\"�������\"���");
            }else{
                printf("page4.b0.txt=\"��������\"���");
            }
            break;
        case    KID_SAVE:   // ���. 01.04.22
            NumberKID = (UINT8)param[0];
            TempNumberKid = (UINT8)param[0];
            
            if(FlagMFRC522._newCart){ // ���� ������ �������� ����
                if(SerialNumberKid[0].SerialKid == SerialNumberKid[1].SerialKid || 
                    SerialNumberKid[0].SerialKid == SerialNumberKid[2].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[3].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[4].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[5].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[6].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[7].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[8].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[9].SerialKid ||
                    SerialNumberKid[0].SerialKid == SerialNumberKid[10].SerialKid){
                    printf("page4.t14.txt=\"��� ����\"���");
                    printf("page4.t13.pic=26���");
                }else{
                    SavingRegisteredKid(NumberKID); // ��������� � ������ ����� ����
                    ReadRegisteredKid(NumberKID); // ����������� ������ ������������ ����� � ����������
                    if(SerialNumberKid[NumberKID].SerialKid == SerialNumberKid[0].SerialKid){
                        printf("page4.t4.txt=\"%lu\"���", SerialNumberKid[NumberKID].SerialKid); // ���. 20.06.22
                        printf("page4.t14.txt=\"�������\"���");
                        printf("page4.t13.pic=27���");
                        printf("page4.b0.txt=\"�������\"���");
                        SaveEvent(106);
                    }else{
                        printf("page4.t14.txt=\"�� �������\"���");
                        printf("pfge4.t13.pic=26���");
                    }
                }
                FlagMFRC522._newCart = 0;
            }else{
                if(!SerialNumberKid[NumberKID].SerialKid){
                    printf("page4.t14.txt=\"��� �����\"���");
                    printf("t13.pic=26���");
                }else{
                    SerialNumberKid[NumberKID].SerialKid = 0;
                    SavingRegisteredKid(NumberKID); // ��������� � ������ ����� ����
                    printf("page4.t14.txt=\"���� ������\"���");
                    printf("page4.b0.txt=\"��������\"���");
                }
            }
            break;
        case PLAY:
            LcdFlag.Select = param[0];
            if(LcdFlag.Select){
                LcdFlag.Play = 1;
                LcdFlag.HandDoun = 0;
                LcdFlag.HandUp = 0;
            }else{
                LcdFlag.Play = 0;
            }
            break;
        case INC:
            LcdFlag.Select = param[0];
            LcdFlag.HandDoun = 0;
            LcdFlag.HandUp = 1;
            break;
        case DEC:
            LcdFlag.Select = param[0];
            LcdFlag.HandUp = 0;
            LcdFlag.HandDoun = 1;
            break;
        case AUTO_DIR:

// ���. 29.03.22             TempNumberKid = SearchIbutton();
            TempNumberKid = SearchNumberKid(); // ���. 01.04.22
            if(TempNumberKid == 0 || TempNumberKid > 10) break;
            TempDirectRs = SelectedDirection; // ��������� ����� �����������
            printf("t0.txt=\"�������� �������\"���");
// ���. 02.09.22
            switch(StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000){
                case AUTO:
                    if(LcdFlag.Debug)/*�������*/xprintf("Com 5 auto lcd\r");
                    TempCommandRs = 5;
                    SaveEvent(15); // ������� ������ ����� ���������� ����������� �� �� 
                    break;
                case MANUAL:
                    // ��������� ������� ���������� �������������
                    if(LcdFlag.Debug)/*�������*/xprintf("Com 5 manual lcd\r");
                    TempCommandRs = 5;
                    SaveEvent(15); // ������� ������ ����� ���������� ����������� �� ��
                    break;
                case LOCK:
                    // ��������� ������� ���������� ��������������
                    if(LcdFlag.Debug)/*�������*/xprintf("Com 4 lcd\r");
                    TempCommandRs = 4;
                    SaveEvent(14); // ������� ������ ����� ���������� ����������� �� �� 
                    break;
            }
            break;
        case RESET_DIR:

// ���. 29.03.22             TempNumberKid = SearchIbutton();
            TempNumberKid = SearchNumberKid(); // ���. 01.04.22
            if(TempNumberKid == 0 || TempNumberKid > 10) break;
            printf("t0.txt=\"�������� �������\"���");
            TempDirectRs = SelectedDirection; // ��������� ����� �����������
            TempCommandRs = 6;  // ������� �����
            break;
        case START_DIR:
            if((StatusBU[SelectedDirection].StatusAuto.StatusAutoByte & 0b11000000) == LOCK)break;// ���. 02.09.22
// ���. 29.03.22             TempNumberKid = SearchIbutton();
            TempNumberKid = SearchNumberKid(); // ���. 01.04.22
            if(TempNumberKid == 0 || TempNumberKid > 10) break;
            printf("t0.txt=\"�������� �������\"���");
            TempDirectRs = SelectedDirection; // ��������� ����� �����������
            TempCommandRs = 7;  // ������� ����� �� �����������
            break;
        case READ_ARHIV_LCD:
            LcdFlag.Select = param[0];
            if(LcdFlag.Select){
                LcdFlag.Play = 1;
                LcdFlag.HandDoun = 0;
                LcdFlag.HandUp = 0;
            }else{
                LcdFlag.Play = 0;
            }
            break;
        case READ_ARHIV_USB:
            LcdFlag.Select = param[0];
            break;
        case UPDATE:
            if(CurrentScreen == 5){
                //__asm__ volatile ("reset");
                asm ("RESET");
            }
            break;
        case BRIGHT:   // ���. 24.03.22
// ��������� ������� ��� ������� �������            
//            if(Bright != 80){
//                Bright = 80;
//                printf("dim=%u���",Bright);
//            }
//            CounterLcdSleep = 30;
            break;
// ���. 07.04.22++++++++++++++++++++++++
        case CHANGE_MODE_RS: 
            if(ControlFlagCP.CurrentModeRs == 1){
                SelectModeRs485(2);
                if(ControlFlagCP.CurrentModeRs == 2){
                    printf("page5.b1.pic=91���");
                }
            }else{
                SelectModeRs485(1);
                if(ControlFlagCP.CurrentModeRs == 1){
                    printf("page5.b1.pic=92���");
                }
            }
            SaveModeRs485((unsigned char) ControlFlagCP.CurrentModeRs);
            break;
        
//+++++++++++++++++++++++++++++++++            
    } 
}
//���� ������ - ����� ��� ���������� ������������� ������� (��� ��������� � case)
typedef enum COMMAND_LIST_U2 {
    UnkNown,        //���. 19.11.19
    FromBU,    //
    FromBOS,
    For_M0,
    FromPC
}CommandListU2;
const CommandType   CommandsU2[MaxCommands]=
{//"�������",       "��� �������"
    { "/",          FromBU },
    { "<",          FromBOS },
    { "#",          FromPC  }
};
int xgetsU2 (		/* 0:��� ������ ��� ���������, 1:������ �������� */
	char* buff,	/* ��������� �� ����� */
	int len		/* ������ ������ */
)
{
static int c, i;

	for (;;) {
        if (!RS485_testRx()) {
            return 0;
        }
		c = RS485_getc();				/* ������ ������ ����� �� �������� ������ */
		if (!c) return 0;               /* ����� ������? */
		if (c == '\r'){
            break;           /* ����� ������? */
        } 
		if (c >= ' ' && i < len - 1) {	/*������� ������� */
			buff[i++] = c;
		}
	}
	buff[i] = 0;	/* ��������� � ����� ������ \0 */
    i = 0;
	return 1;
}
int xatoiRs (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;
	*res = 0;
	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */
	if (c == '-') {		/* negative? */
		s = 1;
		c = *(++(*str));
	}
	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x':		/* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b':		/* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1;	/* single zero */
			if (c < '0' || c > '9') return 0;	/* invalid char */
			r = 8;		/* octal */
		}
	} else {
		if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
		r = 10;			/* decimal */
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;	/* invalid char */
		}
		if (c >= r) return 0;		/* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;			/* apply sign if needed */
	*res = val;
	return 1;
}
UINT8 digits(long n) {
    UINT8 i = 0;
    UINT32 nn; 
    if (n < 0){
        nn = -n;
    }else{
        nn = n;
    }
    while (nn > 0) {
        nn = nn/10;
        i ++;
    }
    if(!i) i = 1;
    return i;
}
void ClrLine(char*Array,unsigned int Length ){
    unsigned int i;
    for(i = 0; i < Length; i++){
        *Array = 0;
        Array++;
    }
}
#define DIRECTION_R     paramRs[0]
#define ADRR_SOURCE_R   paramRs[1]
#define COMAND_R        paramRs[2]
//******************************************************************************
//**************************** M A I N *****************************************
//******************************************************************************
void TerminalRs (void){
unsigned int CounterCommandsRs=0;                                                 //������� ������ ������
unsigned int CounterParamRs=0;                                                    //������� ����������
unsigned int NameCommandRs=0;                                                     //��� �������
unsigned int    tempInt;
char *ptrs;
long paramRs[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static UINT8   LengthRxData;
unsigned char crcRx;
unsigned char crcRxLength;
unsigned int    tempLine;
    tempLine = strlen(LineRs);
    if(tempLine > 128){
        ClrLine(LineRs,256);
    }
    if (!xgetsU2(LineRs, sizeof LineRs)){
        return;             //������� ���� ������ �� ������ 
    }
    ptrs = LineRs; //����������� ��������� �� ���������� ������ ��������� ����������
//------------------ ���������� �������� ������ --------------------------------
    while (CounterCommandsRs<MaxCommands){                                            //���������� ����� ������
        if (!strncmp(ptrs,CommandsU2[CounterCommandsRs].Command,strlen(CommandsU2[CounterCommandsRs].Command))){     //���� ������� ���� � ������
            ptrs=strchr(ptrs,' ');                                                    //������� �� ������� �������
            while(xatoiRs(&ptrs,&paramRs[CounterParamRs])){                           //���������� ��������� �������
                CounterParamRs++;                                                     //����������� ������� ����������
            }                              
            break;
        }
        CounterCommandsRs++;                                                          //����������� ������� ������ ������
    }//����� while (i<MaxCommands)
//===========
    LengthRxData = strlen(LineRs);                                      //��������� ����� ������
    crcRxLength = digits(paramRs[CounterParamRs - 1]);                  // ��������� ����������� ���������� ���������
    crcRx = Crc8((unsigned char*)LineRs,(LengthRxData - crcRxLength));  // ��������� ��
    if(crcRx != (unsigned char)(paramRs[CounterParamRs - 1])){          //��������� ��
        CounterErrRxPacket ++;
        if(CounterErrRxPacket > 10){
            ClrLine(LineRs,256);
        }
        return;
    }
    CounterErrRxPacket = 0; 
//+++++++++++++++
    NameCommandRs=CommandsU2[CounterCommandsRs].Name;                                     //���������� ����� ��������� �������
// ���������� ������
    switch (NameCommandRs) {
        default :
            if(LcdFlag.Debug)/*�������*/xprintf("Err Rs\r");//���. 19.11.19
            break;
        case FromBU:
            if(CounterParamRs == 5){// ������� ��������� � ���������� ������������ ���
                if(RequestedSerial != paramRs[1]){
                    return;
                }
                NumberOfResponses[ControlFlagCP.CurrentModeRs] ++; // ���. 05.04.22
                StatusBU[0].Direct = paramRs[0];
                StatusBU[0].SerialNumber = paramRs[1];
                StatusBU[0].QuantityBos = paramRs[2];
                ClassAlgoritm[DirectControl] = paramRs[3];
                CounterDelayRs = 2;
                WaitData = 1;
            }else{
                if(CounterParamRs > 13){
                    if(RequestedSerial != paramRs[7]){
                        return;
                    }
                    NumberOfResponses[ControlFlagCP.CurrentModeRs] ++; // ���. 05.04.22
                    StatusBU[0].Direct = paramRs[6];
                    StatusBU[0].SerialNumber = paramRs[7];
                    StatusBU[0].IdNet = paramRs[8];
                    StatusBU[0].Situation = paramRs[9];
                    if(StatusBU[0].Situation != StatusBU[DirectControl].Situation){
                        NewEventLcdFlag.NewSituation = 1;
                    }
                    StatusBU[0].FlagErrRoom.ErrByte = (UINT8)paramRs[10];
                    if(StatusBU[0].FlagErrRoom.ErrByte != StatusBU[DirectControl].FlagErrRoom.ErrByte){
                        NewEventLcdFlag.NewStaus = 1;
                    }
// ���. 25.03.22                    
                    if(StatusBU[0].SerialNumber > 2111000){ 
                        paramRs[10] = paramRs[10] >> 8;
                        FildFlagsPcb2[0].ByteFlagRoom_Pcb2 = (UINT8)paramRs[10];
// ���. 12.09.22 ���� ����� ����������� ��� ��������� ������� �� ���
                        if(FildFlagsPcb2[0].Reset){
                            TempCommandRs = 6;  // ������� �����
                            FildFlagsPcb2[0].Reset = 0;
                        }
                        if(FildFlagsPcb2[0].ByteFlagRoom_Pcb2 != FildFlagsPcb2[DirectControl].ByteFlagRoom_Pcb2){
                            NewEventLcdFlag.NewStaus = 1;
                        }
                    }
//====================                    
                    StatusBU[0].StatusAuto.StatusAutoByte = paramRs[11];
                    if(StatusBU[0].StatusAuto.StatusAutoByte != StatusBU[DirectControl].StatusAuto.StatusAutoByte){
                        NewEventLcdFlag.NewAuto = 1;
                    }
                    tempInt = (unsigned int)paramRs[12];
                    StatusBU[0].FlagControlDirect.ControlByte = (UINT8)tempInt;
                    if(StatusBU[0].FlagControlDirect.ControlByte != StatusBU[DirectControl].FlagControlDirect.ControlByte){
                        NewEventLcdFlag.NewOut = 1;
                    }
                    FildFlagsPcb2[0].classIP = (tempInt >> 12);
                    // ���. 09.04.2020 �������� ������� �� ��� � 
                    // ������� �������� ������� �������� StatusBos[0].AlarmByte
                    CounterDelayStart[0] = paramRs[13];
                    if(CurrentScreen == 10)StatusBos[0].AlarmByte = 0;
                    CounterDelayRs = 2;
                    WaitData = 1;
                }else{
                    return; // ��������� �� ������������ �������
                }
            }
            break; 
        case FromBOS:
            StatusBos[paramRs[0]].ShortAddres = paramRs[0];
            StatusBos[paramRs[0]].SerialNumber = paramRs[2];
            StatusBos[paramRs[0]].AlarmByte = paramRs[3];
            if(StatusBos[paramRs[0]].AlarmByte){
                CounterAlarmBos ++; //
                switch(StatusBos[paramRs[0]].AlarmByte & 0x03){
                    case 1:
                        CounterAttentionDevice ++;
                        break;
                    case 2:
                        CounterFireDevice ++;
                        break;
                    case 3:
                        CounterStartDevice ++;
                        break;
                }
            }
            // ��� 09.04.2020 ��������� ������� ���������� �� ���
            if(StatusBos[0].AlarmByte < StatusBos[paramRs[0]].AlarmByte){
                StatusBos[0].AlarmByte = StatusBos[paramRs[0]].AlarmByte;
                LcdFlag.NewData = 1;
            }
            StatusBos[paramRs[0]].StatusByte = paramRs[4];
            if(StatusBos[paramRs[0]].StatusByte) CounterErrBos ++;
            StatusBos[paramRs[0]].TemperaturaSensor[0] = paramRs[5];
            StatusBos[paramRs[0]].TemperaturaSensor[1] = paramRs[6];
            break;
    }//����� switch (NameCommand) 
}


