/***********************************************************************
 * FILE        :MyLight.cpp
 * Update      :2015/05/24
 * Content     :_20160523_integ_seq
 * Author      :森 小澤
 * Memo        :
 ***********************************************************************/
 
 //includeの順番を変えるべからず　前方宣言に不明によりコンパイルエラーの原因になる
#include "Arduino.h"
#include "main.h"

 
void Init_MyLight(){
  pinMode(LIGHT_pin,INPUT);
}

void MyLight(CANSAT* io_cansat){
  io_cansat->light_data.light_value = analogRead(LIGHT_pin);//光センサの値の読み込み
}
