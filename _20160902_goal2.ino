/**********************************************************************************
 * FILE        :_20160818_Noshiro.ino
 * Update      :2016/08/18
 * Content     :
 * Author      :小澤
 * Memo        :能代本番用プログラム GOAL変更
 ***********************************************************************************/

//#include <SoftwareSerial.h>
//#include <FlexiTimer2.h>
#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include "Arduino.h"
#include "main.h"

  /*------------ Openlog 保存日時 入力欄 -----------*/
  String Date = "Date : 2016/09/02 Test Running";
  /*------------------------------------------------*/

// -----------------------------------------------------------------------
// 0 PREPARING  // 準備状態（電源オン→GPS衛星捕捉までの間）
// 1 FLYING     // 地上期（運搬→ロケット収納→ロケット発射）
// 2 DROPPING   // 降下中
// 3 LANDING    // パラ分離
// 4 RUNNING    // 走行中
// 5 IDLING     // アイドリング
// 6 STUCK      // スタック中
// 7 GOAL       // ゴール
// -----------------------------------------------------------------------

CANSAT cansat;
void setup() {
  Serial.begin(9600);
  Serial.println(F("Begin!"));
  Init_MyCanSat (&cansat, Date);           // MySequence.cpp
//  FlexiTimer2::set(10,MyEncoder); //10msごと
//  FlexiTimer2::start();            //タイマー割り込み開始    
  Serial.println(F("Finished Initializing !!"));
}

void loop() {
  MyCanSat (&cansat); // MySequence.cpp
//  delay(1000);
}

