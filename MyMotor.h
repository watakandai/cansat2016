/**********************************************************************************
 * FILE        :MyMotor.h
 * Update      :2016/6/6
 * Content     :
 * Author      :Kobayashi
 * Memo        :06/6時点でpidがうまく動かない。。。
 ***********************************************************************************/

 #ifndef MYMOTOR_H_
 #define MYMOTOR_H_

void MotorControl(CANSAT* io_cansat);
void Run2Goal(CANSAT* io_cansat);
 
 #endif  
