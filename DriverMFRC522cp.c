#include    <xc.h>
#include    <stdio.h>
#include    <stdbool.h>
#include    "alwaysCP_210728.h"
#include    "DriverMFRC522cp.h"
#include    "funcia_spi.h"
#define _DI()		__asm__ volatile("disi #0x3FFF")
#define _EI()		__asm__ volatile("disi #0")

Flag522_t FlagMFRC522;
unsigned char counterReadCart;
unsigned char MFRC522_WriteByte(unsigned char Address,unsigned char Value)
{
	unsigned char Temp;
    CS_RC522 = 0;
    SendByteToSpi((unsigned char)((Address << 1) & 0x7E));
	Temp = SendByteToSpi(Value);
    CS_RC522 = 1;
	return Temp;
}
void MFRC522_Reset(void){
    _DI();
	MFRC522_WriteByte(CommandReg, MFRC522_SOFTRESET);
    _EI();
}
unsigned char MFRC522_ReadByte(unsigned char Address)
{
	unsigned char Temp;
    CS_RC522 = 0;
    SendByteToSpi((unsigned char)(((Address<<1)&0x7E) | 0x80));
	Temp = SendByteToSpi(0);
    CS_RC522 = 1;
	return Temp;
}
//
//
void MFRC522_Clear_Bit( char addr, unsigned char mask )
{     unsigned int  tmp =0x00;
      tmp = MFRC522_ReadByte( addr ) ;
      MFRC522_WriteByte( addr, tmp & (unsigned char)(~mask) );
}
void MFRC522_Set_Bit( char addr, char mask )
{   unsigned int  tmp =0x0;
    tmp = MFRC522_ReadByte( addr ) ;
    MFRC522_WriteByte( addr, tmp | mask );
}
//
void MFRC522_AntennaOn(void)
{                                               
MFRC522_Set_Bit( TxControlReg, 0x03 ); 
}
void MFRC522_AntennaOff(void)
{
 MFRC522_Clear_Bit( TxControlReg, 0x03 );
}
//
void MFRC522_Init(void)      
{
// 02.12.22   ChangModeSpi(3);
    ChangModeSpi(2);
    unsigned char result = 0;
    TRIS_CS_RC522 = 0;
    MFRC522_Reset();
    CounterDelayMs = 2;
    while(CounterDelayMs);
    result = MFRC522_WriteByte( TModeReg, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler  8D-orig
    CounterDelayMs = 2;
    while(CounterDelayMs);
    result = MFRC522_WriteByte( TPrescalerReg, 0x3E ); //TModeReg[3..0] + TPrescalerReg
    CounterDelayMs = 2;
    while(CounterDelayMs);
    result = MFRC522_WriteByte( TReloadRegL, 30 );
    CounterDelayMs = 2;
    while(CounterDelayMs);
    result = MFRC522_WriteByte( TReloadRegH, 0 );
    CounterDelayMs = 2;
    while(CounterDelayMs);
    result = MFRC522_WriteByte( TxAutoReg, 0x40 );    //100%ASK
    CounterDelayMs = 2;
    while(CounterDelayMs);
    result = MFRC522_WriteByte( ModeReg, 0x3D );      // CRC valor inicial de 0x6363
    CounterDelayMs = 2;
    while(CounterDelayMs);
    MFRC522_AntennaOff() ;
    CounterDelayMs = 2;
    while(CounterDelayMs);
    MFRC522_AntennaOn();
    CounterDelayMs = 2;
    while(CounterDelayMs);
//    MFRC522_SoftPowerDown();
    CounterDelayMs = 2;
    while(CounterDelayMs);
    FlagMFRC522._ON = 1;
    ChangModeSpi(2);
}
//---------------------------------------------------------
//   @brief Отправляет команду тегу.
//
//   @param cmd Команда MFRC522 для отправки команды тегу.
//   @param data Данные, необходимые для выполнения команды.
//   @param dlen Длина данных.
//   @param result Результат, возвращаемый тегом.
//   @param rlen Количество допустимых битов в результирующем значении.
//
//   @returns Возвращает статус вычисления.
//            MI_ERR, если что-то пошло не так,
//            MI_NOTAGERR, если не было тега для отправки команды.
//            MI_OK, если все прошло нормально.
//
//Переменная, используемая для подтверждения правильного обнаружения тега 
//----------------------------------------------------------
//
int commandTag(unsigned char cmd, unsigned char *data, int dlen, unsigned char *result, int *rlen) 
{
  int status = MI_ERR;
  unsigned char irqEn = 0x00;
  unsigned char waitIRq = 0x00;
  unsigned char lastBits, n;
  int i;

  switch (cmd) {
  case MFRC522_AUTHENT:
    irqEn = 0x12;
    waitIRq = 0x10;
    break;
  case MFRC522_TRANSCEIVE:
    irqEn = 0x77;
    waitIRq = 0x30;
    break;
  default:
    break;
  }

  MFRC522_WriteByte(CommIEnReg, (unsigned char)(irqEn|0x80));    // interrupt request
  MFRC522_Clear_Bit(CommIrqReg, 0x80);             // Clear all interrupt requests bits.
  MFRC522_Set_Bit(FIFOLevelReg, 0x80);             // FlushBuffer=1, FIFO initialization.

  MFRC522_WriteByte(CommandReg, MFRC522_IDLE);  // No action, cancel the current command.

  // Write to FIFO
  for (i=0; i < dlen; i++) {
    MFRC522_WriteByte(FIFODataReg, data[i]);
  }

  // Execute the command.
  MFRC522_WriteByte(CommandReg, cmd);
  if (cmd == MFRC522_TRANSCEIVE) {
    MFRC522_Set_Bit(BitFramingReg, 0x80);  // StartSend=1, transmission of data starts
  }

  // Waiting for the command to complete so we can receive data.
  i = 25; // Max wait time is 25ms.
  do {
//    __delay_ms(1);
    CounterDelayMs = 2;
    while(CounterDelayMs);
    // CommIRqReg[7..0]
    // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = MFRC522_ReadByte(CommIrqReg);
    i--;
  } while ((i!=0) && !(n&0x01) && !(n&waitIRq));

  MFRC522_Clear_Bit(BitFramingReg, 0x80);  // StartSend=0

  if (i != 0) { // Request did not time out.
    if(!(MFRC522_ReadByte(ErrorReg) & 0x1D)) {  // BufferOvfl Collerr CRCErr ProtocolErr
      status = MI_OK;
      if (n & irqEn & 0x01) {
        status = MI_NOTAGERR;
      }

      if (cmd == MFRC522_TRANSCEIVE) {
        n = MFRC522_ReadByte(FIFOLevelReg);
        lastBits = (unsigned char)(MFRC522_ReadByte(ControlReg) & 0x07);
        if (lastBits) {
          *rlen = (n-1)*8 + lastBits;
        } else {
          *rlen = n*8;
        }

        if (n == 0) {
          n = 1;
        }

        if (n > MAX_LEN) {
          n = MAX_LEN;
        }

        // Reading the received data from FIFO.
        for (i=0; i<n; i++) {
          result[i] = MFRC522_ReadByte(FIFODataReg);
        }
      }
    } else {
      status = MI_ERR;
    }
  }
  return status;
}
//
//------------------------------------------------------------
//  @brief   Checks to see if there is a tag in the vicinity.
//
//  @param   mode  The mode we are requsting in.
//  @param   type  If we find a tag, this will be the type of that tag.
//                 0x4400 = Mifare_UltraLight
//                 0x0400 = Mifare_One(S50)
//                 0x0200 = Mifare_One(S70)
//                 0x0800 = Mifare_Pro(X)
//                 0x4403 = Mifare_DESFire
//
//  @returns Returns the status of the request.
//           MI_ERR        if something went wrong,
//           MI_NOTAGERR   if there was no tag to send the command to.
//           MI_OK         if everything went OK.
//------------------------------------------------------
//   @brief Проверяет, нет ли поблизости метки.
//
//   @param mode Режим, в котором мы запрашиваем.
//   @param type Если мы найдем тег, это будет тип этого тега.
//                  0x4400 = Mifare_UltraLight
//                  0x0400 = Mifare_One (S50)
//                  0x0200 = Mifare_One (S70)
//                  0x0800 = Mifare_Pro (X)
//                  0x4403 = Mifare_DESFire
//
//   @returns Возвращает статус запроса.
//            MI_ERR, если что-то пошло не так,
//            MI_NOTAGERR, если не было тега для отправки команды.
//            MI_OK, если все прошло нормально. 
/**************************************************************************/
int requestTag(unsigned char mode, unsigned char *data)
{
  int status, len;
  MFRC522_WriteByte(BitFramingReg, 0x07);  // TxLastBists = BitFramingReg[2..0]

  data[0] = mode;
  status = commandTag(MFRC522_TRANSCEIVE, data, 1, data, &len);

  if ((status != MI_OK) || (len != 0x10)) {
    status = MI_ERR;
  }
  return status;
}
//
//------------------------------------------------------------------------
//   @brief Обрабатывает коллизии, которые могут произойти, если есть несколько
//            доступных тегов.
//
//   @param serial Серийный номер тега.
//
//   @returns Возвращает статус обнаружения столкновения.
//            MI_ERR, если что-то пошло не так,
//            MI_NOTAGERR, если не было тега для отправки команды.
//            MI_OK, если все прошло нормально. 
//----------------------------------------------------------------
int antiCollision(unsigned char *serial) 
{
  int status, i, len;
  unsigned char check = 0x00;

  MFRC522_WriteByte(BitFramingReg, 0x00);  // TxLastBits = BitFramingReg[2..0]
  serial[0] = MF1_ANTICOLL;
  serial[1] = 0x20;
  status = commandTag(MFRC522_TRANSCEIVE, serial, 2, serial, &len);
  len = len / 8; // len is in bits, and we want each unsigned char.
  if (status == MI_OK) {
    // The checksum of the tag is the ^ of all the values.
    for (i = 0; i < len-1; i++) {
      check ^= serial[i];
    }
    // The checksum should be the same as the one provided from the
    // tag (serial[4]).
    if (check != serial[i]) {
      status = MI_ERR;
    }
  }
  return status;
}
///**
// * Get the current MFRC522 Receiver Gain (RxGain[2:0]) value.
// * See 9.3.3.6 / table 98 in http://www.nxp.com/documents/data_sheet/MFRC522.pdf
// * NOTE: Return value scrubbed with (0x07<<4)=01110000b as RCFfgReg may use reserved bits.
// * 
// * @return Value of the RxGain, scrubbed to the 3 bits used.
// *----------------------------------------------------------------------------------------- 
//  * Получить текущее значение усиления приемника MFRC522 (RxGain [2: 0]).
//  * См. 9.3.3.6 / таблица 98 в http://www.nxp.com/documents/data_sheet/MFRC522.pdf
//  * ПРИМЕЧАНИЕ. Возвращаемое значение очищается с помощью (0x07 << 4) = 01110000b, поскольку RCFfgReg 
//  * может использовать зарезервированные биты.
//  *
//  * @return Значение RxGain, очищенное до 3 используемых бит. 
// */
////int MFRC522_GetAntennaGain(void) 
////{
////	return MFRC522_ReadByte(RFCfgReg) & (0x07<<4);
////}
//
///**
// * Set the MFRC522 Receiver Gain (RxGain) to value specified by given mask.
// * See 9.3.3.6 / table 98 in http://www.nxp.com/documents/data_sheet/MFRC522.pdf
// * NOTE: Given mask is scrubbed with (0x07<<4)=01110000b as RCFfgReg may use reserved bits.
// *------------------------------------------------------------------------------------- 
//  * Установите усиление приемника MFRC522 (RxGain) на значение, указанное в данной маске.
//  * См. 9.3.3.6 / таблица 98 в http://www.nxp.com/documents/data_sheet/MFRC522.pdf
//  * ПРИМЕЧАНИЕ. Данная маска очищается с помощью (0x07 << 4) = 01110000b, поскольку RCFfgReg 
//  * может использовать зарезервированные биты. 
// */
////    000 18 dB
////    001 23 dB
////    010 18 dB
////    011 23 dB
////    100 33 dB
////    101 38 dB
////    110 43 dB
////    111 48 dB
//
////void MFRC522_SetAntennaGain(unsigned char mask) 
////{
////	if (MFRC522_GetAntennaGain() != mask) {						// only bother if there is a change
////		MFRC522_Clear_Bit(RFCfgReg, (0x07<<4));		// clear needed to allow 000 pattern
////		MFRC522_Set_Bit(RFCfgReg, (unsigned char)(mask & (0x07<<4)));	// only set RxGain[2:0] bits
////	}
////}
////    000 18 dB
////    001 23 dB
////    010 18 dB
////    011 23 dB
////    100 33 dB
////    101 38 dB
////    110 43 dB
////    111 48 dB
//
void MFRC522_SoftPowerDown(void)//Note : Only soft power down mode is available throught software
{
    if(!FlagMFRC522._SoftPower)return;
    unsigned char val = MFRC522_ReadByte(CommandReg); // Read state of the command register 
    val |= (1<<4);// set PowerDown bit ( bit 4 ) to 1 
    MFRC522_WriteByte(CommandReg, val);//write new value to the command register
    FlagMFRC522._SoftPower = 0;
}
//
void MFRC522_SoftPowerUp(void)
{
    if(FlagMFRC522._SoftPower)return;
    unsigned char val = MFRC522_ReadByte(CommandReg); // Read state of the command register 
	val &= ~(1<<4);// set PowerDown bit ( bit 4 ) to 0 
	MFRC522_WriteByte(CommandReg, val);//write new value to the command register
    FlagMFRC522._SoftPower = 1;
}
//
//
int readRFID_1(unsigned char * tagData, unsigned char *outData)
{
    unsigned char i;
    int status;
    // Get anti-collision value to properly read information from the Tag
    status = antiCollision(tagData);
    *outData = 0;
    if(status)return status;
    for(i=0;i<4;i++){
        *outData = *tagData;
        tagData ++;
        outData ++;
    }
    return status;
}
int ReadCardRC522_1(unsigned char *outData)
{
    unsigned char TagData_1[5]; 
    int FoundTag; // Variable used to check if Tag was found
    int status = 0;
    //  Check to see if a Tag was detected
    // If yes, then the variable FoundTag will contain "MI_OK"
    FoundTag = requestTag(MF1_REQIDL, TagData_1);
    if (FoundTag == MI_OK)
    {
        if(readRFID_1(TagData_1,outData) == MI_OK)
        {
            status = 0;
        }else status = 1;
    }else 
    {
        status = 2;
    }
    return status;
}
//
//==========
Flag522_t FlagMFRC522;
serialKid_t SerialNumberKid[11];
////==========
unsigned char version;
void ReadSerialNumberKID(unsigned char * tempSn){
    unsigned char ErrorRegRC522;
    
// 02.12.22   ChangModeSpi(3);
    ChangModeSpi(2);
    if(!FlagMFRC522._ReadCart){
        ChangModeSpi(2);
        return;
    }
    if(!FlagMFRC522._ON)
    {
        MFRC522_Init();
        version = MFRC522_ReadByte(VersionReg);
        if(version != 0x88 && version != 0x90 && version != 0x91 && version != 0x92)
        FlagMFRC522._ON = 0;
        else FlagMFRC522._ON = 1;
    }
    if(!FlagMFRC522._SoftPower ){
        MFRC522_SoftPowerUp(); // Включение модуля RC522(режим чтения)
    }
    if(!ReadCardRC522_1(tempSn)){
        FlagMFRC522._ReadCart = 0;
        FlagMFRC522._newCart = 1; // получен серийный номер карты
    }else{ //серийный номер не прочитан
        ErrorRegRC522 = MFRC522_ReadByte(ErrorReg);
        if(ErrorRegRC522){
//            printf("RC522_reinit\r");
            MFRC522_Init();

        }
    }
    if(!FlagMFRC522._ReadCart)MFRC522_SoftPowerDown(); // Выключение модуля RC522(карта прочитана)
    ChangModeSpi(2);
}
