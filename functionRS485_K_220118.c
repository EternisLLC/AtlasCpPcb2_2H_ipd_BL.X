#include    <xc.h>
#include    <stdio.h>
#include    <libpic30.h>
#include    <stdbool.h>

//#include    "AlwaysBur.h"
//#include    "UART24_K.h"

#include    "alwaysCP_210728.h"
#include    "FunctionRS485_K.h"

#define _DI()		__asm__ volatile("disi #0x3FFF")
#define _EI()		__asm__ volatile("disi #0")

/* UART1 используетя для USB */
#define SIZE_BUFFER_RXU1 256
#define SIZE_BUFFER_TXU1 256
char TxRunU1;		// флаг - признак передачи // ТКЮЦ - ОПХГМЮЙ ОЕПЕДЮВХ
static volatile struct {
	int		ri, wi, ct;			/* ХМДЕЙЯ ВРЕМХЪ, ХМДЕЙЯ ГЮОХЯХ, ЯВ╦РВХЙ ДЮММШУ  
                                 * индекс чтения, индекс записи, счетчик данных*/
	unsigned char	buff[SIZE_BUFFER_TXU1];	/* буфер FIFO АСТЕП */
} TxFifoU1;
static volatile struct {
	int		ri, wi, ct;			/* ХМДЕЙЯ ВРЕМХЪ, ХМДЕЙЯ ГЮОХЯХ, ЯВ╦РВХЙ ДЮММШУ 
                                 * индекс чтения, индекс записи, счетчик данных*/
	unsigned char	buff[SIZE_BUFFER_RXU1];	/* буфер FIFO АСТЕП */
}RxFifoU1;
/*----------- отправка одного байта в Tx FIFO буфер 
            * нРОПЕБЙЮ НДМНЦН АЮИРЮ Б Tx FIFO АСТЕП 
 * ------------------*/
void uart1_putc (unsigned char d)
{
	int i;
	while (TxFifoU1.ct >= SIZE_BUFFER_TXU1){
//+++++++++++++++
//        if(!_U1TXIF && TxRun){
//            _U1TXIF = 1;
//        }
//++++++++++++++++
    }
	i = TxFifoU1.wi;		
	TxFifoU1.buff[i++] = d; //запись в Tx FIFO буфер одного байта //ГЮОХЯЭ Б Tx FIFO АСТЕП НДМНЦН АЮИРЮ
	TxFifoU1.wi = i % SIZE_BUFFER_TXU1;
	_DI();
	TxFifoU1.ct++;
	if (!_U1TXIF) {		//если передача по UART Tx не ведётся, то .. //ЕЯКХ ОЕПЕДЮВЮ ОН UART Tx МЕ БЕД╦РЯЪ, РН ..
		TxRunU1 = 1;    //установка флага - признака передачи //СЯРЮМНБЙЮ ТКЮЦЮ - ОПХГМЮЙЮ ОЕПЕДЮВХ
		_U1TXIF = 1;	//разрешения прерывания от UART Tx //ПЮГПЕЬЕМХЪ ОПЕПШБЮМХЪ НР UART Tx
	}
	_EI();
}

//----------- хМХЖХЮКХГЮЖХЪ UART1 ------------------
//----------- Инициализация UART1 ------------------
void InitUart1 (unsigned long bps)
{
	//БШЙКЧВЕМХЕ UART Х Tx/Rx ОПЕПШБЮМХИ
    //выключение UART и Tx/Rx прерываний
    U1MODEbits.UARTEN = 0;
	U1STAbits.UTXEN = 0;
	_U1RXIE = 0;
	_U1TXIE = 0;
    _U1TXIF = 0;
    _U1RXIF = 0;
    _U1TXIP = 4;
    _U1RXIP = 4;
    
	//ХМХЖХЮКХГЮЖХЪ UART1 
    //инициализация UART1 
	U1BRG = FCY / 4 / bps - 1;
    U1MODEbits.BRGH = 1;
//    _RP29R =_RPOUT_U1TX;        //подключение модуля TX1 к ножке RP29//ОНДЙКЧВЕМХЕ ЛНДСКЪ TX1 Й МНФЙЕ RP29
//    _U1RXR = 14;                //подключение модуля RX1 к ножке PR14//ОНДЙКЧВЕМХЕ ЛНДСКЪ RX1 Й МНФЙЕ PR14
    RPOR15bits.RP30R = 0x0003;    //RF2->UART1:U1TX
    RPINR18bits.U1RXR = 0x002D;    //RF6->UART1:U1RX
	//НВХЯРЙЮ ЯВ╦РВХЙНБ Х СЙЮГЮРЕКЕИ Tx/Rx FIFO АСТЕПНБ
    //очистка счётчиков и указателей Tx/Rx FIFO буферов
	TxFifoU1.ri = 0; TxFifoU1.wi = 0; TxFifoU1.ct = 0;
	RxFifoU1.ri = 0; RxFifoU1.wi = 0; RxFifoU1.ct = 0;
	TxRunU1 = 0;
    U1MODEbits.UARTEN = 1;
	U1STAbits.UTXEN = 1;
    
    _U1RXIE = 1;
    _U1TXIE = 1;
}
//----------- ISR ОПЕПШБЮМХЕ НР UART1 Rx ------------------
//----------- ISR прерывание от UART1 Rx ------------------
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt (void)
{
	unsigned char d;
	int i;
	d = (unsigned char)U1RXREG;     //получение байта данных //ОНКСВЕМХЕ АЮИРЮ ДЮММШУ
//+++++++
    if(d=='1'){
        CommandDebug = 1;
    }else{
        if(d >= '0' && d <='9')CommandDebug = 0;
    }
//+++++++    
	_U1RXIF = 0;                    //очистка флага прерывания RX//НВХЯРЙЮ ТКЮЦЮ ОПЕПШБЮМХЪ RX
	i = RxFifoU1.ct;				//количество байт в RX FIFO буфере//ЙНКХВЕЯРБН АЮИР Б RX FIFO АСТЕПЕ
	if (i < SIZE_BUFFER_RXU1) {		//если буфер не полный, то.. //ЕЯКХ АСТЕП МЕ ОНКМШИ, РН.. 
		RxFifoU1.ct = ++i;          //увеличиваем значение счётчика байт в RX FIFO//СБЕКХВХБЮЕЛ ГМЮВЕМХЕ ЯВ╦РВХЙЮ АЮИР Б RX FIFO
		i = RxFifoU1.wi;
		RxFifoU1.buff[i++] = d;     //записываем принятый байт в RX FIFO //ГЮОХЯШБЮЕЛ ОПХМЪРШИ АЮИР Б RX FIFO 
		RxFifoU1.wi = i % SIZE_BUFFER_RXU1;	/* Next write ptr */
	}
}
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt (void)
{
    UINT8 tempU1;
	_U1TXIF = 0;            //очистка флага прерывания TX//НВХЯРЙЮ ТКЮЦЮ ОПЕПШБЮМХЪ TX
	if (TxFifoU1.ct) {      //если есть данные в TX FIFO буфере//ЕЯКХ ЕЯРЭ ДЮММШЕ Б TX FIFO АСТЕПЕ
		TxFifoU1.ct --;
        tempU1 = TxFifoU1.buff[TxFifoU1.ri];
        U1TXREG = TxFifoU1.buff[TxFifoU1.ri++];		//передаём один байт //ОЕПЕДЮ╦Л НДХМ АЮИР
        
		TxFifoU1.ri = TxFifoU1.ri % SIZE_BUFFER_TXU1;	// Next read ptr
	} else {                //если нет данных в Tx FIFO буфере, то ..//ЕЯКХ МЕР ДЮММШУ Б Tx FIFO АСТЕПЕ, РН ..
        while(!U1STAbits.TRMT);
		TxRunU1 = 0;		//сбрасываем флаг признака передачи//ЯАПЮЯШБЮЕЛ ТКЮЦ ОПХГМЮЙЮ ОЕПЕДЮВХ
	}
}

//----------- оПНБЕПЙЮ ЙНКХВЕЯРБЮ АЮИР Б Rx FIFO АСТЕПЕ  ------------------
//----------- Проверка количества байт в Rx FIFO буфере  ------------------
int uart1_testRx (void)
{
	return RxFifoU1.ct;	//БНГБПЮЫЮЕЛ ЙНКХВЕЯРБН АЮИР Б Rx FIFO АСТЕПЕ
                        //возвращаем количество байт в Rx FIFO буфере
}
int uart1_testTx (void)
{
	return TxFifoU1.ct;	//БНГБПЮЫЮЕЛ ЙНКХВЕЯРБН АЮИР Б Tx FIFO АСТЕПЕ
                        //возвращаем количество байт в Tx FIFO буфере
}


/*----------- оНКСВЕМХЕ НДМНЦН АЮИРЮ ХГ Rx FIFO АСТЕПЮ */
/*----------- Получение одного байта из Rx FIFO буфера */
unsigned char uart1_getc (void)
{
	unsigned char d;
	int i;
	while (!RxFifoU1.ct);   //ожидание появления данных в Rx FIFO буфере//НФХДЮМХЕ ОНЪБКЕМХЪ ДЮММШУ Б Rx FIFO АСТЕПЕ
	i = RxFifoU1.ri;				
	d = RxFifoU1.buff[i++]; //чтение байта из Rx FIFO ,буфера//ВРЕМХЕ АЮИРЮ ХГ Rx FIFO ,АСТЕПЮ
	RxFifoU1.ri = i % SIZE_BUFFER_RXU1;
	_DI();
	RxFifoU1.ct--;
	_EI();
	return d;
}

// пюанрю UART дкъ RS485
// РАБОТА UART ДЛЯ RS485
unsigned char TxRunRs;		// флаг - признак передачи// ТКЮЦ - ОПХГМЮЙ ОЕПЕДЮВХ 

sructTxRsFifo   TxFifoRs; 
sructRxRsFifo   RxFifoRs;
/* функции подсчета CRC ТСМЙЖХХ ОНДЯВЕРЮ CRC */
const unsigned char Crc8Table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};


unsigned char Crc8(unsigned char *pcBlock, unsigned char len)
{
    unsigned char crc = 0x00;
 
    while (len--)
        crc = Crc8Table[crc ^ *(pcBlock++)];       
    return crc;
}
unsigned char crc_value = 0;
/*++++++очистка буфера RxFifoRs ++++++НВХЯРЙЮ АСТЕПЮ RxFifoRs*/
void ClrFifo(void){
    unsigned int jj;
    unsigned char *pct;
    unsigned char *pcr;
    pct = (unsigned char*)&TxFifoRs.buffRsTx[0];
    pcr = (unsigned char*)&RxFifoRs.buffRsRx[0];
    for(jj = 0; jj < SIZE_BUFFER_RXRS; jj++){
        *pcr = 0;
        pcr++;
    }
    for(jj = 0; jj < SIZE_BUFFER_TXRS; jj++){
        *pct = 0;
        pct++;
    }
    TxFifoRs.riRsTx = 0; TxFifoRs.wiRsTx = 0; TxFifoRs.ctRsTx = 0;
	RxFifoRs.riRsRx = 0; RxFifoRs.wiRsRx = 0; RxFifoRs.ctRsRx = 0;
	TxRunRs = 0;
}
//----------- хМХЖХЮКХГЮЖХЪ UART3 ЛНДСКЪ ------------------
//----------- Инициализация UART3 модуля ------------------
void InitRS485 (unsigned long bps)
{
	// ХМХЖХЮКХГЮЖХЪ МНФЙХ СОПЮБКЕМХЪ ОЕПЕДЮВЕИ RS485
    // инициализация ножки управления передачей RS485
    ENTX485 = 0;
    TRIS_ENTX485 = 0;
    _TRISB14 = 0;
    //БШЙКЧВЕМХЕ UART Tx/Rx ОПЕПШБЮМХИ
    //выключение UART Tx/Rx прерываний
    _U3RXIE = 0;
	_U3TXIE = 0;
    _U3TXIF = 0;
    _U3RXIF = 0;
    _U3TXIP = 4;
    _U3RXIP = 4;
    _U3ERIP = 4;
    
//	_U3RXIE = 0;
//	_U3TXIE = 0;
    U3MODEbits.UARTEN = 0;
	U3STAbits.UTXEN = 0;
//    _RP10R = 0;       //отключение ножки RP10 от модуля TX3 //НРЙКЧВЕМХЕ МНФЙХ RP10 НР ЛНДСКЪ TX3  
//    _U3RXR = 0x3F;    //отключение модуля RX3 от ножки PR17//НРЙКЧВЕМХЕ ЛНДСКЪ RX3 НР МНФЙХ PR17
	//ХМХЖХЮКХГЮЖХЪ UART1 
	U3BRG = FCY / 4 / bps - 1;
//    _RP10R =_RPOUT_U3TX;        //подключение модуля TX3 к ножке RP10//ОНДЙКЧВЕМХЕ ЛНДСКЪ TX3 Й МНФЙЕ RP10
//    _U3RXR = 17;                //подключение модуля RX3 к ножке PR17//ОНДЙКЧВЕМХЕ ЛНДСКЪ RX3 Й МНФЙЕ PR17
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    RPINR17bits.U3RXR = 0x001D;    //RB15->UART3:U3RX
//    RPOR7bits.RP14R = 0x001C;    //RB14->UART3:U3TX
//    RPOR13bits.RP26R = 0x001C;    //RG7->UART3:U3TX
    
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    _RP16R =  _RPOUT_U3TX;    // изм для pcb2.2 для TD331S485H-A //RF3->UART3:U3TX 
    _RP16R = 0; // изм для pcb2.2 для TD331S485H-A
    _RP14R =  _RPOUT_U3TX;    //RB14->UART3:U3TX
    _U3RXR = 29;    //RB15->UART3:U3RX
     //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
    
    //НВХЯРЙЮ ЯВ╦РВХЙНБ Х СЙЮГЮРЕКЕИ Tx/Rx FIFO АСТЕПНБ
    //очистка счётчиков и указателей Tx/Rx FIFO буферов
    ClrFifo();
    U3MODEbits.BRGH = 1;
	U3MODEbits.UARTEN = 1;
	U3STAbits.UTXEN = 0;
    U3STAbits.UTXISEL0=0;       //режим прерывания TX 1-всё передано//ПЕФХЛ ОПЕПШБЮМХЪ TX 1-БЯ╦ ОЕПЕДЮМН
    U3STAbits.UTXISEL1=0;       //режим прерывания TX 1-U3TXREG пуст//ПЕФХЛ ОПЕПШБЮМХЪ TX 1-U3TXREG ОСЯР
	//включение UART Tx/Rx прерываний//БЙКЧВЕМХЕ UART Tx/Rx ОПЕПШБЮМХИ
	_U3RXIE = 1;
	_U3TXIE = 1;
    
    _U3ERIF = 0;//-------------------29.11.21------------------------------
    _U3ERIE = 1;//-------------------29.11.21------------------------------
}

//----------- хМХЖХЮКХГЮЖХЪ UART2 ЛНДСКЪ ------29.11.21----------------------
//----------- Инициализация UART2 модуля ------29.11.21----------------------
void InitRS485_I (unsigned long bps)
{
// ХМХЖХЮКХГЮЖХЪ МНФЙХ СОПЮБКЕМХЪ ОЕПЕДЮВЕИ RS485_I
// инициализация ножки управления передачей RS485
    ENTX485_I = 1;
    //TRIS_ENTX485_I = 0;   // изменения для pcb 2.2 и TD331S485H-A
    _TRISG7 = 0;
    //БШЙКЧВЕМХЕ UART Tx/Rx ОПЕПШБЮМХИ
    //выключение UART Tx/Rx прерываний
    _U2RXIE = 0;
	_U2TXIE = 0;
    _U2TXIF = 0;
    _U2RXIF = 0;
    _U2TXIP = 4;
    _U2RXIP = 4;
    _U2ERIP = 4;
    
//	_U2RXIE = 0;
//	_U2TXIE = 0;
    U2MODEbits.UARTEN = 0;
	U2STAbits.UTXEN = 0;

//	//ХМХЖХЮКХГЮЖХЪ UART2 
    //инициализация UART2 
	U2BRG = FCY / 4 / bps - 1;
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    RPINR19bits.U2RXR = 0x0015;    //RG6->UART2:U2RX
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    RPINR19bits.U2RXR = 0x0011;    //RF5->UART2:U2RX
    _U2RXR = 17;
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
    
    //НВХЯРЙЮ ЯВ╦РВХЙНБ Х СЙЮГЮРЕКЕИ Tx/Rx FIFO АСТЕПНБ
    //очистка счётчиков и указателей Tx/Rx FIFO буферов
    ClrFifo();
    U2MODEbits.BRGH = 1;
	U2MODEbits.UARTEN = 1;
	U2STAbits.UTXEN = 0;//U2STAbits.UTXEN = 1;

	//БЙКЧВЕМХЕ UART Tx/Rx ОПЕПШБЮМХИ
    //включение UART Tx/Rx прерываний
	_U2RXIE = 1;
	_U2TXIE = 0;//_U2TXIE = 1;
    
    _U2ERIF = 0;//-------------------29.11.21------------------------------
    _U2ERIE = 1;//-------------------29.11.21------------------------------
}

//----------- оПНБЕПЙЮ ЙНКХВЕЯРБЮ АЮИР Б Rx FIFO АСТЕПЕ  ------------------
//----------- Проверка количества байт в Rx FIFO буфере  ------------------
int RS485_testRx (void)
{
	return RxFifoRs.ctRsRx;	//БНГБПЮЫЮЕЛ ЙНКХВЕЯРБН АЮИР Б Rx FIFO АСТЕПЕ
                            //возвращаем количество байт в Rx FIFO буфере
}
int RS485_testTx (void)
{
	return TxFifoRs.ctRsTx;	//БНГБПЮЫЮЕЛ ЙНКХВЕЯРБН АЮИР Б Tx FIFO АСТЕПЕ
}                           //возвращаем количество байт в Tx FIFO буфере

/*----------- оНКСВЕМХЕ НДМНЦН АЮИРЮ ХГ Rx FIFO АСТЕПЮ 
* ----------- Получение одного байта из Rx FIFO буфера */
unsigned char RS485_getc (void)
{
	unsigned char d;
	while (!RxFifoRs.ctRsRx); //ожидание появления данных в Rx FIFO буфере//НФХДЮМХЕ ОНЪБКЕМХЪ ДЮММШУ Б Rx FIFO АСТЕПЕ
	d = RxFifoRs.buffRsRx[RxFifoRs.riRsRx ++];//чтение байта из Rx FIFO ,буфера//ВРЕМХЕ АЮИРЮ ХГ Rx FIFO ,АСТЕПЮ
	RxFifoRs.riRsRx = RxFifoRs.riRsRx % SIZE_BUFFER_RXRS;
	_DI();
	RxFifoRs.ctRsRx --;
	_EI();
	return d;
}

//----------- нРОПaБЙЮ НДМНЦН АЮИРЮ Б Tx FIFO АСТЕП ------------------
//----------- Отпрaвка одного байта в Tx FIFO буфер ------------------
//void RS485_putc (unsigned char d)
//{
//	int i;
//	unsigned char delay;
//    while (TxFifoRs.ctRsTx >= SIZE_BUFFER_TXRS){
//     Nop();	//ждём, если Tx FIFO буфер//ФД╦Л, ЕЯКХ Tx FIFO АСТЕП
//    }
//	i = TxFifoRs.wiRsTx;		
//	TxFifoRs.buffRsTx[i++] = d; //запись в Tx FIFO буфер одного байта//ГЮОХЯЭ Б Tx FIFO АСТЕП НДМНЦН АЮИРЮ
//    if(d != '\r' && d != '\n' && d != 0){
//        crc_value = Crc8Table[crc_value ^ d];
//    }else{
//        crc_value = 0;
//    }
//	TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
//	_DI();
//	TxFifoRs.ctRsTx++;
//	if (!TxRunRs) {//если передача по UART Tx не ведётся, то ..//ЕЯКХ ОЕПЕДЮВЮ ОН UART Tx МЕ БЕД╦РЯЪ, РН ..
//		TxRunRs = 1; //установка флага - признака передачи//СЯРЮМНБЙЮ ТКЮЦЮ - ОПХГМЮЙЮ ОЕПЕДЮВХ
//        //-------------------------------------29.11.21------------------------------        
//        //------бЙКЧВЕМХЕ ОЕПЕДЮВХ ОН МЮОПЮБКЕМХЪЛ RS485------------------------
//        //------Включение передачи по направлениям RS485------------------------
//        U3STAbits.UTXEN = 1;
//        if(DirRS485.UART_TX3)
//            ENTX485 = 1;
//        if(DirRS485.UART_TX2)
//            _RP16R = _RPOUT_U3TX;    // изм для pcb2.2 для TD331S485H-A
//            ENTX485_I = 0;  // TXD RS485_I программно подключен к выходу TXD RS485
//                            // TXD RS485_I ОПНЦПЮЛЛМН ОНДЙКЧВЕМ Й БШУНДС TXD RS485
////-------------------------------------29.11.21------------------------------  
//        for(delay = 0; delay < 60; delay ++);//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
////        ENTX485 = 1;
//		_U3TXIE = 1;	//разрешения прерывания от UART3 Tx//ПЮГПЕЬЕМХЪ ОПЕПШБЮМХЪ НР UART3 Tx
////        _U3TXIF = 1;
//	}
//	_EI();
//}

#define xdev_out(func) xfunc_out = (void(*)(unsigned char))(func)
//extern void (*xfunc_out)(unsigned char);
#define xdev_in(func) xfunc_in = (unsigned char(*)(void))(func)
//extern unsigned char (*xfunc_in)(void);
static char *outptr;
unsigned char (*xfunc_in)(void);	/* Pointer to the input stream */
void (*xfunc_out)(unsigned char);	/* Pointer to the output stream */


void xputc (char c)
{
	if (_CR_CRLF && c == '\n')  xputc('\r');		/* CR -> CRLF */
	if (outptr) {
		*outptr++ = (unsigned char)c;
    	return;
	}
	if (xfunc_out) xfunc_out((unsigned char)c);
}
/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/
void xputs (					// Put a string to the default device
	const char* str				// Pointer to the string
)
{
	while (*str)
		xputc(*str++);
}
#include    <stdarg.h>
static void xvprintf (
	const char*	fmt,	/* Pointer to the format string */
	va_list arp			/* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	unsigned long v;
	char s[16], c, d, *p;


	for (;;) {
		c = *fmt++;					/* Get a char */
		if (!c) break;				/* End of format? */
		if (c != '%') {				/* Pass through it if not a % sequense */
			xputc(c); continue;
		}
		f = 0;
		c = *fmt++;					/* Get first char of the sequense */
		if (c == '0') {				/* Flag: '0' padded */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	/* Minimum width */
			w = w * 10 + c - '0';
		if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
			f |= 4; c = *fmt++;
		}
		if (!c) break;				/* End of format? */
		d = c;
		if (d >= 'a') d -= 0x20;
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++) ;
			while (!(f & 2) && j++ < w) xputc(' ');
			xputs(p);
			while (j++ < w) xputc(' ');
			continue;
		case 'C' :					/* Character */
			xputc((char)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;
		default:					/* Unknown type (passthrough) */
			xputc(c); continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (char)(v % r); v /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if (f & 8) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) xputc(d);
		do xputc(s[--i]); while(i);
		while (j++ < w) xputc(' ');
	}
}

void xprintf (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;
	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);
}

DirRS DirRS485;     //-----------------------------29.11.21---------------------

ErrorRS ErrorDirRS485;     //-----------------------------29.11.21---------------------

void InitUSBTerminal (unsigned long bps){
    InitUart1 (bps);
//	xdev_in(RS485_getc);		//инициализация входного потока//ХМХЖХЮКХГЮЖХЪ БУНДМНЦН ОНРНЙЮ
//	xdev_out(RS485_putc);       //инициализация выходного потока//ХМХЖХЮКХГЮЖХЪ БШУНДМНЦН ОНРНЙЮ
    xdev_in(uart1_getc);		//инициализация входного потока//ХМХЖХЮКХГЮЖХЪ БУНДМНЦН ОНРНЙЮ
	xdev_out(uart1_putc);       //инициализация выходного потока//ХМХЖХЮКХГЮЖХЪ БШУНДМНЦН ОНРНЙЮ
}
// изм. 05.04.22
//unsigned char CurrentModeRs;
static volatile UINT16 CounterRsBreak;
void SelectModeRs485(unsigned char modeRs){
    if(ControlFlagCP.CurrentModeRs != modeRs){
        switch(modeRs){
            case 0: // Выключены все каналы RS
                CLOSE_RX2_UART();
                CLOSE_UART_TX2();
                CLOSE_RX3_UART();
                CLOSE_UART_TX3();
                CLOSE_RX3_TX2();
                CLOSE_RX2_TX3();
                break;
            case 1: // режим линейной передачи RS
                NumberOfResponses[1] = 0;
                OPEN_RX2_UART();
                OPEN_RX3_UART();
                OPEN_UART_TX2();
                OPEN_UART_TX3();
                OPEN_RX3_TX2();
                OPEN_RX2_TX3();
                break;
            case 2:// прередача и прием через uart2
                if((NumberOfResponses[3] != NumberOfResponses[0])|| (NumberOfResponses[2] != NumberOfResponses[0])){
                    if(CounterRsBreak < MaxNumberRequests){
                        CounterRsBreak ++;
                    }else{
                        ControlFlagCP.RsBreakNew = 1;
                    }
                }else{
                    CounterRsBreak = 0;
                    ControlFlagCP.RsBreakNew = 0;
                }
                CLOSE_RX3_TX2();
                CLOSE_RX2_TX3();    
                CLOSE_RX3_UART();
                CLOSE_UART_TX3();
                OPEN_RX2_UART();
                OPEN_UART_TX2();
                NumberOfResponses[2] = 0;
                break;
            case 3:// прередача и прием через uart3
                if((NumberOfResponses[2] != NumberOfResponses[0])|| (NumberOfResponses[3] != NumberOfResponses[0])){
                    if(CounterRsBreak < MaxNumberRequests){
                        CounterRsBreak ++;
                    }else{
                        ControlFlagCP.RsBreakNew = 1;
                    }
                }else{
                    ControlFlagCP.RsBreakNew = 0;
                }
                CLOSE_RX3_TX2();
                CLOSE_RX2_TX3();    
                CLOSE_RX2_UART();
                CLOSE_UART_TX2();
                OPEN_RX3_UART();
                OPEN_UART_TX3();
                NumberOfResponses[3] = 0;
                break;
            default:
                break;
        }
        ControlFlagCP.CurrentModeRs = modeRs;
    }
}

void InitRS485Terminal (unsigned long bps){
    InitRS485(bps);
    InitRS485_I(bps);//----------------------------29.11.21---------------------
    DirRS485.DirRS = 0b00111111;//включены все направления RS485--------29.11.21---------
                                //БЙКЧВЕМШ БЯЕ МЮОПЮБКЕМХЪ RS485--------29.11.21---------
//    CLOSE_RX2_UART();//Тест отключения направления//рЕЯР НРЙКЧВЕМХЪ МЮОПЮБКЕМХЪ--------29.11.21---------
//    CLOSE_RX3_UART();//Тест отключения направления//рЕЯР НРЙКЧВЕМХЪ МЮОПЮБКЕМХЪ--------29.11.21---------
//    CLOSE_UART_TX2();//Тест отключения направления//рЕЯР НРЙКЧВЕМХЪ МЮОПЮБКЕМХЪ--------29.11.21---------
//    CLOSE_UART_TX3();//Тест отключения направления//рЕЯР НРЙКЧВЕМХЪ МЮОПЮБКЕМХЪ--------29.11.21---------
//    CLOSE_RX3_TX2();
//    CLOSE_RX2_TX3();
    SelectModeRs485(ModeRs);    // 03.10.22
//    xdev_in(RS485_getc);		//инициализация входного потока//ХМХЖХЮКХГЮЖХЪ БУНДМНЦН ОНРНЙЮ
//	xdev_out(RS485_putc);       //инициализация выходного потока//ХМХЖХЮКХГЮЖХЪ БШУНДМНЦН ОНРНЙЮ
}
UINT8 CounterErrorRs485;

unsigned char RsTxBuffer[SIZE_BUFFER_TXRS];
unsigned char RsTxCrc;
unsigned int RsTxLength;
unsigned char* pctxrs;

void CopyRsTxPacket(void){
    unsigned int i = 0;
    unsigned int j = 0;
    while (TxFifoRs.ctRsTx >= SIZE_BUFFER_TXRS){
     Nop();	//ждём, если Tx FIFO буфер
    }
    while (j < RsTxLength){
        i = TxFifoRs.wiRsTx;		
        TxFifoRs.buffRsTx[i++] = RsTxBuffer[j++];; //запись в Tx FIFO буфер одного байта
        TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
        TxFifoRs.ctRsTx++;
    }
}

void TransmittRsPacket(void){
    unsigned char delay;
    CopyRsTxPacket();
    U3STAbits.UTXEN = 1;
    //------Включение передачи по направлениям RS485------------------------
        if(DirRS485.UART_TX3)
        ENTX485 = 1;
        if(DirRS485.UART_TX2)
        _RP16R = _RPOUT_U3TX;    // изм для pcb2.2 для TD331S485H-A
        ENTX485_I = 0;  // TXD RS485_I программно подключен к выходу TXD RS485
//    pctxrs =  &RsTxBuffer[0];//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
    TxRunRs = 1;		//выставляем флаг признака передачи
    for(delay = 0; delay < 60; delay ++);
    _U3TXIE = 1;
}

//----------- ISR ОПЕПШБЮМХЕ НР UART3 Rx ------------------
//----------- ISR прерывание от UART3 Rx ------------------
void __attribute__((interrupt, auto_psv)) _U3RXInterrupt (void)
{
    //-------------------29.11.21------------------------------
    unsigned char TEMP_U3RXREG;
    unsigned char delay;
    while((U3STAbits.URXDA == 1)){

        TEMP_U3RXREG = (unsigned char)U3RXREG;
        _U3RXIF = 0;
        DirRS485.RX3_OK = 1;
        ErrorDirRS485.RX3_ERROR = 0;
        CounterErrorRs485 = 10;
        if(DirRS485.RX3_TX2){
            int i;
            while (TxFifoRs.ctRsTx >= SIZE_BUFFER_TXRS){//255
             Nop();	//ждём, если Tx FIFO буфер//ФД╦Л, ЕЯКХ Tx FIFO АСТЕП
            }
            i = TxFifoRs.wiRsTx;		
            TxFifoRs.buffRsTx[i++] = TEMP_U3RXREG; //запись в Tx FIFO буфер одного байта//ГЮОХЯЭ Б Tx FIFO АСТЕП НДМНЦН АЮИРЮ
            TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
//            _DI();
            TxFifoRs.ctRsTx++;
            if (!TxRunRs) {		//если передача по UART Tx не ведётся, то ..//ЕЯКХ ОЕПЕДЮВЮ ОН UART Tx МЕ БЕД╦РЯЪ, РН ..
                TxRunRs = 1;    //установка флага - признака передачи //СЯРЮМНБЙЮ ТКЮЦЮ - ОПХГМЮЙЮ ОЕПЕДЮВХ
        //------бЙКЧВЕМХЕ ОЕПЕДЮВХ ОН МЮОПЮБКЕМХЧ RS485_I------------------------
        //------Включение передачи по направлению RS485_I------------------------
                _RP16R = _RPOUT_U3TX;    // изм для pcb2.2 для TD331S485H-A
                U3STAbits.UTXEN = 1;
                ENTX485_I = 0;  // TXD RS485_I ОПНЦПЮЛЛМН ОНДЙКЧВЕМ Й БШУНДС TXD RS485
                                // TXD RS485_I программно подключен к выходу TXD RS485
                for(delay = 0; delay < 60; delay ++);
                _U3TXIE = 1;	//ПЮГПЕЬЕМХЪ ОПЕПШБЮМХЪ НР UART3 Tx
                                //разрешения прерывания от UART3 Tx
            }
        }
        if(DirRS485.RX3_UART){
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx ++] = TEMP_U3RXREG;   //запись в Rx FIFO байта данных//ГЮОХЯЭ Б Rx FIFO АЮИРЮ ДЮММШУ
            // НОПЕДЕКЪЕЛ ЛЕЯРН ЙСДЮ АСДЕЛ ОХЯЮРЭ ЯКЕДСЧЫХЕ ОПХМЪРШЕ ДЮММШЕ
            // определяем место куда будем писать следующие принятые данные
            RxFifoRs.wiRsRx = RxFifoRs.wiRsRx % SIZE_BUFFER_RXRS;    /*  если буфер заполнился или пуст 
                                                                    * RxFifoRs.wi станет равным 0
                                                                    * ЕЯКХ АСТЕП ГЮОНКМХКЯЪ ХКХ ОСЯР 
                                                                    * RxFifoRs.wi ЯРЮМЕР ПЮБМШЛ 0 */
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx] = 0;
            RxFifoRs.ctRsRx ++; // СБЕКХВХБЮЕЛ МЮ 1 ЙНКХВЕЯРБН МЕ ОПНВХРЮММШУ АЮИР
                                // увеличиваем на 1 количество не прочитанных байт
        }
    }
}
//-------------------29.11.21------------------------------
UINT8 CounterErrorRs485_I;
//----------- ISR ОПЕПШБЮМХЕ НР UART2 Rx ------------------
//----------- ISR прерывание от UART2 Rx ------------------
void __attribute__((interrupt, auto_psv)) _U2RXInterrupt (void)
{
    unsigned char TEMP_U2RXREG;
    unsigned char delay;
    while(U2STAbits.URXDA == 1){
        TEMP_U2RXREG = (unsigned char)U2RXREG;
        _U2RXIF = 0;
        DirRS485.RX2_OK = 1;
        ErrorDirRS485.RX2_ERROR = 0;
        CounterErrorRs485_I = 10;
        if(DirRS485.RX2_TX3){
            int i;
            while (TxFifoRs.ctRsTx >= SIZE_BUFFER_TXRS){
             Nop();	//ждём, если Tx FIFO буфер//ФД╦Л, ЕЯКХ Tx FIFO АСТЕП
            }
            i = TxFifoRs.wiRsTx;		
            TxFifoRs.buffRsTx[i++] = TEMP_U2RXREG; //запись в Tx FIFO буфер одного байта//ГЮОХЯЭ Б Tx FIFO АСТЕП НДМНЦН АЮИРЮ
            TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
            TxFifoRs.ctRsTx++;
            if (!TxRunRs) {     //если передача по UART Tx не ведётся, то ..//ЕЯКХ ОЕПЕДЮВЮ ОН UART Tx МЕ БЕД╦РЯЪ, РН ..
                TxRunRs = 1;    //установка флага - признака передачи//СЯРЮМНБЙЮ ТКЮЦЮ - ОПХГМЮЙЮ ОЕПЕДЮВХ
        //------бЙКЧВЕМХЕ ОЕПЕДЮВХ ОН МЮОПЮБКЕМХЧ RS485------------------------
        //------Включение передачи по направлению RS485------------------------        
                U3STAbits.UTXEN = 1;
                ENTX485 = 1;    // TXD RS485 ОПНЦПЮЛЛМН ОНДЙКЧВЕМ Й БШУНДС TXD RS485
                                // TXD RS485 программно подключен к выходу TXD RS485
                for(delay = 0; delay < 60; delay ++);
                _U3TXIE = 1;	//ПЮГПЕЬЕМХЪ ОПЕПШБЮМХЪ НР UART Tx
                                //разрешения прерывания от UART Tx
            }
        }
        if(DirRS485.RX2_UART){
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx ++] = TEMP_U2RXREG;   //запись в Rx FIFO байта данных//ГЮОХЯЭ Б Rx FIFO АЮИРЮ ДЮММШУ
            // НОПЕДЕКЪЕЛ ЛЕЯРН ЙСДЮ АСДЕЛ ОХЯЮРЭ ЯКЕДСЧЫХЕ ОПХМЪРШЕ ДЮММШЕ
            // определяем место куда будем писать следующие принятые данные
            RxFifoRs.wiRsRx = RxFifoRs.wiRsRx % SIZE_BUFFER_RXRS;   /* если буфер заполнился или пуст 
                                                                    * RxFifoRs.wi станет равным 0
                                                                    * ЕЯКХ АСТЕП ГЮОНКМХКЯЪ ХКХ ОСЯР 
                                                                    * RxFifoRs.wi ЯРЮМЕР ПЮБМШЛ 0 */
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx] = 0;
            RxFifoRs.ctRsRx ++; // СБЕКХВХБЮЕЛ МЮ 1 ЙНКХВЕЯРБН МЕ ОПНВХРЮММШУ АЮИР
                                // увеличиваем на 1 количество не прочитанных байт
        }
    }
}
//-------------------29.11.21------------------------------
//----------- ISR ОПЕПШБЮМХЕ НР UART3 Tx ------------------
//----------- ISR прерывание от UART3 Tx ------------------
//#include "FunctionTMR.h"
void __attribute__((interrupt, auto_psv)) _U3TXInterrupt (void)
{
    _U3TXIF = 0;		//очистка флага прерывания TX
    if (TxFifoRs.ctRsTx) {
        TxFifoRs.ctRsTx --;
		U3TXREG = TxFifoRs.buffRsTx[TxFifoRs.riRsTx ++];		//передаём один байт
		TxFifoRs.riRsTx = TxFifoRs.riRsTx % SIZE_BUFFER_TXRS;	/* Next read ptr */
    }else{
        _U3TXIE = 0;
        while(!U3STAbits.TRMT); // ожидаем опустошения буфера передачи
        TxRunRs = 0;		//сбрасываем флаг признака передачи
        ENTX485 = 0;
        _RP16R = 0;    // изм для pcb2.2 для TD331S485H-A
        ENTX485_I = 1;
        U3STAbits.UTXEN = 0;
    }
}
//-------------------29.11.21------------------------------
//ЯАПНЯ АСТЕПЮ ОПХЕЛМХЙЮ Х RSR Х ЯНЯРНЪМХЪ НЬХАЙХ ОЕПЕОНКМЕМХЪ АСТЕПЮ
//сброс буфера приемника и RSR и состояния ошибки переполнения буфера
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U3ErrInterrupt ( void )
{
    if ((U3STAbits.OERR == 1))
    {
        U3STAbits.OERR = 0;
    }
    IFS5bits.U3ERIF = 0;
}
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2ErrInterrupt ( void )
{
    if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
    }
    IFS4bits.U2ERIF = 0;
}
//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//OOOOOOOOOO---Функции управления ИКЗ---OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void Init_IKZ (void)//Проверка целостности линий RS485 и начальная установка направлений.
{
//    Сброс всех ИКЗ
    Reset_IKZ ();
//    Проверка целостности "кольца"
    TestChenel_IKZ ();
//    Установка направлений МАСТЕРА
    SetChenel_IKZ_M ();
//    Сохранение текущего состояния всех ИКЗ
    SaveChenel_IKZ ();
}

void Terminal_IKZ (void)//Контроль целостности линий RS485 в рабочем режиме.
{
    
}

void Reset_IKZ (void)//Сброс всех ИКЗ в начальное состояние.
{
//    Сброс направлений МАСТЕРА
    
//    Команда сброс состояния всех ИКЗ
    //============ ТЕСТ ========22.01.20===================================
//    while(ENTX485 || !ENTX485_I); // ожидаем окончания предыдущей передачи
//    xprintf("? %lu %u ",StatusBU[2].SerialNumber,222);
//    xprintf("%u\r",crc_value);
//    while(ENTX485 || !ENTX485_I); // ожидаем окончания предыдущей передачи
    //============ Конец ТЕСТа ========22.01.20===================================
}

void SaveChenel_IKZ (void)//Сохранение текущего состояния всех ИКЗ и отключение неисправных участков RS485.
{
//    Команда сохранение текущего состояния всех ИКЗ
    
}

void TestChenel_IKZ (void)
{
    
}

void SetChenel_IKZ_M (void)
{
    
}
