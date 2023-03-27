#include <xc.h>
#include "funcia_spi.h"
//+++++++ ������� SPI �������� � ��������� AT45+++++++++

/* ������������ ������ SPI	*/

void InitSpi24(void){
    SPI1CON1 = 0x0000;
    SPI1CON2 = 0x0000;
    SPI1STAT = 0x0000; 
    SPI1CON1bits.CKP = 1;
    SPI1CON1bits.CKE = 0;
    SPI1CON1bits.SMP = 1;
    SPI1CON1bits.PPRE = 3;      //�������� ������������ 3-1:1 2-4:1 1-16:1 0-64:1
    SPI1CON1bits.SPRE = 6;		//��������� ������������ 7-1:1 6-2:1 5-3:1 0-8:1
    SPI1CON1bits.MSTEN = 1;		//��� ����� ?�����
    SPI1CON2bits.SPIBEN = 0;    //1- ���. ������������ ������ 8����  0-����� 1 ����
    SPI1STATbits.SPIEN = 0;		//���� �����? SPI
    // ����������� ������ SPI1 � ������ ����������
    _SDI1R = 11;                //SPI MISO �� PR11 ����� 2.1 ���� 46
    _RP3R = _RPOUT_SCK1OUT;     //SPI SCK �� PR3 ����� 2.1 ���� 44
    _RP12R = _RPOUT_SDO1;       //SPI MOSI �� PR12 ����� 2.1 ���� 45
}
void ChangModeSpi(unsigned char mode){
    if(SPI1EN)SPI1EN = 0;
    switch(mode){
        case 1:
            SPI1CON1bits.CKP = 0;
            SPI1CON1bits.CKE = 0;
            SPI1CON1bits.SMP = 0;

            break;
        case 2: // ����� ��� ��45
            SPI1CON1bits.CKP = 1;
            SPI1CON1bits.CKE = 0;
            SPI1CON1bits.SMP = 1;
            break;
        case 3:// ����� ��� CC1101
            SPI1CON1bits.CKP = 0;
            SPI1CON1bits.CKE = 1;
            SPI1CON1bits.SMP = 0;
            break;
        case 4:
            SPI1CON1bits.CKP = 1;
            SPI1CON1bits.CKE = 1;
            SPI1CON1bits.SMP = 0;
            break;
    }
    SPI1EN = 1;
}
unsigned char SendByteToSpi (unsigned char Value){
	SPI1BUF = Value;
	while (!SPI1STATbits.SPIRBF)continue;
	return SPI1BUF;
}


//+++++++++++����� ������� SPI+++++++++ ������ ������� AT45
