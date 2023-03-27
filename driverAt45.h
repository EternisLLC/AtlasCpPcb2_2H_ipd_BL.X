/* 
 * File:   DriverAt45.h
 * Author: Администратор
 *
 * Created on 13 Сентябрь 2018 г., 14:05
 */

#define     bitGIEH     _IPL
#define     GIE_EN      _IPL = 1
#define     GIE_DIS     _IPL = 7
typedef union statusType {
    struct{
        char PageSize    :1;
        char Protect     :1;
        char TypeAt      :4;
        char CompAt      :1;
        char RDY      :1;   
    }AT45bits;
    unsigned char   AT45byte;
}At45statusType;
extern At45statusType At45status;
extern unsigned int    At45PageSize;


void InitAt45(void);
void EraseAt45(void);
void ReadStatusAt45(void);
void MainMemoryArrayRead(unsigned int pageNumber,
                        unsigned int byteNumber,
                        unsigned int CounterByte,
                        unsigned char* data);
void WriteToBufferAt45(const unsigned char numberBuffer,// номер буфера
                        unsigned int addrByte,          // адрес байта
                        unsigned int len,               // количество байт
                        unsigned char* data);
void BufferToMainMemoryPageE (const unsigned char numberBuffer,
                            unsigned int numberPage);
void MainMemoryPageToBuffer (const unsigned char numberBuffer, 
                                unsigned int numberPage);
void ReadFromBufferAt45(const unsigned char numberBuffer,
                        unsigned int addrByte,
                        unsigned int len,
                        unsigned char *data);
UINT32 ReadLongFromAT45(UINT16 page,UINT16);
unsigned char ReadCharFromAt45(UINT16 page, UINT16 byte);