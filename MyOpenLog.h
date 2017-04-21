/**********************************************************************************
 * FILE        :MyOpenLog.h
 * Update      :
 * Content     :_20160523_integ_seq
 * Author      :
 * Memo        :
 ***********************************************************************************/

#ifndef _MYOPENLOG_H_
#define _MyOPENLOG_H_


//構造体の中の型はプログラム統合のときに詳しく定義しよう
//いまは仮でfloat(2016/4/16)

//関数定義
void Init_MyOpenLog(CANSAT* io_cansat, String Date);
void MyOpenLog(CANSAT* io_cansat);

#endif
