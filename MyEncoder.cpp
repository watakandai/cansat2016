/**********************************************************************************
 * FILE        :Myencoder.cpp
 * Update      :2016/5/29
 * Content     :_20160529_distributed
 * Author      :小澤
 * Memo        : 
 ***********************************************************************************/

#include "Arduino.h"
#include "main.h"
#include <math.h>


CANSAT g_cansat; // こうすればFlexiTimer2のしょうもない引数設定問題 回避できる

void Init_MyEncoder(){
  pinMode(encoder_pin_r, INPUT);
  pinMode(encoder_pin_l, INPUT);
}


/*MyCountに移動*/
void MyEncoder(){
    int encoder_r = analogRead(encoder_pin_r);
    int encoder_l = analogRead(encoder_pin_l);
    
    int encoder_flg_r;
    int encoder_flg_l;
  //右
  //アナログの値を01に分ける
    if(encoder_r > 100){
      encoder_flg_r = 1;
    }else{
      encoder_flg_r = 0;    
    }
  
  //1→0→1の回数を数える
    if(encoder_flg_r == 1){
      g_cansat.encoder_data.flg_pre_r = 1;
    }else{
      if(g_cansat.encoder_data.flg_pre_r == 1){
        g_cansat.encoder_data.count_r ++;
        g_cansat.encoder_data.flg_pre_r = 0;
      }
    }
  
  //左
  //アナログの値を01に分ける
     if(encoder_l > 100){
      encoder_flg_l = 1;
    }else{
      encoder_flg_l = 0;    
    }
  
  //1→0→1の回数を数える
    if(encoder_flg_l == 1){
      g_cansat.encoder_data.flg_pre_l = 1;
    }else{
      if(g_cansat.encoder_data.flg_pre_l == 1){
        g_cansat.encoder_data.count_l ++;
        g_cansat.encoder_data.flg_pre_l = 0;
      }
    }
  
  
    g_cansat.encoder_data.count++;
    
    if(fmod(g_cansat.encoder_data.count, 2.5) == 0){    //0.01秒ごとに報告
//       double a = g_cansat.encoder_data.count_r/20/0.01;
//       double b = g_cansat.encoder_data.count_l/20/0.01;
//      analogWrite(a, encoder_pin_r);                        //回転数の報告(単位は [回転/s])
//      analogWrite(b, encoder_pin_l);                            
      g_cansat.encoder_data.roll_right = g_cansat.encoder_data.count_r/20/0.01;
      g_cansat.encoder_data.roll_left = g_cansat.encoder_data.count_l/20/0.01;
      analogWrite(g_cansat.encoder_data.roll_right, encoder_pin_r);                        //回転数の報告(単位は [回転/s])
      analogWrite(g_cansat.encoder_data.roll_left, encoder_pin_l);
      g_cansat.encoder_data.count_r = 0;                   //回転数のリセット
      g_cansat.encoder_data.count_l = 0;
    }
    int phtr00 = g_cansat.encoder_data.count_r;
//    Serial.println(phtr00);
}
    
