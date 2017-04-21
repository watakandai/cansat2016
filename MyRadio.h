/**********************************************************************************
 * FILE        :MyRadio.h
 * Update      :2016/4/22
 * Content     :_20160523_integ_seq
 * Author      :杉原
 * Memo        :PackData:データを詰め込む、MU2_send:コマンドを作りMU-2に送信する
 ***********************************************************************************/

#ifndef MYRADIO_H_
#define MYRADIO_H_


//MU2はserial2
void Init_MyRadio();
//void PackData(CANSAT* io_cansat);
void MU2_Send(CANSAT* io_cansat);
void CommandSendTest(CANSAT* io_cansat);

#endif
