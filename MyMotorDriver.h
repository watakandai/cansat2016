/**********************************************************************************
 * FILE        :MyMotorDriver.h
 * Update      :2016/4/22
 * Content     :_20160523_integ_seq
 * Author      :黒須
 * Memo        :●5Vの外部電圧をかけたとき，
                  Dig_PWM 150を150にすると2.7Vくらいがモータに流れる
 ***********************************************************************************/

#ifndef MYMOTORDRIVER_H_
#define MYMOTORDRIVER_H_


void Init_MyMotorDriver();
void MyMotor_forward();
void MyMotor_back();
void MyMotor_stop();

#endif

