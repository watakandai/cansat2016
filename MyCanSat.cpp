/**********************************************************************************
 * FILE        :MyCanSat.cpp
 * Update      :2016/06/11
 * Content     :統合関数
 * Author      :小澤
 * Memo        :エンコーダーが回転回数を取得するために，FlexiTimer2による割り込みを導入
 ***********************************************************************************/

#include <FlexiTimer2.h>
#include "Arduino.h"
#include "main.h"

// ------------------------------------------------------------------------------
// ino 初期化関数
// ------------------------------------------------------------------------------
void Init_MyCanSat(CANSAT*io_cansat, String Date){
//  Serial.begin(MU2_BAUDRATE);
//  Serial2.begin(MU2_BAUDRATE);  //initializing serial communication with MU-2
  Init_MySequence(io_cansat); // シークエンス
  Init_MyGyro();              // ジャイロ
  Init_MyCompass();           // デジコン
  Init_MyGPS();               // GPS
//  Init_MyMotorDriver();       // モータドライバ
  Init_MyRadio();             // 無線
  Init_MyLight();             // 光センサ
  Init_MyOpenLog(io_cansat, Date);           // OpenLog
//  Init_MyEncoder();           // Encoder
  Init_MyGuidance();          // 誘導
  /*エンコーダーのために割り込み発生 ?*/
  
}


// ------------------------------------------------------------------------------
// ino ループ関数: state 情報からどの関数を呼び出すかを決める関数 
// ----- GPSデータ (GPGGA) が取得「成功」or「失敗」が CanSat の挙動を支配 ------
// ------------------------------------------------------------------------------

/* GPSデータ (GPGGA) が取得「成功」or「失敗」が CanSat の挙動を支配 */
void MyCanSat(CANSAT* io_cansat){
  MyGPS (io_cansat);
  MyCompass (io_cansat);
//  Serial.print("My state is : ");
//  Serial.println(io_cansat->state_data);
//  Serial.print("Carrier judge loop count is  ");
//  Serial.println(io_cansat->sequence_data.Count_0to1);
//  Serial.print("Releasing judge loop count is  ");
//  Serial.println(io_cansat->sequence_data.Count_1to2);
    
    ///////////////試験用//////////////
//    io_cansat -> state_data = RUNNING;
    ///////////////////////////////////
    if (io_cansat->state_data != RUNNING || io_cansat->state_data != LANDING || io_cansat->state_data != STUCK || io_cansat->state_data != GOAL || io_cansat->state_data != IDLING){
      MyLight (io_cansat);
    }
    MyGyro (io_cansat);
    MyOpenLog(io_cansat);
    if (io_cansat->state_data == PREPARING || io_cansat->state_data == DROPPING || io_cansat->state_data == LANDING || io_cansat->state_data == GOAL){
      MU2_Send (io_cansat);
    }
//    MU2_Send (io_cansat);
    
    SerialStateSwitcher(io_cansat); // 手動で state 変更
    
    switch (io_cansat->state_data) {
      case PREPARING:
        MyPREPARING(io_cansat);
        break;
      case FLYING:
        MyFLYING(io_cansat);
        break;
      case DROPPING:
        MyDROPPING(io_cansat);
        break;
      case LANDING:
        MyLANDING(io_cansat);
        break;
      case RUNNING:
        MyRUNNING(io_cansat);
        break;
      case IDLING:
        MyIDLING(io_cansat);
        break;
      case STUCK:
        MySTUCK(io_cansat);
        break;
      case GOAL:
        MyGOAL(io_cansat);
        break;
      default:
        break;
    }
    io_cansat->openlog_data.all_log = ""; //初期化
    io_cansat->send_data.SendCommandString = ""; //初期化
//    delay(30);
}

