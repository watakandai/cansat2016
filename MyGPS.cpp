/**********************************************************************************
 * FILE        :MyGPS.cpp
 * Update      :2016/5/27
 * Content     :_20160523_integ_seq
 * Author      :吉田 小澤
 * Memo        :GPGGAの部分のみ抜き出して格納する 0828杉原変更
 ***********************************************************************************/
 
 //includeの順番を変えるべからず　前方宣言に不明によりコンパイルエラーの原因になる
#include "Arduino.h"
#include "main.h"
#include <stdio.h>


void Init_MyGPS() {
  Serial1.begin(GPSBAUDRATE);
};

void MyGPS(CANSAT* io_cansat) {
  Serial1.end();
  Serial1.begin(GPSBAUDRATE);
  char buf[200];
  int count = 0;

  //GPGGA部分のみバッファに読み込み
  do {
    if (Serial1.available()) {
      buf[count] = Serial1.read();
      count++;
    }
    if (count >= 200 )break;     //バッファが溢れそうなら強制的に1行終了
  } while (buf[count - 1] != 0x0A); //\rを検出するまで読み込み
  buf[count] = '\0';

  //GPGGAのみ変数に格納
  if (0 == strncmp("$GPGGA", buf, 6)) { //先頭6文字を比較
    strtok(buf, ",");
    io_cansat->gps_data.Time = atof(strtok(NULL, ","));       // UTC時刻の抽出
    io_cansat->gps_data.LatDo = atof(strtok(NULL, "."));        // 緯度の度
    io_cansat->gps_data.LatHun = atof(strtok(NULL, ","));      // 緯度の分
    strtok(NULL, ",");       // N or S
    io_cansat->gps_data.LonDo = atof(strtok(NULL, "."));       // 経度の度
    io_cansat->gps_data.LonHun = atof(strtok(NULL, ","));       // 経度の分
    strtok(NULL, ",");       // E or W
    strtok(NULL, ",");
    io_cansat->gps_data.SatNum = (short)atof(strtok(NULL, ","));     // The number of Satellites
    io_cansat->gps_data.PosAccuracy = atof(strtok(NULL, ","));// The accuracy of its position
    io_cansat->gps_data.Alt = atof(strtok(NULL, ","));     // The height

//  //for test
//       io_cansat->gps_data.LonDo =13939;
//        io_cansat->gps_data.LonHun =2097;
    /////変換　3541.1493 → 35.411493 → 35.6858
//    io_cansat->gps_data.LatDoReal = (long)io_cansat->gps_data.LatDo / 100;
//    io_cansat->gps_data.LatHunReal = round(( (io_cansat->gps_data.LatDo / 100 - io_cansat->gps_data.LatDoReal) * 100 + io_cansat->gps_data.LatHun / 10000) / 60 * 1000000); //0828
//    io_cansat->gps_data.Lat = io_cansat->gps_data.LatDoReal + io_cansat->gps_data.LatHunReal / 1000000; //0828
//    io_cansat->gps_data.LonDoReal = (long)io_cansat->gps_data.LonDo / 100;
//    io_cansat->gps_data.LonHunReal = round(( (io_cansat->gps_data.LonDo / 100 - io_cansat->gps_data.LonDoReal) * 100 + io_cansat->gps_data.LonHun / 10000) / 60 * 1000000);
//    io_cansat->gps_data.Lon = io_cansat->gps_data.LonDoReal + io_cansat->gps_data.LonHunReal /1000000; //0828

    io_cansat->gps_data.LatDoReal = (long)io_cansat->gps_data.LatDo / 100;
    io_cansat->gps_data.LatHunReal = round(( (io_cansat->gps_data.LatDo / 100 - io_cansat->gps_data.LatDoReal) * 100 + io_cansat->gps_data.LatHun / 10000) / 60 * 1000000); //0828
    io_cansat->gps_data.Lat = io_cansat->gps_data.LatDoReal*1000000 + io_cansat->gps_data.LatHunReal;
    io_cansat->gps_data.LonDoReal = (long)io_cansat->gps_data.LonDo / 100;
    io_cansat->gps_data.LonHunReal = round(( (io_cansat->gps_data.LonDo / 100 - io_cansat->gps_data.LonDoReal) * 100 + io_cansat->gps_data.LonHun / 10000) / 60 * 1000000);
    io_cansat->gps_data.Lon = io_cansat->gps_data.LonDoReal*1000000 + io_cansat->gps_data.LonHunReal;

//  Serial.println(io_cansat->gps_data.Lon);
//    if(io_cansat->gps_data.Lat == 0){
//      io_cansat->sequence_data.GPGGA_flag = 1; // GPGGA 取得成功 0 -> 1
//    } else {
//      /////////////////////////////////////////////////////////////////////////////////////////////////////
////      io_cansat->sequence_data.GPGGA_flag = 0; // GPGGA 取得失敗 1 -> 0
//      io_cansat->sequence_data.GPGGA_flag = 1; // 試験確認用 GPGGA常に１      
//    }
  }


  
//  Serial.print("GPGGA_flag");
//  Serial.println(io_cansat->sequence_data.GPGGA_flag);
}

