/**********************************************************************************
   FILE        :MySequence.cpp
   Update      :2016/5/26
   Content     :_20160806_Landing
   Author      :小澤
   Memo        :●着地検知の方法を変更（風で引きずられても着地判定ができるように）
 ***********************************************************************************/

// -----------------------------------------------------------------------
// 0 PREPARING  //準備状態（電源オン→GPS衛星捕捉までの間）
// 1 FLYING     // 地上期（運搬→ロケット収納→ロケット発射）
// 2 DROPPING   // 降下中
// 3 LANDING    // パラ分離
// 4 RUNNING    // 走行中
// 5 IDLING     // アイドリング
// 6 STUCK      // スタック中
// 7 GOAL       // ゴール
// ----------------------------------------------------------------------

// include の順番これじゃないと前方不宣言くらう
#include "Arduino.h"
#include "main.h"
#include <math.h>

// ------------------------------------------------------------------------------
// 初期化関数
// ------------------------------------------------------------------------------
void Init_MySequence(CANSAT* io_cansat) {
  //  PREPARING :準備状態（電源オン→GPS衛星補足までの間）
  io_cansat -> state_data = PREPARING; // stateを PREPARING に

  // ピンの設定---------------------------------------------
  int LED_pin[3]; // 初期化のため宣言だけ
  LED_pin[0] = LED_1_pin;
  LED_pin[1] = LED_2_pin;
  LED_pin[2] = LED_3_pin;

  // state ピンをOUTPUTに設定 ------------------------------
  for (int i = 0; i < 3; i++) {
    pinMode(LED_pin[i], OUTPUT);
  }

  //パラ分離用のピンをOUTPUTに設定 ------------------------
  pinMode(release_pin, OUTPUT);

  // LEDごとに各変数初期化 --------------------------------
  /* previousMillis */
  for (int i = 0; i < 3; i++) {
    io_cansat -> sequence_data.PreviousBlinkingTime[i] = 0;
  }
  /* ledState */
  for (int i = 0; i < 3; i++) {
    io_cansat -> sequence_data.ledState[i] = LOW; // シークエンス判断用のLEDをLOWに
  }
}


// ------------------------------------------------------------------------------
// state をシリアル入力から手動で変更する関数
// ------------------------------------------------------------------------------
void SerialStateSwitcher(CANSAT* io_cansat) {
  if (Serial.available() > 0) {
    byte inByte = Serial.read(); // 入力数字を読み込む
    Serial.print(F("\n"));
    Serial.print(F("I received: "));   // 受信データを送りかえす

    switch (inByte) {
      case '0':
        io_cansat -> state_data = PREPARING;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '1':
        io_cansat -> state_data = FLYING;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '2':
        io_cansat -> state_data = DROPPING;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '3':
        io_cansat -> state_data = LANDING;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '4':
        io_cansat -> state_data = RUNNING;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '5':
        io_cansat -> state_data = IDLING;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '6':
        io_cansat -> state_data = STUCK;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      case '7':
        io_cansat -> state_data = GOAL;
        Serial.print(F("Switch to "));
        Serial.println(io_cansat -> state_data);
        break;
      default:
        Serial.print(F("Unknown Command. Please Enter the State Number 0 to 7."));
        Serial.println("\n");
        break;
    }
    delay(1000); // msec
  }
}



// ------------------------------------------------------------------------------
// delay関数を使用せずにLEDを点滅させる関数
// ------------------------------------------------------------------------------
/* 点滅間隔(interval) と 対象のLED番号(LED_num) から デジタル信号(ledState) を変化 */
void MyLEDsBlinking(CANSAT* io_cansat, unsigned long interval, char LED_num) {
  char LED_pin;
  if (LED_num == LED_num1) {
    LED_pin = LED_1_pin;
  }
  if (LED_num == LED_num2) {
    LED_pin = LED_2_pin;
  }
  if (LED_num == LED_num3) {
    LED_pin = LED_3_pin;
  }
  unsigned long CurrentBlinkingTime = millis();
  if (CurrentBlinkingTime - io_cansat->sequence_data.PreviousBlinkingTime[LED_num - 1] > interval) {
    io_cansat -> sequence_data.PreviousBlinkingTime[LED_num - 1] = CurrentBlinkingTime;
    if (io_cansat -> sequence_data.ledState[LED_num - 1] == LOW) {
      io_cansat -> sequence_data.ledState[LED_num - 1] = HIGH;
    } else {
      io_cansat -> sequence_data.ledState[LED_num - 1] = LOW;
    }
  }
  digitalWrite(LED_pin, io_cansat->sequence_data.ledState[LED_num - 1]);
}

// ------------------------------------------------------------------------------
// キャリア格納検知関数
// ------------------------------------------------------------------------------
void CarrierJudge (CANSAT* io_cansat) {
  if (io_cansat->state_data == PREPARING && io_cansat->light_data.light_value < PRE2FLY_thre) {
    //    io_cansat->state_data = FLYING;      // PREPARING -> FLYING
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    io_cansat->sequence_data.Count_0to1 = io_cansat->sequence_data.Count_0to1 + 1;
    if (io_cansat->sequence_data.Count_0to1 > Count_0to1_thre) {
      io_cansat -> state_data = FLYING;  // PREPARING -> FLYING
    }
  } else if (io_cansat->state_data == PREPARING && io_cansat->light_data.light_value > PRE2FLY_thre) {
    io_cansat->sequence_data.Count_0to1 = 0;
  }
}

// ------------------------------------------------------------------------------
// 放出検知関数
// ------------------------------------------------------------------------------
void LightJudge(CANSAT* io_cansat) {
  if (io_cansat->state_data == FLYING && io_cansat->light_data.light_value > FLY2DROP_thre) {
    //    io_cansat -> state_data = DROPPING;  // FLYING -> DROPPING
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    io_cansat->sequence_data.Count_1to2 = io_cansat->sequence_data.Count_1to2 + 1;
    if (io_cansat->sequence_data.Count_1to2 > Count_1to2_thre) {
      io_cansat -> state_data = DROPPING;  // FLYING -> DROPPING
    }
  } else if (io_cansat->state_data == FLYING && io_cansat->light_data.light_value < FLY2DROP_thre) {
    io_cansat->sequence_data.Count_1to2 = 0;
  }
}


// ------------------------------------------------------------------------------
// 着地検知関数
// ------------------------------------------------------------------------------

/*やりたいこと---------------------------------------------------
  ① 光センサによる放出検知のフラグが立っている
  ② 三軸の加速度が一時刻前の値と比較して閾値以下に収まる

  ③ 条件①と②を満たしつつ一定時間経過
  （z_accelのif文内に何回連続で入ったかで数える．
   従って，y_accelのif文において連続通過フラグが
   立っていない場合には回数をリセットする仕様にする）
  → ③が成立で，着地検知のフラグを立てて走行フェーズに移行

  // 20160806　変更点
  上記の方法だと風でパラシュートと一緒に引きずられたときに着地検知ができない問題がある．
  そのため，以下のような判定方法に変更．
  ①

  -----------------------------------------------------------------*/

void LandingJudge (CANSAT* io_cansat) {
  /* 着地判定条件 */
  unsigned long NOW = millis();
  // 角速度
  if (io_cansat -> state_data == DROPPING) {
    if (fabs(io_cansat->gyro_data.x_gyro - io_cansat->gyro_data.pre_x_gyro) < x_gyro_thre) {
      if (fabs(io_cansat->gyro_data.y_gyro - io_cansat->gyro_data.pre_y_gyro) < y_gyro_thre) {
        if (fabs(io_cansat->gyro_data.z_gyro - io_cansat->gyro_data.pre_z_gyro) < z_gyro_thre) {
          io_cansat->sequence_data.Series_num1 = io_cansat->sequence_data.Series_num1 + 1;  // 計測数を 1 増やす
        } else {
          io_cansat->sequence_data.Series_num1 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
        }
      } else {
        io_cansat->sequence_data.Series_num1 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
      }
    } else {
      io_cansat->sequence_data.Series_num1 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
    }

    Serial.println(io_cansat->sequence_data.Series_num1);

    // 角度
    if (fabs(io_cansat->gyro_data.x_euler_kal - io_cansat->gyro_data.pre_x_angle) < x_angle_thre) {
      if (fabs(io_cansat->gyro_data.y_euler_kal - io_cansat->gyro_data.pre_y_angle) < y_angle_thre) {
        if (fabs(io_cansat->gyro_data.z_euler - io_cansat->gyro_data.pre_z_angle) < z_angle_thre) {
          io_cansat->sequence_data.Series_num2 = io_cansat->sequence_data.Series_num2 + 1;  // 計測数を 1 増やす
        } else {
          io_cansat->sequence_data.Series_num2 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
        }
      } else {
        io_cansat->sequence_data.Series_num2 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
      }
    } else {
      io_cansat->sequence_data.Series_num2 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
    }
    Serial.println(io_cansat->sequence_data.Series_num2);

    // 加速度
    if (fabs(io_cansat->gyro_data.x_accel - io_cansat->gyro_data.pre_x_accel) < x_accel_thre) {
      if (fabs(io_cansat->gyro_data.y_accel - io_cansat->gyro_data.pre_y_accel) < y_accel_thre) {
        if (fabs(io_cansat->gyro_data.z_accel - io_cansat->gyro_data.pre_z_accel) < z_accel_thre) {
          io_cansat->sequence_data.Series_num3 = io_cansat->sequence_data.Series_num3 + 1;  // 計測数を 1 増やす
        } else {
          io_cansat->sequence_data.Series_num3 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
        }
      } else {
        io_cansat->sequence_data.Series_num3 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
      }
    } else {
      io_cansat->sequence_data.Series_num3 = 0;  // 連続でz軸の判定に通らなかった場合は計測数を 0 に
    }
    Serial.println(io_cansat->sequence_data.Series_num3);

    // 冗長系で一定時間経過した場合に着地検知と判断してステート移行
    if (io_cansat->sequence_data.DropStartTime != 0) {
      if (NOW - io_cansat->sequence_data.DropStartTime > LandingTime_thre) {
        io_cansat->sequence_data.NEXT_flag = 1;
      } else {
        io_cansat->sequence_data.NEXT_flag = 0;
      }
    }
    Serial.println(io_cansat->sequence_data.NEXT_flag);


    /* 着地検知 */
    if (io_cansat->state_data == DROPPING) {
      if ((io_cansat->sequence_data.Series_num1 > Series_num_thre && io_cansat->sequence_data.Series_num2 > Series_num_thre && io_cansat->sequence_data.Series_num3 > Series_num_thre) || io_cansat->sequence_data.NEXT_flag == 1) {
        io_cansat->state_data = LANDING;  // DROPPING -> LANDING
        if (io_cansat->sequence_data.ReleaseStartTime == 0) {
          io_cansat->sequence_data.ReleaseStartTime = millis(); // パラ分離スタート時間
        }
        io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
        io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
        io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
      }
    }
  }
}

// ------------------------------------------------------------------------------
// パラシュート分離関数
// ------------------------------------------------------------------------------
void Releasing (CANSAT* io_cansat) {
  if (io_cansat->state_data == LANDING) {
    digitalWrite(release_pin, HIGH);
    unsigned long CurrentReleasingTime = millis();
    if ( CurrentReleasingTime - io_cansat->sequence_data.ReleaseStartTime > ReleaseTime_thre) {
      digitalWrite(release_pin, LOW); // HIGH -> LOW
      io_cansat->state_data = RUNNING;  // LANDING -> RUNNING
      io_cansat->sequence_data.RunningStartTime = millis(); // 走行スタート時間
    }
  }
}


// ------------------------------------------------------------------------------
// スタック検知関数
// ------------------------------------------------------------------------------

/*やりたいこと---------------------------------------------------
  // スタック検知 ---------------------
  ①GPSの値は変化なし
  ②ジャイロ加速度センサは一時刻前とは異なる値を取得し続けている
  ③エンコーダの値は増え続けている
  ①かつ②または③

  // スタック脱出 --------------------
  ①Stuck_flag = 1ならスタック防止機構は常に上下
  ②GPSの値に変化ありならStuck_flag = 0にしてスタック防止機構停止
  ③stateをRUNNINGに遷移
  -----------------------------------------------------------------*/

void StuckJudge (CANSAT* io_cansat) {
  /* スタック検知用に5s前のGPS座標を保存し，現時刻と比較 */
  unsigned long CurrentGpsPollingTime = millis();
  if (io_cansat->sequence_data.PreGpsPollingTime == 0) {
    io_cansat->sequence_data.PreGpsPollingTime = millis();
  } else if (io_cansat->sequence_data.PreGpsPollingTime != 0 && CurrentGpsPollingTime - io_cansat->sequence_data.PreGpsPollingTime > 20000) { // 10s
    io_cansat->sequence_data.Pre20sGpsLat = io_cansat->gps_data.Lat;
    io_cansat->sequence_data.Pre20sGpsLon = io_cansat->gps_data.Lon;
    io_cansat->sequence_data.PreGpsPollingTime = 0;
  }

  /* RUNNING から STUCK に陥る時の判断 */
  if (abs(io_cansat->sequence_data.Pre20sGpsLat - io_cansat->gps_data.Lat) == 0 && abs(io_cansat->sequence_data.Pre20sGpsLon - io_cansat->gps_data.Lon) == 0) {
    io_cansat->state_data = STUCK;
  }

  /* STUCK から RUNNING への判断 */
  if (abs(io_cansat->sequence_data.Pre20sGpsLat - io_cansat->gps_data.Lat) != 0 || abs(io_cansat->sequence_data.Pre20sGpsLon - io_cansat->gps_data.Lon) != 0) {
    io_cansat->state_data = RUNNING;
  }
}


// ------------------------------------------------------------------------------
// ゴール検知関数
// ------------------------------------------------------------------------------

/*やりたいこと---------------------------------------------------
  ①ゴール座標 [メートル]近傍 (10メートル以内) である
  ②「走行用モータ停止」と指示している
  以上が １秒間 成立
  -> ゴール検知フラグ Goal_flag = 1 としてゴールに state 遷移
  -----------------------------------------------------------------*/
void GoalJudge (CANSAT* io_cansat) {
  long dLat = dest_Lat - io_cansat->guidance_data.rober_Lat;
  long dLon = dest_Lon - io_cansat->guidance_data.rober_Lon;
  //  Serial.print(dLat, 7);
  //  Serial.print(dLon, 7);
  if (abs(dLat) <= Goal_area && abs(dLon) <= Goal_area) { // float の絶対値は fabs
    analogWrite(PinMotorLeft, 0); // モータ停止
    analogWrite(PinMotorRight, 0);  // モータ停止
    io_cansat->state_data = GOAL; // RUNNING -> GOAL
    io_cansat->sequence_data.Goal_flag = 1;

    /* 20160727時点では以下は省略 */

    //    if(io_cansat->sequence_data.PreviousGoalJudgeTime == 0){
    //      io_cansat->sequence_data.PreviousGoalJudgeTime = millis();
    //    }
    //    unsigned long CurrentGoalJudgeTime;
    //    int SubstitutedJustNow;
    //    int Ikkaime;
    //    if(CurrentGoalJudgeTime - io_cansat->sequence_data.PreviousGoalJudgeTime > Goal_interval){
    //      if (io_cansat->sequence_data.PreviousGpsPollingTime == 0){
    //        SubstitutedJustNow = 1; // 一度目の代入は無視
    //        Ikkaime = 1;
    //      }else if(io_cansat->sequence_data.PreviousGpsPollingTime != 0){
    //        SubstitutedJustNow = 1; // 今代入したよ 0 -> 1
    //        Ikkaime = 0;
    //      }
    //      io_cansat->sequence_data.PreviousGoalJudgeTime = CurrentGoalJudgeTime;
    //      io_cansat->sequence_data.Pre1sGpsLat = io_cansat->guidance_data.rober_Lat;
    //      io_cansat->sequence_data.Pre1sGpsLon = io_cansat->guidance_data.rober_Lon;
    //    }else{
    //      SubstitutedJustNow = 0; // 今代入してないよ 1 -> 0
    //      if(Ikkaime == 1){
    //        SubstitutedJustNow = 1; // 一度目は無視
    //      }
    //    }
    //    if (SubstitutedJustNow == 0 && abs(io_cansat->sequence_data.Pre1sGpsLat - io_cansat->guidance_data.rober_Lat) <= Goal_area && abs(io_cansat->sequence_data.Pre1sGpsLon - io_cansat->guidance_data.rober_Lon) <= Goal_area){
    //      io_cansat->state_data = GOAL; // RUNNING -> GOAL
    //      io_cansat->sequence_data.Goal_flag = 1;
    //      io_cansat->sequence_data.GoalTime = millis(); // ゴール時間を保存
    //    }
    //  } else {
    //    io_cansat->state_data = RUNNING;
    //    io_cansat->sequence_data.PreviousGoalJudgeTime = 0; // リセット
    //  }
  }
}


// ------------------------------------------------------------------------------
// Sequence: 準備中（電源on→GPS衛星捕捉
// State   : 電源on，センサ・モータ動作確認，SD記録開始＆無線確認→無線OFF
// state 0 : PREPARING
// ------------------------------------------------------------------------------
void MyPREPARING (CANSAT* io_cansat) {
  analogWrite(PinMotorLeft, 0);
  analogWrite(PinMotorRight, 0);
  io_cansat->sequence_data.WelcomeOnBoardTime = millis(); // CanSat起動時間として記録
  io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
  io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
  io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
  digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
  digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
  digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  /* GPS 座標が指定数以上で起動と判断し起動時間を記録 */
  // 数は http://library.jsce.or.jp/jsce/open/00037/1997/562-0141.pdf より 6 に設定 (20160526現在)
  if (io_cansat -> state_data == PREPARING && io_cansat->gps_data.SatNum >= 6) {
    io_cansat->sequence_data.WelcomeOnBoardTime = millis(); // CanSat起動時間として記録
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }

  /*--------------------------- State Shift---------------------------*/
  CarrierJudge (io_cansat); // PREPARING -> FLYING
  /*------------------------- State Shift END-------------------------*/
}

// ------------------------------------------------------------------------------
// Sequence: ロケット格納→格納→放出
// State   : 各センサ待機
// state 1 : FLYING
// ------------------------------------------------------------------------------
void MyFLYING (CANSAT* io_cansat) {
  /*--------------------------- State Shift---------------------------*/
  LightJudge(io_cansat);// FLYING -> DROPPING
  /*------------------------- State Shift END-------------------------*/

  /* LED */
  if (io_cansat->state_data == FLYING) {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  } else {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }
}

// ------------------------------------------------------------------------------
// Sequence: 降下中
// State   : 光センサで放出検知→無線動作開始→ジャイロ加速度センサ
// state 2 : DROPPING
// ------------------------------------------------------------------------------
void MyDROPPING (CANSAT* io_cansat) {
  if (io_cansat->sequence_data.DropStartTime == 0) {
    io_cansat->sequence_data.DropStartTime = millis();
  }
  /*--------------------------- State Shift---------------------------*/
  LandingJudge(io_cansat); // DROPPING -> LANDING
  /*------------------------- State Shift END-------------------------*/

  /* LED */
  if (io_cansat->state_data == DROPPING) {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  } else {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }
}

// ------------------------------------------------------------------------------
// Sequence: 着地後
// State   : パラ分離
// state 3 : LANDING
// ------------------------------------------------------------------------------
void MyLANDING (CANSAT* io_cansat) {
  if (io_cansat->sequence_data.ReleaseStartTime == 0) {
    io_cansat->sequence_data.ReleaseStartTime = millis();
  }
  /*--------------------------- State Shift---------------------------*/
  Releasing (io_cansat); // LANDING -> RUNNING & Releasing the Parachute
  /*------------------------- State Shift END-------------------------*/

  /* LED */
  if (io_cansat->state_data == LANDING) {
    MyLEDsBlinking(io_cansat, interval_1_sec, LED_num3);
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
  } else {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }
}

// ------------------------------------------------------------------------------
// Sequence: 走行フェーズ
// State   : 各センサをフルに使用して目的地に向かう
//              --------------------------------------------
//                          スタック : RUNNING -> STUCK
//                            ゴール : RUNNING -> GOAL
//                 GPS衛星補足数不足 : RUNNING -> IDLING
//              --------------------------------------------
// state 4 : RUNNING
// ------------------------------------------------------------------------------
void MyRUNNING (CANSAT* io_cansat) {
  /* Running */
  WitchWayToGo(io_cansat);
  MotorControl(io_cansat);
  
  /*--------------------------- State Shift---------------------------*/
  GoalJudge (io_cansat);
  //  StuckJudge (io_cansat);
  /*------------------------- State Shift END-------------------------*/

  /* LED */
  if (io_cansat->state_data == RUNNING) {
    if ((io_cansat->sequence_data.LED1_flag == 0 && io_cansat->sequence_data.LED2_flag == 0 && io_cansat->sequence_data.LED3_flag == 0)
        || (io_cansat->sequence_data.LED1_flag == 1 && io_cansat->sequence_data.LED2_flag == 1 && io_cansat->sequence_data.LED3_flag == 1)) {
      io_cansat->sequence_data.ledState[LED_num1 - 1] = HIGH;
      io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
      if (io_cansat->sequence_data.LED1_flag == 0) {
        io_cansat->sequence_data.LED1_flag = 1;
      } else if (io_cansat->sequence_data.LED1_flag == 1) {
        io_cansat->sequence_data.LED1_flag = 0;
      }
    } else if ((io_cansat->sequence_data.LED1_flag == 1 && io_cansat->sequence_data.LED2_flag == 0 && io_cansat->sequence_data.LED3_flag == 0)
               || (io_cansat->sequence_data.LED1_flag == 0 && io_cansat->sequence_data.LED2_flag == 1 && io_cansat->sequence_data.LED3_flag == 1)) {
      io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num2 - 1] = HIGH;
      io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
      if (io_cansat->sequence_data.LED2_flag == 0) {
        io_cansat->sequence_data.LED2_flag = 1;
      } else if (io_cansat->sequence_data.LED2_flag == 1) {
        io_cansat->sequence_data.LED2_flag = 0;
      }
    } else if ((io_cansat->sequence_data.LED1_flag == 1 && io_cansat->sequence_data.LED2_flag == 1 && io_cansat->sequence_data.LED3_flag == 0)
               || (io_cansat->sequence_data.LED1_flag == 0 && io_cansat->sequence_data.LED2_flag == 0 && io_cansat->sequence_data.LED3_flag == 1)) {
      io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num3 - 1] = HIGH;
      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
      if (io_cansat->sequence_data.LED3_flag == 0) {
        io_cansat->sequence_data.LED3_flag = 1;
      } else if (io_cansat->sequence_data.LED3_flag == 1) {
        io_cansat->sequence_data.LED3_flag = 0;
      }
    } else {
      io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
    }
  } else {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }
  //  Serial.print("\n");
  //  Serial.print(io_cansat->sequence_data.LED1_flag);
  //  Serial.print(io_cansat->sequence_data.LED2_flag);
  //  Serial.print(io_cansat->sequence_data.LED3_flag);
}

// ------------------------------------------------------------------------------
// Sequence: アイドリング（あってもなくてもいい）
// State   : 衛星の捕捉数等が足りない場合には待機
// state 5 : IDLING
// ------------------------------------------------------------------------------
void MyIDLING (CANSAT* io_cansat) {
  /*--------------------------- State Shift---------------------------*/

  /*------------------------- State Shift END-------------------------*/

  /* LED */
  if (io_cansat->state_data == IDLING) {
    MyLEDsBlinking(io_cansat, interval_4_sec, LED_num1);
    MyLEDsBlinking(io_cansat, interval_4_sec, LED_num2);
    MyLEDsBlinking(io_cansat, interval_4_sec, LED_num3);
  } else {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }
}

// ------------------------------------------------------------------------------
// Sequence: スタック
// State   : スタック検知（ジャイロ＆GPS）→スタック防止機構のモータ回転
// state 6 : STUCK
// ------------------------------------------------------------------------------
void MySTUCK (CANSAT* io_cansat) {

  /*やりたいこと---------------------------------------------------
    1秒ごとに左右のモータへの出力を変化させてスタックを脱出
    -----------------------------------------------------------------*

    /*--------------------------- State Shift---------------------------*/
  StuckJudge (io_cansat);
  /*------------------------- State Shift END-------------------------*/
  unsigned long CurrentMillis = millis();
  if (io_cansat->sequence_data.PreviousTime == 0) {
    io_cansat->sequence_data.PreviousTime = millis();
  } else if (io_cansat->sequence_data.PreviousTime != 0) {
    if (CurrentMillis - io_cansat->sequence_data.PreviousTime < 1000) {
      analogWrite(PinMotorLeft, 250);
      analogWrite(PinMotorRight, 200);
    } else if (CurrentMillis - io_cansat->sequence_data.PreviousTime > 1000 && CurrentMillis - io_cansat->sequence_data.PreviousTime < 2000) {
      analogWrite(PinMotorLeft, 200);
      analogWrite(PinMotorRight, 250);
      io_cansat->sequence_data.PreviousTime = 0;
    }
  }

  io_cansat->sequence_data.Stuck_flag = 1;


  /* LED */
  // スタック（放心状態）
  if (io_cansat->state_data == STUCK) {
    io_cansat->sequence_data.ledState[LED_num2 - 1] = HIGH;
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    // スタック防止機構稼働中 ----------------------------------------------
    if (io_cansat->state_data == STUCK && io_cansat->sequence_data.Stuck_flag == 1) {
      MyLEDsBlinking(io_cansat, interval_1_sec, LED_num1);
      MyLEDsBlinking(io_cansat, interval_1_sec, LED_num3);
    } else {
      io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
      io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
    }
  } else {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num2 - 1] = LOW;
    io_cansat->sequence_data.ledState[LED_num3 - 1] = LOW;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);
  }
}


// ------------------------------------------------------------------------------
// Sequence: ゴール検知
// State   : ゴーーーーーーーーーーーーーーーーーーーーーーーール!!!!!!!!!!!!
// state 7 : GOAL
// ------------------------------------------------------------------------------
void MyGOAL (CANSAT* io_cansat) {
  analogWrite(PinMotorLeft, 0);
  analogWrite(PinMotorRight, 0);
  /*--------------------------- State Shift---------------------------*/
  GoalJudge (io_cansat);
  /*------------------------- State Shift END-------------------------*/

  /* LED */
  unsigned long AfterGoalTime = millis();
  if (io_cansat->sequence_data.Goal_flag = 1) {
    io_cansat->sequence_data.ledState[LED_num1 - 1] = HIGH;
    digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1 - 1]);
    io_cansat->sequence_data.ledState[LED_num2 - 1] = HIGH;
    digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2 - 1]);
    io_cansat->sequence_data.ledState[LED_num3 - 1] = HIGH;
    digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3 - 1]);


    //    if (io_cansat->state_data == GOAL && AfterGoalTime - io_cansat->sequence_data.GoalTime < GoalLEDs_thre){
    //      io_cansat->sequence_data.ledState[LED_num1-1] = HIGH;
    //      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1-1]);
    //      io_cansat->sequence_data.ledState[LED_num2-1] = HIGH;
    //      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2-1]);
    //      io_cansat->sequence_data.ledState[LED_num3-1] = HIGH;
    //      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3-1]);
    //    } else {
    //      io_cansat->sequence_data.ledState[LED_num1-1] = LOW;
    //      digitalWrite(LED_1_pin, io_cansat->sequence_data.ledState[LED_num1-1]);
    //      io_cansat->sequence_data.ledState[LED_num2-1] = LOW;
    //      digitalWrite(LED_2_pin, io_cansat->sequence_data.ledState[LED_num2-1]);
    //      io_cansat->sequence_data.ledState[LED_num3-1] = LOW;
    //      digitalWrite(LED_3_pin, io_cansat->sequence_data.ledState[LED_num3-1]);
    //    }
  }
}

