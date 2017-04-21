/**********************************************************************************
 * FILE        :MyDigitalCompass.cpp
 * Update      :2016/4/22
 * Content     :_20160523_integ_seq
 * Author      :黒須
 * Memo        : 
 ***********************************************************************************/
 
 //includeの順番を変えるべからず　前方宣言に不明によりコンパイルエラーの原因になる
#include "Arduino.h"
#include "main.h"
#include <Wire.h>


void Init_MyCompass() {
   Wire.begin();
  //角度表示のためのシリアル通信開始
  //Serial.begin(9600);
  
  //Continuous Modeに設定する
  Wire.beginTransmission(compassAddress);
  //RAM書き込み用コマンド
  Wire.write('G');
  //書き込み先指定
  Wire.write(0x74);
  //モード設定
  Wire.write(0x50);
  //通信終了
  Wire.endTransmission();
  //処理時間
//  delayMicroseconds(70);
 // Serial.println("completed");
  
  //Wire.write('W');  //wakeup
  Wire.begin(compassAddress);  //I2C通信を開始
  //キャリブレーション開始コマンド送信
  Wire.beginTransmission(compassAddress);
  Wire.write("C");  //これがキャリブレーション開始コマンド
  Wire.endTransmission();
  Serial.print(F("Digital compass calibration ...   "));
  delay(3000);  //モージュールをゆらゆら(3000 msec)
  Serial.println(F("Calibration completed !"));
  //キャリブレーション終了コマンド送信
  Wire.beginTransmission(compassAddress);
  Wire.write("E");  //これがキャリブレーション終了コマンド
  Wire.endTransmission();
}

void MyCompass(CANSAT* io_cansat){
  Wire.end();
  Wire.begin(compassAddress);// エンドビギン戦法
  
  Wire.beginTransmission(compassAddress);  //デジタルコンパスに対して送信を開始
  Wire.requestFrom(compassAddress, 2);  //これにより，デジコンから値を2byte分取得します宣言
  Wire.write("A");
  Wire.endTransmission();
  /*0x41により，デジコンの磁気計(方位センサ)に方位角の計算を行わせる
    この時点で，2byte分のデータが算出され，readまたはcommand待ちとなる*/
  int value;
  float ppvalue[2] = {};

  value = Wire.read();  //そしてここでまず上位8bit分=1byteのデータをありがたく頂戴する
  value = value << 8;  //そして8bit分レフトシフトする
  value += Wire.read();  //その後，下位8bit=1byteのデータをありがたく頂戴する
  io_cansat->compass_data.fvalue = (float) value*0.1;
  
//  io_cansat->compass_data.pvalue[2] = io_cansat->compass_data.pvalue[1];
//  io_cansat->compass_data.pvalue[1] = io_cansat->compass_data.pvalue[0];
//  io_cansat->compass_data.pvalue[0] = (float) value*0.1;
//  
//
////  // 同時に3回分取得 --------------------------------------------------------
////  if (Wire.available()){ 
////    for (int count=0; count<3 ; count++){
////    value = Wire.read();  //そしてここでまず上位8bit分=1byteのデータをありがたく頂戴する
////    value = value << 8;  //そして8bit分レフトシフトする
////    value += Wire.read();  //その後，下位8bit=1byteのデータをありがたく頂戴する
////    io_cansat->compass_data.pvalue[count] = (float) value*0.1; //10で除してfloat型にキャストすると，見事所望の方位角が得られるのだ！
////    Wire.beginTransmission(compassAddress);  //デジタルコンパスに対して送信を開始
////    Wire.requestFrom(compassAddress, 2);  //これにより，デジコンから値を2byte分取得します宣言
////    Wire.write("A");
////    Wire.endTransmission();
//////    delay(30);
////    }
////  }
////  
//////  Serial.print("pvalue[0]: "); Serial.println(io_cansat->compass_data.pvalue[0]);
//////  Serial.print("pvalue[1]: "); Serial.println(io_cansat->compass_data.pvalue[1]);
//////  Serial.print("pvalue[2]: "); Serial.println(io_cansat->compass_data.pvalue[2]);
////  
////  /* -------------------取得角度の象限で移動平均フィルタのアルゴリズム変更------------------ */
//  ppvalue[0] = io_cansat->compass_data.pvalue[0]*0.011111;
//  ppvalue[1] = io_cansat->compass_data.pvalue[1]*0.011111;
//  ppvalue[2] = io_cansat->compass_data.pvalue[2]*0.011111;
////  Serial.print(F("ppvalue[0]: ")); Serial.println(ppvalue[0]);
////  Serial.print(F("ppvalue[1]: ")); Serial.println(ppvalue[1]);
////  Serial.print(F("ppvalue[2]: ")); Serial.println(ppvalue[2]);
//  
//  if((1 > ppvalue[0] || ppvalue[0] > 3) && (1 > ppvalue[1] || ppvalue[1] > 3) && (1 > ppvalue[2] || ppvalue[2] > 3)){
////    if(1 > ppvalue[1] || ppvalue[1] > 3){
////      if(1 > ppvalue[2] || ppvalue[2] > 3){ // この時点で "1未満" または "3より大きい値"のみ
//        if(ppvalue[0]+ppvalue[1]+ppvalue[2] > 3 && ppvalue[0]+ppvalue[1]+ppvalue[2] < 9){ // この時点で 0-90 deg と 270-360の値がどちらも含まれ，かつその象限の値のみ
//          if (ppvalue[0] < 1){
//            io_cansat->compass_data.pvalue[0] = io_cansat->compass_data.pvalue[0] + 360; // 0-90 deg なら正規化のため1周分回す
//          }
//          if (ppvalue[1] < 1){
//            io_cansat->compass_data.pvalue[1] = io_cansat->compass_data.pvalue[1] + 360; // 0-90 deg なら正規化のため1周分回す
//          }
//          if (ppvalue[2] < 1){
//            io_cansat->compass_data.pvalue[2] = io_cansat->compass_data.pvalue[2] + 360; // 0-90 deg なら正規化のため1周分回す
//          }
////          Serial.print("ppvalue[0]: "); Serial.println(ppvalue[0]);
////          Serial.print("ppvalue[1]: "); Serial.println(ppvalue[1]);
////          Serial.print("ppvalue[2]: "); Serial.println(ppvalue[2]);
//          io_cansat->compass_data.fvalue = (Dw0*io_cansat->compass_data.pvalue[0]
//                             + Dw1*io_cansat->compass_data.pvalue[1] + Dw2*io_cansat->compass_data.pvalue[2]) *0.33333;  //3次の移動平均フィルタ
//          if(io_cansat->compass_data.fvalue > 360){
//            io_cansat->compass_data.fvalue = io_cansat->compass_data.fvalue - 360; // 正規化
//          }
//        }else{
//          io_cansat->compass_data.fvalue = (Dw0*io_cansat->compass_data.pvalue[0]
//                             + Dw1*io_cansat->compass_data.pvalue[1] + Dw2*io_cansat->compass_data.pvalue[2])  *0.33333;  //3次の移動平均フィルタ
//        }
////      }else{
////        io_cansat->compass_data.fvalue = (Dw0*io_cansat->compass_data.pvalue[0]
////                             + Dw1*io_cansat->compass_data.pvalue[1] + Dw2*io_cansat->compass_data.pvalue[2])  *0.33333;  //3次の移動平均フィルタ
////      }
////    }else{
////      io_cansat->compass_data.fvalue = (Dw0*io_cansat->compass_data.pvalue[0]
////                             + Dw1*io_cansat->compass_data.pvalue[1] + Dw2*io_cansat->compass_data.pvalue[2])  *0.33333;  //3次の移動平均フィルタ
////    }
//  }else{
//    io_cansat->compass_data.fvalue = (Dw0*io_cansat->compass_data.pvalue[0]
//                             + Dw1*io_cansat->compass_data.pvalue[1] + Dw2*io_cansat->compass_data.pvalue[2]) / 3;  //3次の移動平均フィルタ
//  
//////  Serial.print("fvalue: "); Serial.println(io_cansat->compass_data.fvalue);
//  /* ---------------------------------------------------------------------------------- */
//  }
}

