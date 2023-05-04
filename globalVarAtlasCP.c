#include    "alwaysCP_210728.h"

UINT8           DirectControl = 1;
UINT8           CurrentScreen = 255; // ����� �������� ������ NEXTION
UINT8           PreviousScreen;     // ����� � �������� ����������� �������
UINT8           SelectedDirection = 0; /* ���������� ������������ ��������� �����������
                              * (���� 0 �� ������� �� ���� ����������� 
                              * 1...10 ����� ���������� �����������
                              *  255 ����� ���������� ����������� �� ��������)
                              */
LcdFlagBits     LcdFlag;    // ��������� ������������ ����� ������� � ������ �� ���
UINT16          CommandDebug = 0;
UINT32          AccessPassword   = 1234;
UINT32          OldAccessPassword;
//UINT32          MySerialNumber;
UnionStatusBU       StatusBU[11]; // ��������� ����������� ��������� ��
UINT8           ClassAlgoritm[12]= {0,0,0,0,0,0,0,0,0,0,0,0};// ������ �������� ����� �� ��� �������� ���
UINT8           ConnectBUR[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // ������ ������������ ������� ����� � ��
UINT32          RequestedSerial;// �������� ����� � �������� ����������� ���������
UINT8           WaitData; // ���� �������� ������ �� RS
UINT8           CounterDirect = 1;
UINT8           CounterCheckBU; // ��������� ����� ������������� � �������������� ������������ ���� ����������
UINT16          IndexBos;// ������ ����������������� �������� �.�. ���
UINT32          TempSerialNumberBUR; // ������� ���������� �������� ��������� ������ ���������� ���
UINT16          CounterDelayStart[11]; // ��� 28.07.21 ���������� �������� �������� ������� �������� �����
UINT8           CounterDelayDoor[12]; // ��� 28.07.21 ���������� �������� �������� ������� �������� �����

// ���. 24.03.22
UINT8           Bright = 80;
UINT8           CounterLcdSleep = 30;
UINT16          CounterErrBos;
UINT16          CounterAlarmBos;
UINT16          CounterFireDevice;
UINT16          CounterAttentionDevice;
UINT16          CounterStartDevice;


// ���. 29.03.22 
UINT8 NumberKID;
UINT16 CounterDelayMs;

// ���. 05.04.22
UINT8 NumberOfResponses[4];       // ���������� ������������������ �������� � �������� ������� �� ������� �� RS485

// ���. 06.04.22
FildControlBits ControlFlagCP;
UINT8 ErrPwr;
unsigned char ModeRs; // ���. 13.04.22
UINT16 CounterWaitingNewScreen;  // 07.10.22
UINT8 WaitingScreen = 255;  // 07.10.22 ��������� ��������

UINT8 TempSound;
UINT8 TempLedError;
UINT8 TempLedFire;
UINT8 TempLedStart;
UINT8 TempLedAuto;

//
FildNewEventForLcd NewEventLcdFlag;