/* 
 * File:   funcia_spi.h
 * Author: 11
 *
 * Created on 31 марта 2022 г., 14:10
 */

#ifndef FUNCIA_SPI_H
#define	FUNCIA_SPI_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define     SPI1EN       SPI1STATbits.SPIEN
#define     AT_SPI_MODE 2
#define     RC_SPI_MODE 3
#define     CC_SPI_MODE 3    
void InitSpi24(void);
void ChangModeSpi(unsigned char mode);
unsigned char SendByteToSpi (unsigned char Value);



#ifdef	__cplusplus
}
#endif

#endif	/* FUNCIA_SPI_H */

