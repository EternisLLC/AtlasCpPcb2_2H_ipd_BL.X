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

/* UART1 ����������� ��� USB */
#define SIZE_BUFFER_RXU1 256
#define SIZE_BUFFER_TXU1 256
char TxRunU1;		// ���� - ������� �������� // ���� - ������� ��������
static volatile struct {
	int		ri, wi, ct;			/* ������ ������, ������ ������, ������� ������  
                                 * ������ ������, ������ ������, ������� ������*/
	unsigned char	buff[SIZE_BUFFER_TXU1];	/* ����� FIFO ����� */
} TxFifoU1;
static volatile struct {
	int		ri, wi, ct;			/* ������ ������, ������ ������, ������� ������ 
                                 * ������ ������, ������ ������, ������� ������*/
	unsigned char	buff[SIZE_BUFFER_RXU1];	/* ����� FIFO ����� */
}RxFifoU1;
/*----------- �������� ������ ����� � Tx FIFO ����� 
            * �������� ������ ����� � Tx FIFO ����� 
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
	TxFifoU1.buff[i++] = d; //������ � Tx FIFO ����� ������ ����� //������ � Tx FIFO ����� ������ �����
	TxFifoU1.wi = i % SIZE_BUFFER_TXU1;
	_DI();
	TxFifoU1.ct++;
	if (!_U1TXIF) {		//���� �������� �� UART Tx �� ��ģ���, �� .. //���� �������� �� UART Tx �� ������, �� ..
		TxRunU1 = 1;    //��������� ����� - �������� �������� //��������� ����� - �������� ��������
		_U1TXIF = 1;	//���������� ���������� �� UART Tx //���������� ���������� �� UART Tx
	}
	_EI();
}

//----------- ������������� UART1 ------------------
//----------- ������������� UART1 ------------------
void InitUart1 (unsigned long bps)
{
	//���������� UART � Tx/Rx ����������
    //���������� UART � Tx/Rx ����������
    U1MODEbits.UARTEN = 0;
	U1STAbits.UTXEN = 0;
	_U1RXIE = 0;
	_U1TXIE = 0;
    _U1TXIF = 0;
    _U1RXIF = 0;
    _U1TXIP = 4;
    _U1RXIP = 4;
    
	//������������� UART1 
    //������������� UART1 
	U1BRG = FCY / 4 / bps - 1;
    U1MODEbits.BRGH = 1;
//    _RP29R =_RPOUT_U1TX;        //����������� ������ TX1 � ����� RP29//����������� ������ TX1 � ����� RP29
//    _U1RXR = 14;                //����������� ������ RX1 � ����� PR14//����������� ������ RX1 � ����� PR14
    RPOR15bits.RP30R = 0x0003;    //RF2->UART1:U1TX
    RPINR18bits.U1RXR = 0x002D;    //RF6->UART1:U1RX
	//������� ��������� � ���������� Tx/Rx FIFO �������
    //������� �ޣ������ � ���������� Tx/Rx FIFO �������
	TxFifoU1.ri = 0; TxFifoU1.wi = 0; TxFifoU1.ct = 0;
	RxFifoU1.ri = 0; RxFifoU1.wi = 0; RxFifoU1.ct = 0;
	TxRunU1 = 0;
    U1MODEbits.UARTEN = 1;
	U1STAbits.UTXEN = 1;
    
    _U1RXIE = 1;
    _U1TXIE = 1;
}
//----------- ISR ���������� �� UART1 Rx ------------------
//----------- ISR ���������� �� UART1 Rx ------------------
void __attribute__((interrupt, auto_psv)) _U1RXInterrupt (void)
{
	unsigned char d;
	int i;
	d = (unsigned char)U1RXREG;     //��������� ����� ������ //��������� ����� ������
//+++++++
    if(d=='1'){
        CommandDebug = 1;
    }else{
        if(d >= '0' && d <='9')CommandDebug = 0;
    }
//+++++++    
	_U1RXIF = 0;                    //������� ����� ���������� RX//������� ����� ���������� RX
	i = RxFifoU1.ct;				//���������� ���� � RX FIFO ������//���������� ���� � RX FIFO ������
	if (i < SIZE_BUFFER_RXU1) {		//���� ����� �� ������, ��.. //���� ����� �� ������, ��.. 
		RxFifoU1.ct = ++i;          //����������� �������� �ޣ����� ���� � RX FIFO//����������� �������� �������� ���� � RX FIFO
		i = RxFifoU1.wi;
		RxFifoU1.buff[i++] = d;     //���������� �������� ���� � RX FIFO //���������� �������� ���� � RX FIFO 
		RxFifoU1.wi = i % SIZE_BUFFER_RXU1;	/* Next write ptr */
	}
}
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt (void)
{
    UINT8 tempU1;
	_U1TXIF = 0;            //������� ����� ���������� TX//������� ����� ���������� TX
	if (TxFifoU1.ct) {      //���� ���� ������ � TX FIFO ������//���� ���� ������ � TX FIFO ������
		TxFifoU1.ct --;
        tempU1 = TxFifoU1.buff[TxFifoU1.ri];
        U1TXREG = TxFifoU1.buff[TxFifoU1.ri++];		//�������� ���� ���� //������� ���� ����
        
		TxFifoU1.ri = TxFifoU1.ri % SIZE_BUFFER_TXU1;	// Next read ptr
	} else {                //���� ��� ������ � Tx FIFO ������, �� ..//���� ��� ������ � Tx FIFO ������, �� ..
        while(!U1STAbits.TRMT);
		TxRunU1 = 0;		//���������� ���� �������� ��������//���������� ���� �������� ��������
	}
}

//----------- �������� ���������� ���� � Rx FIFO ������  ------------------
//----------- �������� ���������� ���� � Rx FIFO ������  ------------------
int uart1_testRx (void)
{
	return RxFifoU1.ct;	//���������� ���������� ���� � Rx FIFO ������
                        //���������� ���������� ���� � Rx FIFO ������
}
int uart1_testTx (void)
{
	return TxFifoU1.ct;	//���������� ���������� ���� � Tx FIFO ������
                        //���������� ���������� ���� � Tx FIFO ������
}


/*----------- ��������� ������ ����� �� Rx FIFO ������ */
/*----------- ��������� ������ ����� �� Rx FIFO ������ */
unsigned char uart1_getc (void)
{
	unsigned char d;
	int i;
	while (!RxFifoU1.ct);   //�������� ��������� ������ � Rx FIFO ������//�������� ��������� ������ � Rx FIFO ������
	i = RxFifoU1.ri;				
	d = RxFifoU1.buff[i++]; //������ ����� �� Rx FIFO ,������//������ ����� �� Rx FIFO ,������
	RxFifoU1.ri = i % SIZE_BUFFER_RXU1;
	_DI();
	RxFifoU1.ct--;
	_EI();
	return d;
}

// ������ UART ��� RS485
// ������ UART ��� RS485
unsigned char TxRunRs;		// ���� - ������� ��������// ���� - ������� �������� 

sructTxRsFifo   TxFifoRs; 
sructRxRsFifo   RxFifoRs;
/* ������� �������� CRC ������� �������� CRC */
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
/*++++++������� ������ RxFifoRs ++++++������� ������ RxFifoRs*/
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
//----------- ������������� UART3 ������ ------------------
//----------- ������������� UART3 ������ ------------------
void InitRS485 (unsigned long bps)
{
	// ������������� ����� ���������� ��������� RS485
    // ������������� ����� ���������� ��������� RS485
    ENTX485 = 0;
    TRIS_ENTX485 = 0;
    _TRISB14 = 0;
    //���������� UART Tx/Rx ����������
    //���������� UART Tx/Rx ����������
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
//    _RP10R = 0;       //���������� ����� RP10 �� ������ TX3 //���������� ����� RP10 �� ������ TX3  
//    _U3RXR = 0x3F;    //���������� ������ RX3 �� ����� PR17//���������� ������ RX3 �� ����� PR17
	//������������� UART1 
	U3BRG = FCY / 4 / bps - 1;
//    _RP10R =_RPOUT_U3TX;        //����������� ������ TX3 � ����� RP10//����������� ������ TX3 � ����� RP10
//    _U3RXR = 17;                //����������� ������ RX3 � ����� PR17//����������� ������ RX3 � ����� PR17
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    RPINR17bits.U3RXR = 0x001D;    //RB15->UART3:U3RX
//    RPOR7bits.RP14R = 0x001C;    //RB14->UART3:U3TX
//    RPOR13bits.RP26R = 0x001C;    //RG7->UART3:U3TX
    
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    _RP16R =  _RPOUT_U3TX;    // ��� ��� pcb2.2 ��� TD331S485H-A //RF3->UART3:U3TX 
    _RP16R = 0; // ��� ��� pcb2.2 ��� TD331S485H-A
    _RP14R =  _RPOUT_U3TX;    //RB14->UART3:U3TX
    _U3RXR = 29;    //RB15->UART3:U3RX
     //ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
    
    //������� ��������� � ���������� Tx/Rx FIFO �������
    //������� �ޣ������ � ���������� Tx/Rx FIFO �������
    ClrFifo();
    U3MODEbits.BRGH = 1;
	U3MODEbits.UARTEN = 1;
	U3STAbits.UTXEN = 0;
    U3STAbits.UTXISEL0=0;       //����� ���������� TX 1-�ӣ ��������//����� ���������� TX 1-�� ��������
    U3STAbits.UTXISEL1=0;       //����� ���������� TX 1-U3TXREG ����//����� ���������� TX 1-U3TXREG ����
	//��������� UART Tx/Rx ����������//��������� UART Tx/Rx ����������
	_U3RXIE = 1;
	_U3TXIE = 1;
    
    _U3ERIF = 0;//-------------------29.11.21------------------------------
    _U3ERIE = 1;//-------------------29.11.21------------------------------
}

//----------- ������������� UART2 ������ ------29.11.21----------------------
//----------- ������������� UART2 ������ ------29.11.21----------------------
void InitRS485_I (unsigned long bps)
{
// ������������� ����� ���������� ��������� RS485_I
// ������������� ����� ���������� ��������� RS485
    ENTX485_I = 1;
    //TRIS_ENTX485_I = 0;   // ��������� ��� pcb 2.2 � TD331S485H-A
    _TRISG7 = 0;
    //���������� UART Tx/Rx ����������
    //���������� UART Tx/Rx ����������
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

//	//������������� UART2 
    //������������� UART2 
	U2BRG = FCY / 4 / bps - 1;
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    RPINR19bits.U2RXR = 0x0015;    //RG6->UART2:U2RX
//ZZZZZZZZZZZZZZZZZZZZZZZZ 15.06.22 ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//    RPINR19bits.U2RXR = 0x0011;    //RF5->UART2:U2RX
    _U2RXR = 17;
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
    
    //������� ��������� � ���������� Tx/Rx FIFO �������
    //������� �ޣ������ � ���������� Tx/Rx FIFO �������
    ClrFifo();
    U2MODEbits.BRGH = 1;
	U2MODEbits.UARTEN = 1;
	U2STAbits.UTXEN = 0;//U2STAbits.UTXEN = 1;

	//��������� UART Tx/Rx ����������
    //��������� UART Tx/Rx ����������
	_U2RXIE = 1;
	_U2TXIE = 0;//_U2TXIE = 1;
    
    _U2ERIF = 0;//-------------------29.11.21------------------------------
    _U2ERIE = 1;//-------------------29.11.21------------------------------
}

//----------- �������� ���������� ���� � Rx FIFO ������  ------------------
//----------- �������� ���������� ���� � Rx FIFO ������  ------------------
int RS485_testRx (void)
{
	return RxFifoRs.ctRsRx;	//���������� ���������� ���� � Rx FIFO ������
                            //���������� ���������� ���� � Rx FIFO ������
}
int RS485_testTx (void)
{
	return TxFifoRs.ctRsTx;	//���������� ���������� ���� � Tx FIFO ������
}                           //���������� ���������� ���� � Tx FIFO ������

/*----------- ��������� ������ ����� �� Rx FIFO ������ 
* ----------- ��������� ������ ����� �� Rx FIFO ������ */
unsigned char RS485_getc (void)
{
	unsigned char d;
	while (!RxFifoRs.ctRsRx); //�������� ��������� ������ � Rx FIFO ������//�������� ��������� ������ � Rx FIFO ������
	d = RxFifoRs.buffRsRx[RxFifoRs.riRsRx ++];//������ ����� �� Rx FIFO ,������//������ ����� �� Rx FIFO ,������
	RxFifoRs.riRsRx = RxFifoRs.riRsRx % SIZE_BUFFER_RXRS;
	_DI();
	RxFifoRs.ctRsRx --;
	_EI();
	return d;
}

//----------- ����a��� ������ ����� � Tx FIFO ����� ------------------
//----------- ����a��� ������ ����� � Tx FIFO ����� ------------------
//void RS485_putc (unsigned char d)
//{
//	int i;
//	unsigned char delay;
//    while (TxFifoRs.ctRsTx >= SIZE_BUFFER_TXRS){
//     Nop();	//�ģ�, ���� Tx FIFO �����//���, ���� Tx FIFO �����
//    }
//	i = TxFifoRs.wiRsTx;		
//	TxFifoRs.buffRsTx[i++] = d; //������ � Tx FIFO ����� ������ �����//������ � Tx FIFO ����� ������ �����
//    if(d != '\r' && d != '\n' && d != 0){
//        crc_value = Crc8Table[crc_value ^ d];
//    }else{
//        crc_value = 0;
//    }
//	TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
//	_DI();
//	TxFifoRs.ctRsTx++;
//	if (!TxRunRs) {//���� �������� �� UART Tx �� ��ģ���, �� ..//���� �������� �� UART Tx �� ������, �� ..
//		TxRunRs = 1; //��������� ����� - �������� ��������//��������� ����� - �������� ��������
//        //-------------------------------------29.11.21------------------------------        
//        //------��������� �������� �� ������������ RS485------------------------
//        //------��������� �������� �� ������������ RS485------------------------
//        U3STAbits.UTXEN = 1;
//        if(DirRS485.UART_TX3)
//            ENTX485 = 1;
//        if(DirRS485.UART_TX2)
//            _RP16R = _RPOUT_U3TX;    // ��� ��� pcb2.2 ��� TD331S485H-A
//            ENTX485_I = 0;  // TXD RS485_I ���������� ��������� � ������ TXD RS485
//                            // TXD RS485_I ���������� ��������� � ������ TXD RS485
////-------------------------------------29.11.21------------------------------  
//        for(delay = 0; delay < 60; delay ++);//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
////        ENTX485 = 1;
//		_U3TXIE = 1;	//���������� ���������� �� UART3 Tx//���������� ���������� �� UART3 Tx
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
//	xdev_in(RS485_getc);		//������������� �������� ������//������������� �������� ������
//	xdev_out(RS485_putc);       //������������� ��������� ������//������������� ��������� ������
    xdev_in(uart1_getc);		//������������� �������� ������//������������� �������� ������
	xdev_out(uart1_putc);       //������������� ��������� ������//������������� ��������� ������
}
// ���. 05.04.22
//unsigned char CurrentModeRs;
static volatile UINT16 CounterRsBreak;
void SelectModeRs485(unsigned char modeRs){
    if(ControlFlagCP.CurrentModeRs != modeRs){
        switch(modeRs){
            case 0: // ��������� ��� ������ RS
                CLOSE_RX2_UART();
                CLOSE_UART_TX2();
                CLOSE_RX3_UART();
                CLOSE_UART_TX3();
                CLOSE_RX3_TX2();
                CLOSE_RX2_TX3();
                break;
            case 1: // ����� �������� �������� RS
                NumberOfResponses[1] = 0;
                OPEN_RX2_UART();
                OPEN_RX3_UART();
                OPEN_UART_TX2();
                OPEN_UART_TX3();
                OPEN_RX3_TX2();
                OPEN_RX2_TX3();
                break;
            case 2:// ��������� � ����� ����� uart2
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
            case 3:// ��������� � ����� ����� uart3
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
    DirRS485.DirRS = 0b00111111;//�������� ��� ����������� RS485--------29.11.21---------
                                //�������� ��� ����������� RS485--------29.11.21---------
//    CLOSE_RX2_UART();//���� ���������� �����������//���� ���������� �����������--------29.11.21---------
//    CLOSE_RX3_UART();//���� ���������� �����������//���� ���������� �����������--------29.11.21---------
//    CLOSE_UART_TX2();//���� ���������� �����������//���� ���������� �����������--------29.11.21---------
//    CLOSE_UART_TX3();//���� ���������� �����������//���� ���������� �����������--------29.11.21---------
//    CLOSE_RX3_TX2();
//    CLOSE_RX2_TX3();
    SelectModeRs485(ModeRs);    // 03.10.22
//    xdev_in(RS485_getc);		//������������� �������� ������//������������� �������� ������
//	xdev_out(RS485_putc);       //������������� ��������� ������//������������� ��������� ������
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
     Nop();	//�ģ�, ���� Tx FIFO �����
    }
    while (j < RsTxLength){
        i = TxFifoRs.wiRsTx;		
        TxFifoRs.buffRsTx[i++] = RsTxBuffer[j++];; //������ � Tx FIFO ����� ������ �����
        TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
        TxFifoRs.ctRsTx++;
    }
}

void TransmittRsPacket(void){
    unsigned char delay;
    CopyRsTxPacket();
    U3STAbits.UTXEN = 1;
    //------��������� �������� �� ������������ RS485------------------------
        if(DirRS485.UART_TX3)
        ENTX485 = 1;
        if(DirRS485.UART_TX2)
        _RP16R = _RPOUT_U3TX;    // ��� ��� pcb2.2 ��� TD331S485H-A
        ENTX485_I = 0;  // TXD RS485_I ���������� ��������� � ������ TXD RS485
//    pctxrs =  &RsTxBuffer[0];//ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
    TxRunRs = 1;		//���������� ���� �������� ��������
    for(delay = 0; delay < 60; delay ++);
    _U3TXIE = 1;
}

//----------- ISR ���������� �� UART3 Rx ------------------
//----------- ISR ���������� �� UART3 Rx ------------------
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
             Nop();	//�ģ�, ���� Tx FIFO �����//���, ���� Tx FIFO �����
            }
            i = TxFifoRs.wiRsTx;		
            TxFifoRs.buffRsTx[i++] = TEMP_U3RXREG; //������ � Tx FIFO ����� ������ �����//������ � Tx FIFO ����� ������ �����
            TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
//            _DI();
            TxFifoRs.ctRsTx++;
            if (!TxRunRs) {		//���� �������� �� UART Tx �� ��ģ���, �� ..//���� �������� �� UART Tx �� ������, �� ..
                TxRunRs = 1;    //��������� ����� - �������� �������� //��������� ����� - �������� ��������
        //------��������� �������� �� ����������� RS485_I------------------------
        //------��������� �������� �� ����������� RS485_I------------------------
                _RP16R = _RPOUT_U3TX;    // ��� ��� pcb2.2 ��� TD331S485H-A
                U3STAbits.UTXEN = 1;
                ENTX485_I = 0;  // TXD RS485_I ���������� ��������� � ������ TXD RS485
                                // TXD RS485_I ���������� ��������� � ������ TXD RS485
                for(delay = 0; delay < 60; delay ++);
                _U3TXIE = 1;	//���������� ���������� �� UART3 Tx
                                //���������� ���������� �� UART3 Tx
            }
        }
        if(DirRS485.RX3_UART){
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx ++] = TEMP_U3RXREG;   //������ � Rx FIFO ����� ������//������ � Rx FIFO ����� ������
            // ���������� ����� ���� ����� ������ ��������� �������� ������
            // ���������� ����� ���� ����� ������ ��������� �������� ������
            RxFifoRs.wiRsRx = RxFifoRs.wiRsRx % SIZE_BUFFER_RXRS;    /*  ���� ����� ���������� ��� ���� 
                                                                    * RxFifoRs.wi ������ ������ 0
                                                                    * ���� ����� ���������� ��� ���� 
                                                                    * RxFifoRs.wi ������ ������ 0 */
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx] = 0;
            RxFifoRs.ctRsRx ++; // ����������� �� 1 ���������� �� ����������� ����
                                // ����������� �� 1 ���������� �� ����������� ����
        }
    }
}
//-------------------29.11.21------------------------------
UINT8 CounterErrorRs485_I;
//----------- ISR ���������� �� UART2 Rx ------------------
//----------- ISR ���������� �� UART2 Rx ------------------
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
             Nop();	//�ģ�, ���� Tx FIFO �����//���, ���� Tx FIFO �����
            }
            i = TxFifoRs.wiRsTx;		
            TxFifoRs.buffRsTx[i++] = TEMP_U2RXREG; //������ � Tx FIFO ����� ������ �����//������ � Tx FIFO ����� ������ �����
            TxFifoRs.wiRsTx = i % SIZE_BUFFER_TXRS;
            TxFifoRs.ctRsTx++;
            if (!TxRunRs) {     //���� �������� �� UART Tx �� ��ģ���, �� ..//���� �������� �� UART Tx �� ������, �� ..
                TxRunRs = 1;    //��������� ����� - �������� ��������//��������� ����� - �������� ��������
        //------��������� �������� �� ����������� RS485------------------------
        //------��������� �������� �� ����������� RS485------------------------        
                U3STAbits.UTXEN = 1;
                ENTX485 = 1;    // TXD RS485 ���������� ��������� � ������ TXD RS485
                                // TXD RS485 ���������� ��������� � ������ TXD RS485
                for(delay = 0; delay < 60; delay ++);
                _U3TXIE = 1;	//���������� ���������� �� UART Tx
                                //���������� ���������� �� UART Tx
            }
        }
        if(DirRS485.RX2_UART){
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx ++] = TEMP_U2RXREG;   //������ � Rx FIFO ����� ������//������ � Rx FIFO ����� ������
            // ���������� ����� ���� ����� ������ ��������� �������� ������
            // ���������� ����� ���� ����� ������ ��������� �������� ������
            RxFifoRs.wiRsRx = RxFifoRs.wiRsRx % SIZE_BUFFER_RXRS;   /* ���� ����� ���������� ��� ���� 
                                                                    * RxFifoRs.wi ������ ������ 0
                                                                    * ���� ����� ���������� ��� ���� 
                                                                    * RxFifoRs.wi ������ ������ 0 */
            RxFifoRs.buffRsRx[RxFifoRs.wiRsRx] = 0;
            RxFifoRs.ctRsRx ++; // ����������� �� 1 ���������� �� ����������� ����
                                // ����������� �� 1 ���������� �� ����������� ����
        }
    }
}
//-------------------29.11.21------------------------------
//----------- ISR ���������� �� UART3 Tx ------------------
//----------- ISR ���������� �� UART3 Tx ------------------
//#include "FunctionTMR.h"
void __attribute__((interrupt, auto_psv)) _U3TXInterrupt (void)
{
    _U3TXIF = 0;		//������� ����� ���������� TX
    if (TxFifoRs.ctRsTx) {
        TxFifoRs.ctRsTx --;
		U3TXREG = TxFifoRs.buffRsTx[TxFifoRs.riRsTx ++];		//�������� ���� ����
		TxFifoRs.riRsTx = TxFifoRs.riRsTx % SIZE_BUFFER_TXRS;	/* Next read ptr */
    }else{
        _U3TXIE = 0;
        while(!U3STAbits.TRMT); // ������� ����������� ������ ��������
        TxRunRs = 0;		//���������� ���� �������� ��������
        ENTX485 = 0;
        _RP16R = 0;    // ��� ��� pcb2.2 ��� TD331S485H-A
        ENTX485_I = 1;
        U3STAbits.UTXEN = 0;
    }
}
//-------------------29.11.21------------------------------
//����� ������ ��������� � RSR � ��������� ������ ������������ ������
//����� ������ ��������� � RSR � ��������� ������ ������������ ������
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
//OOOOOOOOOO---������� ���������� ���---OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
void Init_IKZ (void)//�������� ����������� ����� RS485 � ��������� ��������� �����������.
{
//    ����� ���� ���
    Reset_IKZ ();
//    �������� ����������� "������"
    TestChenel_IKZ ();
//    ��������� ����������� �������
    SetChenel_IKZ_M ();
//    ���������� �������� ��������� ���� ���
    SaveChenel_IKZ ();
}

void Terminal_IKZ (void)//�������� ����������� ����� RS485 � ������� ������.
{
    
}

void Reset_IKZ (void)//����� ���� ��� � ��������� ���������.
{
//    ����� ����������� �������
    
//    ������� ����� ��������� ���� ���
    //============ ���� ========22.01.20===================================
//    while(ENTX485 || !ENTX485_I); // ������� ��������� ���������� ��������
//    xprintf("? %lu %u ",StatusBU[2].SerialNumber,222);
//    xprintf("%u\r",crc_value);
//    while(ENTX485 || !ENTX485_I); // ������� ��������� ���������� ��������
    //============ ����� ����� ========22.01.20===================================
}

void SaveChenel_IKZ (void)//���������� �������� ��������� ���� ��� � ���������� ����������� �������� RS485.
{
//    ������� ���������� �������� ��������� ���� ���
    
}

void TestChenel_IKZ (void)
{
    
}

void SetChenel_IKZ_M (void)
{
    
}
