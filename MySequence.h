/**********************************************************************************
 * FILE        :MySequence.h
 * Update      :2016/5/23
 * Content     : _20160523_integ_seq         シークエンスの箱　ヘッダ
 * Author      :小澤
 * Memo        :
 ***********************************************************************************/
#ifndef MySequence_h_
#define MySequence_h_


void Init_MySequence(CANSAT* io_cansat);
void SerialStateSwitcher(CANSAT* io_cansat);
void MyLEDsBlinking(CANSAT* io_cansat,unsigned long interval,char LED_num);
void CarrierJudge (CANSAT* io_cansat);
void LightJudge(CANSAT* io_cansat);
void LandingJudge (CANSAT* io_cansat);
void StuckJudge (CANSAT* io_cansat);
void GoalJudge (CANSAT* io_cansat);
void MyPREPARING (CANSAT* io_cansat);
void MyFLYING (CANSAT* io_cansat);
void MyDROPPING (CANSAT* io_cansat);
void MyLANDING (CANSAT* io_cansat);
void MySEPARATING (CANSAT* io_cansat);
void MyRUNNING (CANSAT* io_cansat);
void MyIDLING (CANSAT* io_cansat);
void MySTUCK (CANSAT* io_cansat);
void MyGOAL (CANSAT* io_cansat);

#endif
