/* 
 * File:   function.h
 * Author: 11
 *
 * Created on 31 марта 2020 г., 14:52
 */

#ifndef FUNCTION_H
#define	FUNCTION_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    // изм. 31.08.22
    #define MANUAL  192
    #define LOCK    128
    #define AUTO    0
        typedef union{
        struct{
            UINT16 KeyStart         :1;
            UINT16 KeyStartShort    :1;
            UINT16 KeyStartLong     :1;
            UINT16 KeySoundShort    :1;
            UINT16 KeySoundLong     :1;
            UINT16 KeyAutoShort     :1;
            UINT16 KeyAutoLong      :1;
            UINT16  KeyReset        :1;
            UINT16  reserv          :8;
        };
        UINT16 FlagKeyStatus;
    }FlagKey_t;
    extern FlagKey_t FlagKey;
    // определение типов 
    typedef struct{
        UINT16   Year;           //0 1   
        UINT8    Month;          //2
        UINT8    Day;            //3
        UINT8    Hour;           //4
        UINT8    Minute;         //5
        UINT8    Second;         //6
        UINT8    TypeEvent;      //7 
        UINT32   SerialNumber;   //8...11
        UINT16   IdNet;          //12 13
        UINT16   QuantityBos;    //14 15
        UINT8    Direct;         //16
        UINT8    Situation;      //17
        UINT8    ErrByte;        //18
        UINT8    StatusAutoByte;     //19
        UINT8    ControlByte;//20
        UINT8    reserv;           // 21
    }EventStruct;
        // изм. 25.03.22
    typedef union{
        struct {
            unsigned char Err_Udp    :1;
            unsigned char Err_Uvoa   :1;
            unsigned char Rip        :1;
            unsigned char classIP    :4;
            unsigned char Reset      :1;    // изм 12.09.22
        };
        unsigned char ByteFlagRoom_Pcb2;
    }fildFlagBitsPcb2;
    extern fildFlagBitsPcb2 FildFlagsPcb2[12];
    // внешние переменные
    extern UINT16  CurrentEventWrite;
    extern UINT8   GroupNumber;
    extern UINT16  DeviceNumber;
    // прототипы функций
    void SaveCurrentPosition(void);
    unsigned int ReadCurrentPositionAt45(void);
    void SaveAccessPassword(unsigned long accessPassword);
    void SaveSerialNumberBU(unsigned long tempSerial,UINT16 addr);
    void SaveEvent(UINT8 TypeEvent);
    UINT8 CheckStatusBU201106(UINT8 direct);
//    void MainMenu0521(void);
//    void DisplayStatusDirection(void);
//    void DisplayStatusBU(void);
//    void DisplayStatusBOS1202(void);
//    void DisplayControlDirection(void);
    void IndicatorDirection(UINT8 dir);
    void DisplayReadArhiv0525(void);
    void TransmittArhivUSB(void);
    void IndicationControl(void);
//    void CheckKey1(void);
    void CheckStausPWR(void);
    void SetAutoStatus201029(UINT8 ii);
    void TestKey(void);
// изм. 01.04.22    
    void SavingRegisteredKid(UINT8 num);
    void ReadRegisteredKid(UINT8 num);
    UINT8 SearchNumberKid(void);
// изм. 07.04.22
    void SaveModeRs485(unsigned char mode485);
    void TestLed(void);
    void KeyCheckMain(void);
    void KeyHandler(void);
    void PrintDirectionNumber(UINT8 screen);
    UINT8 Check32Device(UINT8 GroupNumber);
    UINT8 CheckDevice(UINT16 number);
    void PrintStatusBosNew(unsigned char page,unsigned int index);
    void DisplayStatusBOS1202(unsigned char page,UINT16 deviceNumber);
#ifdef	__cplusplus
}
#endif

#endif	/* FUNCTION_H */

