/**********************************************************************************
 * FILE        :MyGyro.h
 * Update      :2016/4/22
 * Content     :_20160523_integ_seq
 * Author      :小澤
 * Memo        :
 ***********************************************************************************/

#ifndef _MyGyro_h_
#define _MyGyro_h_


void Init_MyGyro();
void MyGyro(CANSAT* io_cansat);
uint8_t i2cWrite1(uint8_t registerAddress, uint8_t data, bool sendStop);
uint8_t i2cWrite2(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop);
uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes);

#endif
