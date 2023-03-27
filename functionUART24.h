#define BUFFER_SIZE 128

// UART4 ТБВПФБЕФ У LCD

typedef struct {
	int		ri, wi, ct;			/* индекс чтения, индекс записи, счётчик данных */
	unsigned char	buff[BUFFER_SIZE];	/* FIFO буфер */
} FifoU4;
extern int TxRunLcd;
extern FifoU4 RxFifoU4;
extern FifoU4 TxFifoU4;
void InitUart4 (unsigned long bps);
int uart4_testRx (void);
int uart4_testTx (void);
void uart4_putc (unsigned char d);
unsigned char uart4_getc (void);


