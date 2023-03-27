//------------------------------------------------------------------------/
// Модуль работы с UART для PIC24F                          
//-------------------------------------------------------------------------/
#include <xc.h>
#include <stdio.h>
#include <libpic30.h>
#include    "alwaysCP_210728.h"
#include    "FunctionUART24.h"


// UART4 ТБВПФБЕФ У LCD

#define BUFFER_SIZE 128

#define _DI()		__asm__ volatile("disi #0x3FFF")
#define _EI()		__asm__ volatile("disi #0")

int TxRunLcd;		// флаг - признак передачи 

//static volatile struct {
//	int		ri, wi, ct;			/* индекс чтения, индекс записи, счётчик данных */
//	unsigned char	buff[BUFFER_SIZE];	/* FIFO буфер */
//} TxFifoU4, RxFifoU4;

FifoU4 TxFifoU4, RxFifoU4;

//---- перенаправление стандартного потока ввода-вывода для printf и др.-----------
int __attribute__((__weak__, __section__(".libc")))
write(int handle, void * buffer, unsigned int len)
{ 
#define STDIN   0 //признак потока ввода stdin
#define STDOUT  1 //признак потока вывода stdout
#define STDERR  2 ////признак потока ошибок stderr
 int i = 0;   
    switch (handle)
    {
        case STDIN:break; 
        case STDOUT: while (i < len) uart4_putc(((char*)buffer)[i++]); break;
        case STDERR:break;    
     }
    return (len);  // количество записанных байт
}

//----------- ISR прерывание от UART1 Rx ------------------
void __attribute__((interrupt, auto_psv)) _U4RXInterrupt (void)
{
	unsigned char d;
	int i;
    if(U4STAbits.OERR){
        U4STAbits.OERR = 0;
    }
	d = (unsigned char)U4RXREG;			//получение байта данных
    _U4RXIF = 0;                        //очистка флага прерывания RX
//    if(d == 0xf8){
//        return;
//    }
    if(d == '\n'){
        return;
    }
	i = RxFifoU4.ct;				//количество байт в RX FIFO буфере
	if (i < BUFFER_SIZE) {		//если буфер не полный, то.. 
		RxFifoU4.ct = ++i;        //увеличиваем значение счётчика байт в RX FIFO
		i = RxFifoU4.wi;
		RxFifoU4.buff[i++] = d;	//записываем принятый байт в RX FIFO 
		RxFifoU4.wi = i % BUFFER_SIZE;	/* Next write ptr */
	}
}


//----------- ISR прерывание от UART4 Tx ------------------
void __attribute__((interrupt, auto_psv)) _U4TXInterrupt (void)
{
	int i;
	_U4TXIF = 0;		//очистка флага прерывания TX
	i = TxFifoU4.ct;		//количество байт в TX FIFO буфере
	if (i) {			//если есть данные в TX FIFO буфере
		TxFifoU4.ct = --i;
		i = TxFifoU4.ri;
		U4TXREG = TxFifoU4.buff[i++];		//передаём один байт
		TxFifoU4.ri = i % BUFFER_SIZE;	/* Next read ptr */
	} else {			//если нет данных в Tx FIFO буфере, то ..
		TxRunLcd = 0;		//сбрасываем флаг признака передачи
	}
}

//----------- Проверка количества байт в Rx FIFO буфере  ------------------
int uart4_testRx (void)
{
	return RxFifoU4.ct;	//возвращаем количество байт в Rx FIFO буфере
}
int uart4_testTx (void)
{
	return TxFifoU4.ct;	//возвращаем количество байт в Rx FIFO буфере
}
//----------- Получение одного байта из Rx FIFO буфера */
unsigned char uart4_getc (void)
{
	unsigned char d;
	int i;
	while (!RxFifoU4.ct) ;		//ожидание появления данных в Rx FIFO буфере
	i = RxFifoU4.ri;				
	d = RxFifoU4.buff[i++];       //чтение байта из Rx FIFO ,буфера
	RxFifoU4.ri = i % BUFFER_SIZE;
	_DI();
	RxFifoU4.ct--;
	_EI();
	return d;
}

//----------- Отпревка одного байта в Tx FIFO буфер ------------------
void uart4_putc (unsigned char d)
{
	int i;
	while (TxFifoU4.ct >= BUFFER_SIZE) ;	//ждём, если Tx FIFO буфер 
	i = TxFifoU4.wi;		
	TxFifoU4.buff[i++] = d; //запись в Tx FIFO буфер одного байта
	TxFifoU4.wi = i % BUFFER_SIZE;
	_DI();
	TxFifoU4.ct++;
	if (!TxRunLcd) {		//если передача по UART Tx не ведётся, то ..
		TxRunLcd = 1;      //установка флага - признака передачи
		_U4TXIF = 1;	//разрешения прерывания от UART Tx
	}
	_EI();
}

//----------- Инициализация UART модуля ------------------
void InitUart4 (unsigned long bps)
{
	//выключение UART Tx/Rx прерываний
	_U4RXIE = 0;
	_U4TXIE = 0;

	//инициализация UART1 
	U4BRG = FCY / 4 / bps - 1;
    _RP20R = _RPOUT_U4TX;        //подключение модуля TX1 к ножке RP20
    _U4RXR = 25;                //подключение модуля RX1 к ножке PR25
//    RPINR27bits.U4RXR = 0x0019;    //RD4->UART4:U4RX
//    RPOR10bits.RP20R = 0x001E;    //RD5->UART4:U4TX
    U4MODEbits.BRGH = 1;
	U4MODEbits.UARTEN = 1;
	U4STAbits.UTXEN = 1;

	//очистка счётчиков и указателей Tx/Rx FIFO буферов
	TxFifoU4.ri = 0; TxFifoU4.wi = 0; TxFifoU4.ct = 0;
	RxFifoU4.ri = 0; RxFifoU4.wi = 0; RxFifoU4.ct = 0;
	TxRunLcd = 0;

	//включение UART Tx/Rx прерываний
	_U4RXIE = 1;
	_U4TXIE = 1;
}


