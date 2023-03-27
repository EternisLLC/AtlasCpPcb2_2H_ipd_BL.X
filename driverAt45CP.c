#include    <xc.h>
#include    "alwaysCP_210728.h"
#include    "driverAt45.h"
#include    "funcia_spi.h"

#define CS_AT45             LATDbits.LATD9 // УИЕНБ 2.1 ОПЗБ 43
#define TRIS_CS_AT45        TRISDbits.TRISD9
#define AT45EN              CS_AT45 = 0
#define AT45DIS             CS_AT45 = 1
// ТЕЦЙН ТБВПФЩ SPI
//#define ModeAt45            2

At45statusType At45status;
unsigned int    At45PageSize = 256;
unsigned char MirrorGie;

////+++++++ жхолгйй SPI УПЧНЕЭБА У ДТБКЧЕТПН AT45+++++++++
//
///* ЛПОЖЙЗХТБГЙС НПДХМС SPI	*/
//
//void InitSpi24(void){
//    SPI1CON1 = 0x0000;
//    SPI1CON2 = 0x0000;
//    SPI1STAT = 0x0000; 
//    SPI1CON1bits.CKP = 1;
//    SPI1CON1bits.CKE = 0;
//    SPI1CON1bits.SMP = 1;
//    SPI1CON1bits.PPRE = 3;      //ПУОПЧОПК РТЕДДЕМЙФЕМШ 3-1:1 2-4:1 1-16:1 0-64:1
//    SPI1CON1bits.SPRE = 6;		//ЧФПТЙЮОЩК РТЕДДЕМЙФЕМШ 7-1:1 6-2:1 5-3:1 0-8:1
//    SPI1CON1bits.MSTEN = 1;		//ЧЛМ ТЕЦЙН ?БУФЕТ
//    SPI1CON2bits.SPIBEN = 0;    //1- ЧЛМ. ТБУЫЙТЕООПЗП ВХЖЕТБ 8ВБКФ  0-ВХЖЕТ 1 ВБКФ
//    SPI1STATbits.SPIEN = 0;		//ЧЩЛМ НПДХМ? SPI
//    // РПДЛМАЮЕОЙЕ НПДХМС SPI1 Л ОПЦЛБН РТПГЕУУПТБ
//    _SDI1R = 11;                //SPI MISO ОБ PR11 УИЕНБ 2.1 ОПЗБ 46
//    _RP3R = _RPOUT_SCK1OUT;     //SPI SCK ОБ PR3 УИЕНБ 2.1 ОПЗБ 44
//    _RP12R = _RPOUT_SDO1;       //SPI MOSI ОБ PR12 УИЕНБ 2.1 ОПЗБ 45
//}
//void ChangModeSpi(unsigned char mode){
//    switch(mode){
//        case 1:
//            SPI1CON1bits.CKP = 0;
//            SPI1CON1bits.CKE = 0;
//            SPI1CON1bits.SMP = 0;
//
//            break;
//        case 2: // ТЕЦЙН ДМС бф45
//            SPI1CON1bits.CKP = 1;
//            SPI1CON1bits.CKE = 0;
//            SPI1CON1bits.SMP = 1;
//            break;
//        case 3:// ТЕЦЙН ДМС CC1101
//            SPI1CON1bits.CKP = 0;
//            SPI1CON1bits.CKE = 1;
//            SPI1CON1bits.SMP = 0;
//            break;
//        case 4:
//            SPI1CON1bits.CKP = 1;
//            SPI1CON1bits.CKE = 1;
//            SPI1CON1bits.SMP = 0;
//            break;
//    }
//}
//unsigned char SendByteToSpi (unsigned char Value){
//	SPI1BUF = Value;
//	while (!SPI1STATbits.SPIRBF)continue;
//	return SPI1BUF;
//}
//
//
////+++++++++++лпоег жхолгйк SPI+++++++++ дбмшые дтбкчет AT45

void
InitAt45(void){
    CS_AT45 = 1;
    TRIS_CS_AT45 = out;
    InitSpi24();
}
// БЛФЙЧЙЪБГЙС AT45 ДМС ТБВПФЩ
void
At45Enable(void){
    GIE_DIS;
	if(SPI1EN == 0)SPI1EN = 1;
    AT45EN;
}
// ДЕБЛФЙЧБГЙС AT45
void
At45Disable(void){
    AT45DIS;
    SPI1EN = 0;
}
// ЮФЕОЙЕ ТЕЗЙУФТБ УФБФХУ_________________________________________
void
ReadStatusAt45(void){
    MirrorGie = bitGIEH;
    At45Enable();
    (void)SendByteToSpi(0x57);
    At45status.AT45byte = SendByteToSpi(0);
    At45Disable();
    bitGIEH = MirrorGie;
}
void
EraseAt45(void){
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ПЦЙДБЕН ЗПФПЧОПУФЙ РБНСФЙ Л УФЙТБОЙА 
    }
    MirrorGie = bitGIEH;
    At45Enable();
    (void)SendByteToSpi(0xC7);
    (void)SendByteToSpi(0x94);
    (void)SendByteToSpi(0x80);
    (void)SendByteToSpi(0x9A);
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ПЦЙДБЕН ПЛПОЮБОЙС УФЙТБОЙС
    }
    Nop();
}

void WriteToBufferAt45(const unsigned char numberBuffer,// ОПНЕТ ВХЖЕТБ // номер буфера
                        unsigned int addrByte,          // adr	БДТЕУ ВБКФБ Ч ВХЖЕТЕ ПФ 0 ДП 263// адрес байта
                        unsigned int len,               // len			ЛПМЙЮЕУФЧП ЪБРЙУЩЧБЕНЩИ ВБКФ// количество байт
                        unsigned char* data){           // data_buf[]	НБУУЙЧ ЪБРЙУЩЧБЕНЩИ ДБООЩИ// указатель на данные
    unsigned char p1;
    unsigned char p2;
    unsigned int	i=0;
    p1 = (unsigned char)(addrByte/0xFF);
    p2 = (unsigned char)addrByte;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ПЦЙДБЕН ЗПФПЧОПУФЙ РБНСФЙ// ожидаем готовности памяти
    }
	MirrorGie = bitGIEH;
    At45Enable();
//передаем команду//РЕТЕДБЕН ЛПНБОДХ
    if(numberBuffer == 1){
        (void)SendByteToSpi(0x84);
    }else{
        (void)SendByteToSpi(0x87);
    }
//передаем незначащую последовательность//РЕТЕДБЕН ОЕЪОБЮБЭХА РПУМЕДПЧБФЕМШОПУФШ
	(void)SendByteToSpi(0x00);
//передаем адрес байта в буфере//РЕТЕДБЕН БДТЕУ ВБКФБ Ч ВХЖЕТЕ
	(void)SendByteToSpi(p1);
	(void)SendByteToSpi(p2);
//пердаем i-й байт данных//РЕТДБЕН i-К ВБКФ ДБООЩИ
	for(i=0;i<len;i++){
		(void)SendByteToSpi(*data);
        data ++;
    }
	At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем окончания операции// ПЦЙДБЕН ПЛПОЮБОЙС ПРЕТБГЙЙ
    }
}
void ReadFromBufferAt45(const unsigned char numberBuffer,
                        unsigned int addrByte,
                        unsigned int len,
                        unsigned char *data){
    unsigned int	i=0;
    unsigned char p1;
    unsigned char p2;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем готовности памяти
    }
	MirrorGie = bitGIEH;
    At45Enable();
    p1 = (unsigned char)(addrByte >> 8);
    p2 = (unsigned char)addrByte;
//передаем команду
	if(numberBuffer == 1){
		(void)SendByteToSpi(0xD4);//0x54
	}else{
		(void)SendByteToSpi(0xD6);//0x56
	}
//передаем незначащую последовательность
	(void)SendByteToSpi(0x00);
//передаем адрес байта в буфере
	(void)SendByteToSpi(p1);
	(void)SendByteToSpi(p2);
//передаем незначащую последовательность
	(void)SendByteToSpi(0x00);
//читаем i-й байт данных и помещаем в i
	for(i=0;i<len;i++){
		*data = SendByteToSpi(0x00);
		data ++;
	}
	At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем окончания операции
    }
}
//запись буфера в страницу основной памяти со стиранием___________________
void BufferToMainMemoryPageE (const unsigned char numberBuffer,
                            unsigned int numberPage){
//запись со стиранием
// adr адрес страницы памяти
    unsigned int temp;
    unsigned char p1;
    unsigned char p2;
// готовим адресную последовательность
    temp = numberPage;
    temp = temp << 1;
    p1 = (unsigned char)(temp >> 8);
    p2 = (unsigned char)temp;
// запрещаем прерывания
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем готовности памяти
    }    
	MirrorGie = bitGIEH;
    At45Enable();
//передаем команду записи
	if( numberBuffer == 1){
        SendByteToSpi(0x83);
    }else {
        SendByteToSpi(0x86);
    }
//передаем адрес страницы
    SendByteToSpi(p1);
    SendByteToSpi(p2);
//передаем незначащую последовательность
	SendByteToSpi(0x00);
// разрешаем прерывания    
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем окончания операции
    }
}
//запись страницы основной памяти в буфер_______________________________
void MainMemoryPageToBuffer (const unsigned char numberBuffer, 
                                unsigned int numberPage){
//читаем  всегда через 2-й буфер
// adr адрес страницы памяти
    unsigned char p1;
    unsigned char p2;
unsigned int temp;
// готовим последовательность
    temp = numberPage;
    temp = temp << 1;
    p1 = (unsigned char)(temp >> 8);
    p2 = (unsigned char)temp;
// запрещаем прерывания
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем готовности памяти
    }    
	MirrorGie = bitGIEH;
    At45Enable();
//передаем команду
	if(numberBuffer == 1){
        SendByteToSpi(0x53);
    }else {
        SendByteToSpi(0x55);
    }
//передаем адрес страницы
    SendByteToSpi(p1);
    SendByteToSpi(p2);
//передаем незначащую последовательность
	SendByteToSpi(0x00);
// разрешаем прерывания    
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем окончания операции
    }
}
void
MainMemoryArrayRead(unsigned int pageNumber,
                    unsigned int byteNumber,
                    unsigned int CounterByte,
                    unsigned char* data){   // массив куда поместить данные
    unsigned int temp;
    unsigned char p1;
    unsigned char p2;
    temp = pageNumber << 1;// освобождаем место для старшего бита номера байта
    temp = temp + (byteNumber/256);// добавляем старший бит номера байта
    p1 = (unsigned char)(temp >> 8);
    p2 = (unsigned char)temp;
    // запрещаем прерывания
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем готовности памяти
    }    
	MirrorGie = bitGIEH;
    At45Enable();
    SendByteToSpi(0xE8);
    SendByteToSpi(p1);
    SendByteToSpi(p2);
    SendByteToSpi((unsigned char)byteNumber);
    SendByteToSpi(0);
    SendByteToSpi(0);
    SendByteToSpi(0);
    SendByteToSpi(0);
    temp = 0;
    while(temp < CounterByte){
        *data = SendByteToSpi(0);
        temp ++;
        data ++;
    }
// разрешаем прерывания    
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ожидаем окончания операции
    }
}
UINT32 ReadLongFromAT45(UINT16 page,UINT16 byte){
    UINT32 TempUINT32;
    MainMemoryArrayRead(page,byte,4,(unsigned char*)&TempUINT32);
    return TempUINT32;
}
unsigned char ReadCharFromAt45(UINT16 page, UINT16 byte){
    UINT8 TempUINT8;
    MainMemoryArrayRead(page,byte,1,(unsigned char*)&TempUINT8);
    return TempUINT8;
}