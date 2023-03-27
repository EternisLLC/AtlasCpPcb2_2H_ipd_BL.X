#include    <xc.h>
#include    <libpic30.h>
#include    "alwaysCP_210728.h"
#include    "init256GA106AtlasCP.h"
#include    "functionRS485_K.h"
void InitPort(void){
    // ������������� ��� ����� ������ ��� ��������
    AD1PCFG = 0xFFFF;               //��� ����� ���������
    // ���������� ��� �� ����
    TRISB = 0xFFFF;
    TRISC = 0xFFFF;
    
    // ������������� ������
    ENTX485_I = 1;
    TRIS_ENTX485_I = 0;   // ��������� ��� pcb 2.2 � TD331S485H-A
    LED_START = 0;
    LED_FIRE = 0;
    LED_ERROR = 0;
    LCD_EN = 0;
    SOUND = 0;
    LED_USB_REDY = 1;
    EXT_SOUND = 0;
    
    TXRS_ISO = 1;           // ��������� ��� pcb 2.2 � TD331S485H-A
    TRIS_TXRS_ISO = out;    // ��������� ��� pcb 2.2 � TD331S485H-A
    LED_AUTO = 0;           // ���. 12.07.22
    TRIS_LED_AUTO = 0;      // ���. 12.07.22
    LED_PWR1 = 1;
    LED_PWR2 = 1;
    TRIS_LED_PWR1 = 0;
    TRIS_LED_PWR2 = 0;
    OUT_ERROR = 1;
    TRIS_OUT_ERROR = 0;
    OUT_FIRE = 0;
    TRIS_OUT_FIRE = 0;
    OUT_START = 0;
    TRIS_OUT_START = 0;
    TRIS_LED_START = out;
    TRIS_LED_ERR = out;
    TRIS_LCD_EN = out;
    TRIS_LED_FIRE = out;
    TRIS_SOUND = out;
    TRIS_LED_USB_REDY = out;
    TRIS_EXT_SOUND = out;
    
}
//+++++++++++++++++������ � ���������+++++++++++
void InitTmr1(void){//������������ ������� 1
    T1CONbits.TCKPS = 0;
    _T1IF = 0;
    PR1 = 0x3E7F;
	_T1IE = 0;
	T1CONbits.TON = 0;
}
void OpenTmr1(void){
	TMR1 = 0x00; 
    _T1IP = 4; 
	_T1IF = 0; 
	_T1IE = 1;
	T1CONbits.TON = 1;  
}
void InitTmr1_(UINT16 delay_us){//������������ ������� 1
   _T1IF = 0;               //������� ����� ����������
    T1CONbits.TCKPS = 0;         //������������ 0-1:1,1-1:8,2-1:64,3-1:256
    PR1 = (delay_us * 16)-1;
	_T1IE = 0;				//���������� �� ������� 1
	T1CONbits.TON = 0;		//��������� ������� 1
}
void OpenTmr1_(UINT16 delay_us){
    T1CONbits.TCKPS = 0;         //������������ 0-1:1,1-1:8,2-1:64,3-1:256
	PR1 = (delay_us * 16)-1;
    TMR1 = 0x00;                //�������� �ޣ���� �������
    _T1IP = 4;                  //������������� ��������� ����������
	_T1IF = 0;                //�������� ���� ����������
	T1CONbits.TON = 1;         //���. ������ 4
}

void InitTmr2(void){//������������ ������� 2
    _T2IF = 0;               //������� ����� ����������//������� ����� ����������
    T2CONbits.TCKPS = 2;     //������������ 0-1:1,1-1:8,2-1:64,3-1:256
    PR2 = 24999;             //��������� ������� ������� 0..0xFFFF
	_T2IE = 0;				//���������� �� ������� 2
	T2CONbits.TON = 0;		//��������� ������� 2
}
void InitTmr3(void){
    _T3IF = 0;              //������� ����� ����������
    T3CONbits.TCKPS = 1;    //������������ 0-1:1,1-1:8,2-1:64,3-1:256	
    PR3 = 19999;            //��������� ������� ������� 0..0xFFFF
	_T3IE = 0;				//���������� �� ������� 3
	T3CONbits.TON = 0;		//��������� ������� 3
}

void OpenTmr2(void){
	TMR2 = 0x00;                //�������� �ޣ���� �������
    _T2IP = 4;                  //������������� ��������� ����������
	_T2IF = 0;                //�������� ���� ����������
	_T2IE = 1;                 //���. ���������� �� ������� 2
	T2CONbits.TON = 1;         //���. ������ 2
}

void OpenTmr3(void){
    TMR3 = 0x00; //�������� �ޣ���� �������
    _T3IP = 4; //������������� ��������� ����������
    _T3IF = 0; //�������� ���� ����������
    _T3IE = 1; //���. ���������� �� ������� 3
    T3CONbits.TON = 1; //���. ������ 3
}

void CloseTmr1(void){
    T1CONbits.TON = 0;        //���. ������ 1
	_T1IF = 0;                //�������� ���� ����������
	_T1IE = 0;                //�s��. ���������� �� ������� 1
	
}
void CloseTmr2(void){
    T2CONbits.TON = 0;        //��������� ������
	_T2IF = 0;                //����� ����� ����������
	_T2IE = 0;                //������ ���������� 
}
void CloseTmr3(void){
    T3CONbits.TON = 0;        //��������� ������
	_T3IF = 0;                //����� ����� ����������
	_T3IE = 0;                //������ ���������� 1
}
//TMR4 1ms
void InitTmr4(void){
    T4CONbits.TCKPS = 0;
    _T4IF = 0;
    PR4 = 0x3E7F;
	_T4IE = 0;
	T4CONbits.TON = 0;
}
void OpenTmr4(void){
	TMR4 = 0x00; 
    _T4IP = 4; 
	_T4IF = 0; 
	_T4IE = 1;
	T4CONbits.TON = 1;  
}
//++++++++ ����������� ���
void InitADC( int amask){
    AD1PCFG = amask; 		//��������� ���������� �����
    AD1CON1 = 0x00E0; 		//������������� ������ �������������� ����� �������
    AD1CSSL = 0; 			// no scanning required
    AD1CON2 = 0; 			// use MUXA, AVss and AVdd are used as Vref+/-
    AD1CON3 = 0x8F02; 		// Tsamp = 32 x Tad; Tad=125ns
}

#include    "driverAt45.h"
#include    "functionUART24.h"
#include    "FunctionRS485_K.h"
#include    "funcia_spi.h"
#include    "DriverMFRC522cp.h"

void ProcessorInit (void){
    InitPort();
    InitADC(0xFF3F);
    InitTmr1();
    InitTmr2();
    InitTmr3();
    InitTmr4();
    InitAt45();
    ReadStatusAt45();
    InitUart4 (115200);
    InitUSBTerminal(115200);
    ModeRs = ReadCharFromAt45(2047,250);   // 03.10.22
    InitRS485Terminal(38400);
    OpenTmr1();
    OpenTmr2();
    OpenTmr3();
    OpenTmr4();
}

//============

 
//=========