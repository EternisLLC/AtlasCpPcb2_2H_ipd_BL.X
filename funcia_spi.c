#include <xc.h>
#include "funcia_spi.h"
//+++++++ ФУНКЦИИ SPI совмещаю с драйвером AT45+++++++++

/* конфигурация модуля SPI	*/

void InitSpi24(void){
    SPI1CON1 = 0x0000;
    SPI1CON2 = 0x0000;
    SPI1STAT = 0x0000; 
    SPI1CON1bits.CKP = 1;
    SPI1CON1bits.CKE = 0;
    SPI1CON1bits.SMP = 1;
    SPI1CON1bits.PPRE = 3;      //основной предделитель 3-1:1 2-4:1 1-16:1 0-64:1
    SPI1CON1bits.SPRE = 6;		//вторичный предделитель 7-1:1 6-2:1 5-3:1 0-8:1
    SPI1CON1bits.MSTEN = 1;		//вкл режим ?астер
    SPI1CON2bits.SPIBEN = 0;    //1- вкл. расширенного буфера 8байт  0-буфер 1 байт
    SPI1STATbits.SPIEN = 0;		//выкл модул? SPI
    // подключение модуля SPI1 к ножкам процессора
    _SDI1R = 11;                //SPI MISO на PR11 схема 2.1 нога 46
    _RP3R = _RPOUT_SCK1OUT;     //SPI SCK на PR3 схема 2.1 нога 44
    _RP12R = _RPOUT_SDO1;       //SPI MOSI на PR12 схема 2.1 нога 45
}
void ChangModeSpi(unsigned char mode){
    if(SPI1EN)SPI1EN = 0;
    switch(mode){
        case 1:
            SPI1CON1bits.CKP = 0;
            SPI1CON1bits.CKE = 0;
            SPI1CON1bits.SMP = 0;

            break;
        case 2: // режим для АТ45
            SPI1CON1bits.CKP = 1;
            SPI1CON1bits.CKE = 0;
            SPI1CON1bits.SMP = 1;
            break;
        case 3:// режим для CC1101
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


//+++++++++++КОНЕЦ ФУНКЦИЙ SPI+++++++++ ДАЛЬШЕ ДРАЙВЕР AT45
