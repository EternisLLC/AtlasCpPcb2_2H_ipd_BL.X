/* 
 * File:   UART24.h
 * Author: 11
 *
 * Created on 12 ������� 2020 �., 12:20
 */

#ifndef UART24_H
#define	UART24_H

#ifdef	__cplusplus
extern "C" {
#endif
    

    
    void InitUart1 (unsigned long bps);
    void SetDirectUART1(unsigned int dir);
    int uart1_testRx (void);
    unsigned char uart1_getc (void);
    void SelectDirUart(void);
    extern char TxRunU1;		// ���� - ������� ��������
    void InitRS485Terminal (unsigned long bps);
    void InitUSBTerminal (unsigned long bps);
    
//    #define BUFFER_SIZE_RS 64
    #define SIZE_BUFFER_RXRS 256
    #define SIZE_BUFFER_TXRS 256
    #define	ENTX485             _LATB13     // Rb13 ����� ������������ ������� ���������� �������� �� RS485
                                            // Rb13 ����� ������������ ������� ���������� �������� �� RS485
    #define	TRIS_ENTX485        _TRISB13
    #define	ENTX485_I           _LATF4      // RD8 ����� ������������ ������� ���������� �������� �� RS485_I//---------29.11.21-----------
                                            // RD8 ����� ������������ ������� ���������� �������� �� RS485_I//---------29.11.21-----------
    #define	TRIS_ENTX485_ISO      _TRISF4 //---------29.11.21-----------
    #define _USE_XFUNC_OUT	1	/* 1: Use output functions */
    #define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */
    #define _USE_XFUNC_IN	1	/* 1: Use input function */
    #define	_LINE_ECHO		1	/* 1: Echo back input chars in xgets function */
    #define DW_CHAR		sizeof(char)
    #define DW_SHORT	sizeof(short)
    #define DW_LONG		sizeof(long)

    #define OPEN_UART_TX2()     DirRS485.UART_TX2 = 1//���� ���������� �����������--------29.11.21---------
    #define OPEN_UART_TX3()     DirRS485.UART_TX3 = 1//���� ���������� �����������--------29.11.21---------
    #define OPEN_RX2_UART()     DirRS485.RX2_UART = 1//���� ���������� �����������--------29.11.21---------
    #define OPEN_RX3_UART()     DirRS485.RX3_UART = 1//���� ���������� �����������--------29.11.21---------
    #define CLOSE_UART_TX2()    DirRS485.UART_TX2 = 0//���� ���������� �����������--------29.11.21---------
    #define CLOSE_UART_TX3()    DirRS485.UART_TX3 = 0//���� ���������� �����������--------29.11.21---------
    #define CLOSE_RX2_UART()    DirRS485.RX2_UART = 0//���� ���������� �����������--------29.11.21---------
    #define CLOSE_RX3_UART()    DirRS485.RX3_UART = 0//���� ���������� �����������--------29.11.21---------
    
    #define     OPEN_RX2_TX3()  DirRS485.RX2_TX3 = 1
    #define     OPEN_RX3_TX2()  DirRS485.RX3_TX2 = 1
    #define     CLOSE_RX2_TX3() DirRS485.RX2_TX3 = 0
    #define     CLOSE_RX3_TX2() DirRS485.RX3_TX2 = 0
    

//    void InitRS485Terminal (unsigned long bps);
    unsigned char Crc8(unsigned char *pcBlock, unsigned char len);
    int RS485_testRx (void);
    unsigned char RS485_getc (void);
    void xprintf (			/* Put a formatted string to the default device */
        const char*	fmt,	/* Pointer to the format string */
        ...					/* Optional arguments */
    );
    void ClrFifo(void);
    extern unsigned char crc_value;
    void uart1_putc (unsigned char d);
    
    typedef struct {
        int		riRsRx, wiRsRx, ctRsRx;			/* ������ ������, ������ ������, ������� ������ 
                                                 * ������ ������, ������ ������, �ޣ���� ������ */
        unsigned char	buffRsRx[SIZE_BUFFER_RXRS];	/* FIFO ����� */
    }sructRxRsFifo;
    typedef struct {
        int		riRsTx, wiRsTx, ctRsTx;			/* ������ ������, ������ ������, ������� ������ 
                                                * ������ ������, ������ ������, �ޣ���� ������ */
        unsigned char	buffRsTx[SIZE_BUFFER_TXRS];	/* FIFO ����� */
    }sructTxRsFifo; 
    extern sructRxRsFifo RxFifoRs;
    extern UINT8 DirectUart;
    
    extern unsigned char RsTxBuffer[256];
    extern unsigned char RsTxCrc;
    extern unsigned int RsTxLength;
    extern unsigned char* pctxrs;
    
    extern UINT8 CounterErrorRs485;
    extern UINT8 CounterErrorRs485_I;
    //----------------------------24.11.21------------------------------------------    
    typedef union {
        struct {
            unsigned char RX3_TX2       :1; //����������� ����������� �������� ������
            unsigned char RX2_TX3       :1; //����������� ����������� �������� ������
            unsigned char UART_TX3      :1; //����������� ����������� �������� ������
            unsigned char UART_TX2      :1; //����������� ����������� �������� ������
            unsigned char RX3_UART      :1; //����������� ����������� �������� ������
            unsigned char RX2_UART      :1; //����������� ����������� �������� ������
            unsigned char RX3_OK        :1; //������ ���� �� RS485(������������ � RS485_putc)
            unsigned char RX2_OK        :1; //������ ���� �� RS485_I(������������ � RS485_putc)
        };
        unsigned char    DirRS;
    }DirRS;
    extern DirRS DirRS485;
    
    typedef union {
        struct {
            unsigned char RX3_ERROR     :1; //��� ������ �� RS485//��� ������ �� RS485
            unsigned char RX2_ERROR     :1; //��� ������ �� RS485_I//��� ������ �� RS485_I
            unsigned char RX3_OFF       :1; //������ ������ �� RS485//������ ������ �� RS485
            unsigned char RX2_OFF       :1; //������ ������ �� RS485_I//������ ������ �� RS485_I
//            unsigned char RX3_UART      :1; //����������� ����������� �������� ������//����������� ����������� �������� ������
//            unsigned char RX2_UART      :1; //����������� ����������� �������� ������//����������� ����������� �������� ������
//            unsigned char RX3_OK        :1; //������ ���� �� RS485(������������ � RS485_putc)//������ ���� �� RS485(������������ � RS485_putc)
//            unsigned char RX2_OK        :1; //������ ���� �� RS485_I(������������ � RS485_putc)//������ ���� �� RS485_I(������������ � RS485_putc)
        };
        unsigned char    ErrorDirRS;
    }ErrorRS;
    extern ErrorRS ErrorDirRS485;
    extern unsigned char TxRunRs;		// ���� - ������� ��������
    //extern unsigned char CurrentModeRs;
    void Init_IKZ (void);
    void Terminal_IKZ (void);
    void Reset_IKZ (void);
    void SaveChenel_IKZ (void);
    void TestChenel_IKZ (void);
    void SetChenel_IKZ_M (void);
    void TransmittRsPacket(void);
    void SelectModeRs485(unsigned char mode);
//------------------------------------------------------------------------------
#ifdef	__cplusplus
}
#endif

#endif	/* UART24_H */

