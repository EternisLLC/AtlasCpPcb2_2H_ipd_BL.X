#include    <xc.h>
#include    "alwaysCP_210728.h"
#include    "driverAt45.h"
#include    "funcia_spi.h"

#define CS_AT45             LATDbits.LATD9 // ����� 2.1 ���� 43
#define TRIS_CS_AT45        TRISDbits.TRISD9
#define AT45EN              CS_AT45 = 0
#define AT45DIS             CS_AT45 = 1
// ����� ������ SPI
//#define ModeAt45            2

At45statusType At45status;
unsigned int    At45PageSize = 256;
unsigned char MirrorGie;

////+++++++ ������� SPI �������� � ��������� AT45+++++++++
//
///* ������������ ������ SPI	*/
//
//void InitSpi24(void){
//    SPI1CON1 = 0x0000;
//    SPI1CON2 = 0x0000;
//    SPI1STAT = 0x0000; 
//    SPI1CON1bits.CKP = 1;
//    SPI1CON1bits.CKE = 0;
//    SPI1CON1bits.SMP = 1;
//    SPI1CON1bits.PPRE = 3;      //�������� ������������ 3-1:1 2-4:1 1-16:1 0-64:1
//    SPI1CON1bits.SPRE = 6;		//��������� ������������ 7-1:1 6-2:1 5-3:1 0-8:1
//    SPI1CON1bits.MSTEN = 1;		//��� ����� ?�����
//    SPI1CON2bits.SPIBEN = 0;    //1- ���. ������������ ������ 8����  0-����� 1 ����
//    SPI1STATbits.SPIEN = 0;		//���� �����? SPI
//    // ����������� ������ SPI1 � ������ ����������
//    _SDI1R = 11;                //SPI MISO �� PR11 ����� 2.1 ���� 46
//    _RP3R = _RPOUT_SCK1OUT;     //SPI SCK �� PR3 ����� 2.1 ���� 44
//    _RP12R = _RPOUT_SDO1;       //SPI MOSI �� PR12 ����� 2.1 ���� 45
//}
//void ChangModeSpi(unsigned char mode){
//    switch(mode){
//        case 1:
//            SPI1CON1bits.CKP = 0;
//            SPI1CON1bits.CKE = 0;
//            SPI1CON1bits.SMP = 0;
//
//            break;
//        case 2: // ����� ��� ��45
//            SPI1CON1bits.CKP = 1;
//            SPI1CON1bits.CKE = 0;
//            SPI1CON1bits.SMP = 1;
//            break;
//        case 3:// ����� ��� CC1101
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
////+++++++++++����� ������� SPI+++++++++ ������ ������� AT45

void
InitAt45(void){
    CS_AT45 = 1;
    TRIS_CS_AT45 = out;
    InitSpi24();
}
// ����������� AT45 ��� ������
void
At45Enable(void){
    GIE_DIS;
	if(SPI1EN == 0)SPI1EN = 1;
    AT45EN;
}
// ����������� AT45
void
At45Disable(void){
    AT45DIS;
    SPI1EN = 0;
}
// ������ �������� ������_________________________________________
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
        ReadStatusAt45();   // ������� ���������� ������ � �������� 
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
        ReadStatusAt45();   // ������� ��������� ��������
    }
    Nop();
}

void WriteToBufferAt45(const unsigned char numberBuffer,// ����� ������ // ����� ������
                        unsigned int addrByte,          // adr	����� ����� � ������ �� 0 �� 263// ����� �����
                        unsigned int len,               // len			���������� ������������ ����// ���������� ����
                        unsigned char* data){           // data_buf[]	������ ������������ ������// ��������� �� ������
    unsigned char p1;
    unsigned char p2;
    unsigned int	i=0;
    p1 = (unsigned char)(addrByte/0xFF);
    p2 = (unsigned char)addrByte;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ���������� ������// ������� ���������� ������
    }
	MirrorGie = bitGIEH;
    At45Enable();
//�������� �������//�������� �������
    if(numberBuffer == 1){
        (void)SendByteToSpi(0x84);
    }else{
        (void)SendByteToSpi(0x87);
    }
//�������� ���������� ������������������//�������� ���������� ������������������
	(void)SendByteToSpi(0x00);
//�������� ����� ����� � ������//�������� ����� ����� � ������
	(void)SendByteToSpi(p1);
	(void)SendByteToSpi(p2);
//������� i-� ���� ������//������� i-� ���� ������
	for(i=0;i<len;i++){
		(void)SendByteToSpi(*data);
        data ++;
    }
	At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ��������� ��������// ������� ��������� ��������
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
        ReadStatusAt45();   // ������� ���������� ������
    }
	MirrorGie = bitGIEH;
    At45Enable();
    p1 = (unsigned char)(addrByte >> 8);
    p2 = (unsigned char)addrByte;
//�������� �������
	if(numberBuffer == 1){
		(void)SendByteToSpi(0xD4);//0x54
	}else{
		(void)SendByteToSpi(0xD6);//0x56
	}
//�������� ���������� ������������������
	(void)SendByteToSpi(0x00);
//�������� ����� ����� � ������
	(void)SendByteToSpi(p1);
	(void)SendByteToSpi(p2);
//�������� ���������� ������������������
	(void)SendByteToSpi(0x00);
//������ i-� ���� ������ � �������� � i
	for(i=0;i<len;i++){
		*data = SendByteToSpi(0x00);
		data ++;
	}
	At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ��������� ��������
    }
}
//������ ������ � �������� �������� ������ �� ���������___________________
void BufferToMainMemoryPageE (const unsigned char numberBuffer,
                            unsigned int numberPage){
//������ �� ���������
// adr ����� �������� ������
    unsigned int temp;
    unsigned char p1;
    unsigned char p2;
// ������� �������� ������������������
    temp = numberPage;
    temp = temp << 1;
    p1 = (unsigned char)(temp >> 8);
    p2 = (unsigned char)temp;
// ��������� ����������
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ���������� ������
    }    
	MirrorGie = bitGIEH;
    At45Enable();
//�������� ������� ������
	if( numberBuffer == 1){
        SendByteToSpi(0x83);
    }else {
        SendByteToSpi(0x86);
    }
//�������� ����� ��������
    SendByteToSpi(p1);
    SendByteToSpi(p2);
//�������� ���������� ������������������
	SendByteToSpi(0x00);
// ��������� ����������    
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ��������� ��������
    }
}
//������ �������� �������� ������ � �����_______________________________
void MainMemoryPageToBuffer (const unsigned char numberBuffer, 
                                unsigned int numberPage){
//������  ������ ����� 2-� �����
// adr ����� �������� ������
    unsigned char p1;
    unsigned char p2;
unsigned int temp;
// ������� ������������������
    temp = numberPage;
    temp = temp << 1;
    p1 = (unsigned char)(temp >> 8);
    p2 = (unsigned char)temp;
// ��������� ����������
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ���������� ������
    }    
	MirrorGie = bitGIEH;
    At45Enable();
//�������� �������
	if(numberBuffer == 1){
        SendByteToSpi(0x53);
    }else {
        SendByteToSpi(0x55);
    }
//�������� ����� ��������
    SendByteToSpi(p1);
    SendByteToSpi(p2);
//�������� ���������� ������������������
	SendByteToSpi(0x00);
// ��������� ����������    
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ��������� ��������
    }
}
void
MainMemoryArrayRead(unsigned int pageNumber,
                    unsigned int byteNumber,
                    unsigned int CounterByte,
                    unsigned char* data){   // ������ ���� ��������� ������
    unsigned int temp;
    unsigned char p1;
    unsigned char p2;
    temp = pageNumber << 1;// ����������� ����� ��� �������� ���� ������ �����
    temp = temp + (byteNumber/256);// ��������� ������� ��� ������ �����
    p1 = (unsigned char)(temp >> 8);
    p2 = (unsigned char)temp;
    // ��������� ����������
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ���������� ������
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
// ��������� ����������    
    At45Disable();
    bitGIEH = MirrorGie;
    ReadStatusAt45();
    while(At45status.AT45bits.RDY == 0){
        ReadStatusAt45();   // ������� ��������� ��������
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