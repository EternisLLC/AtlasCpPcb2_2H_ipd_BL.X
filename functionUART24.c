//------------------------------------------------------------------------/
// ������ ������ � UART ��� PIC24F                          
//-------------------------------------------------------------------------/
#include <xc.h>
#include <stdio.h>
#include <libpic30.h>
#include    "alwaysCP_210728.h"
#include    "FunctionUART24.h"


// UART4 �������� � LCD

#define BUFFER_SIZE 128

#define _DI()		__asm__ volatile("disi #0x3FFF")
#define _EI()		__asm__ volatile("disi #0")

int TxRunLcd;		// ���� - ������� �������� 

FifoU4 TxFifoU4, RxFifoU4;

//---- ��������������� ������������ ������ �����-������ ��� printf � ��.-----------
int __attribute__((__weak__, __section__(".libc")))
write(int handle, void * buffer, unsigned int len)
{ 
#define STDIN   0 //������� ������ ����� stdin
#define STDOUT  1 //������� ������ ������ stdout
#define STDERR  2 ////������� ������ ������ stderr
 int i = 0;   
    switch (handle)
    {
        case STDIN:break; 
        case STDOUT: while (i < len) uart4_putc(((char*)buffer)[i++]); break;
        case STDERR:break;    
     }
    return (len);  // ���������� ���������� ����
}

//----------- ISR ���������� �� UART1 Rx ------------------
void __attribute__((interrupt, auto_psv)) _U4RXInterrupt (void)
{
	unsigned char d;
	int i;
    if(U4STAbits.OERR){
        U4STAbits.OERR = 0;
    }
	d = (unsigned char)U4RXREG;			//��������� ����� ������
    _U4RXIF = 0;                        //������� ����� ���������� RX
    if(d == '\n'){
        return;
    }
	i = RxFifoU4.ct;				//���������� ���� � RX FIFO ������
	if (i < BUFFER_SIZE) {		//���� ����� �� ������, ��.. 
		RxFifoU4.ct = ++i;        //����������� �������� �������� ���� � RX FIFO
		i = RxFifoU4.wi;
		RxFifoU4.buff[i++] = d;	//���������� �������� ���� � RX FIFO 
		RxFifoU4.wi = i % BUFFER_SIZE;	/* Next write ptr */
	}
}


//----------- ISR ���������� �� UART4 Tx ------------------
void __attribute__((interrupt, auto_psv)) _U4TXInterrupt (void)
{
	int i;
	_U4TXIF = 0;		//������� ����� ���������� TX
	i = TxFifoU4.ct;		//���������� ���� � TX FIFO ������
	if (i) {			//���� ���� ������ � TX FIFO ������
		TxFifoU4.ct = --i;
		i = TxFifoU4.ri;
		U4TXREG = TxFifoU4.buff[i++];		//������� ���� ����
		TxFifoU4.ri = i % BUFFER_SIZE;	/* Next read ptr */
	} else {			//���� ��� ������ � Tx FIFO ������, �� ..
		TxRunLcd = 0;		//���������� ���� �������� ��������
	}
}

//----------- �������� ���������� ���� � Rx FIFO ������  ------------------
int uart4_testRx (void)
{
	return RxFifoU4.ct;	//���������� ���������� ���� � Rx FIFO ������
}
int uart4_testTx (void)
{
	return TxFifoU4.ct;	//���������� ���������� ���� � Rx FIFO ������
}
//----------- ��������� ������ ����� �� Rx FIFO ������ */
unsigned char uart4_getc (void)
{
	unsigned char d;
	int i;
	while (!RxFifoU4.ct) ;		//�������� ��������� ������ � Rx FIFO ������
	i = RxFifoU4.ri;				
	d = RxFifoU4.buff[i++];       //������ ����� �� Rx FIFO ,������
	RxFifoU4.ri = i % BUFFER_SIZE;
	_DI();
	RxFifoU4.ct--;
	_EI();
	return d;
}

//----------- �������� ������ ����� � Tx FIFO ����� ------------------
void uart4_putc (unsigned char d)
{
	int i;
	while (TxFifoU4.ct >= BUFFER_SIZE) ;	//���, ���� Tx FIFO ����� 
	i = TxFifoU4.wi;		
	TxFifoU4.buff[i++] = d; //������ � Tx FIFO ����� ������ �����
	TxFifoU4.wi = i % BUFFER_SIZE;
	_DI();
	TxFifoU4.ct++;
	if (!TxRunLcd) {		//���� �������� �� UART Tx �� ������, �� ..
		TxRunLcd = 1;      //��������� ����� - �������� ��������
		_U4TXIF = 1;	//���������� ���������� �� UART Tx
	}
	_EI();
}

//----------- ������������� UART ������ ------------------
void InitUart4 (unsigned long bps)
{
	//���������� UART Tx/Rx ����������
	_U4RXIE = 0;
	_U4TXIE = 0;

	//������������� UART1 
	U4BRG = FCY / 4 / bps - 1;
    _RP20R = _RPOUT_U4TX;        //����������� ������ TX1 � ����� RP20
    _U4RXR = 25;                //����������� ������ RX1 � ����� PR25
    U4MODEbits.BRGH = 1;
	U4MODEbits.UARTEN = 1;
	U4STAbits.UTXEN = 1;

	//������� ��������� � ���������� Tx/Rx FIFO �������
	TxFifoU4.ri = 0; TxFifoU4.wi = 0; TxFifoU4.ct = 0;
	RxFifoU4.ri = 0; RxFifoU4.wi = 0; RxFifoU4.ct = 0;
	TxRunLcd = 0;

	//��������� UART Tx/Rx ����������
	_U4RXIE = 1;
	_U4TXIE = 1;
}


