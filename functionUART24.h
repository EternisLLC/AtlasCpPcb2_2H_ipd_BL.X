#define BUFFER_SIZE 128

// UART4 �������� � LCD

typedef struct {
	int		ri, wi, ct;			/* ������ ������, ������ ������, ������� ������ */
	unsigned char	buff[BUFFER_SIZE];	/* FIFO ����� */
} FifoU4;
extern int TxRunLcd;
extern FifoU4 RxFifoU4;
extern FifoU4 TxFifoU4;
void InitUart4 (unsigned long bps);
int uart4_testRx (void);
int uart4_testTx (void);
void uart4_putc (unsigned char d);
unsigned char uart4_getc (void);


