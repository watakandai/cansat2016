/**********************************************************************************
 * FILE        :main.h
 * Update      :2016/5/24
 * Content     :_20160523_integ_seq
 * Author      :小澤
 * Memo        :・ここで全部のヘッダをinclude 0901 goal関連変更
 *       ・設計変数等はここで定義
 ***********************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

/* 全ヘッダをインクルード */
#include <FlexiTimer2.h>
#include <stdio.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <Wire.h>
#include <math.h>
#include "MyStruct.h"
#include "MyMotor.h"
#include "MySequence.h"
#include "MyOpenLog.h"
#include "MyGyro.h"
#include "MyDigitalCompass.h"
#include "MyGPS.h"
#include "MyMotorDriver.h"
#include "MyLight.h"
#include "MyRadio.h"
#include "MyKalman.h"
#include "MyEncoder.h"
#include "MyGuidance.h"
#include "MyLocalization.h"
#include "MyPID.h"
#include "MyGuidance.h"
#include "MyCanSat.h"

// -----------------------------------------------------------------------
// ピン番号 (20160524現在)         ※ I2C通信センサは使用ピン: 20, 21
// -----------------------------------------------------------------------

/* デジタル */
#define release_pin       6   // パラ分離用ピン
#define PinMotorLeft      7   // 走行用モータ(右)
#define PinMotorRight     8   // 走行用モータ（左）    
#define Power_pin         9   // モータドライバ（スタック防止）
#define Driver_pin_0     10   // モータドライバ（スタック防止）
#define Driver_pin_1     11   // モータドライバ（スタック防止）
#define MU2_Rx_pin       14   // 無線
#define MU2_Tx_pin       15   // 無線
#define OpenLog_Rx_pin   16   // SD (Openlog)
#define OpenLog_Tx_pin   17   // SD (Openlog)
#define GPS_Rx_pin       18   // GPS
#define GPS_Tx_pin       19   // GPS

/* アナログ */
#define LIGHT_pin        A0   // 光センサ
#define LED_3_pin        A1   // ステート確認ピン
#define LED_2_pin        A2   // ステート確認ピン
#define LED_1_pin        A3   // ステート確認ピン
#define encoder_pin_r    A6   // エンコーダ 1（右：読み取り）
#define encoder_pin_l    A7   // エンコーダ 2（左：読み取り）


// -----------------------------------------------------------------------
// I2C アドレス
// -----------------------------------------------------------------------
const uint8_t IMUAddress = 0x68;         // ジャイロ加速度センサ
#define compassAddress  0x42 >> 1        //=0x21 // デジコン


// -----------------------------------------------------------------------
// メイン( .ino )
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
// 自己位置推定
// -----------------------------------------------------------------------


// ----------------------------------------------------------
// 0828以降　GOALは小数点なしで書いて！

// ----------------------------------------------------------
/* 自己位置 */
#define GUIDANCE_judge 10
#define ROLL_ANGLE 10

//#define dest_Lat 35.5563458  // Destination latitude
//#define dest_Lon 139.6533326 // Destination longtitude

//#define dest_Lat 35.554931  // Destination latitude
//#define dest_Lon 139.655640 // Destination longtitude

//#define dest_Lat 35.742908 //  kikyuu
//#define dest_Lon 140.011862

//#define dest_Lat 35.546773
//#define dest_Lon 139.640004

//#define dest_Lat 35.5409799 // Tsunashima park
//#define dest_Lon 139.6353286

//#define dest_Lat 40.142725 // Noshiro
//#define dest_Lon 139.987305

//// 20160826 ------------------------------------------------
//
//#define dest_Lat 35554654 // 矢上 グラウンド奥
//#define dest_Lon 139656959
//
#define dest_Lat 35554985 // 矢上 グラウンド手前
#define dest_Lon 139656031

////#define dest_Lat  35.584569 // 多摩川　左
////#define dest_Lon 139.666498
//
////#define dest_Lat  35.583877 // 多摩川　左
////#define dest_Lon 139.667584
//
//// ----------------------------------------------------------

// 20160827 ------------------------------------------------

//#define dest_Lat 35.552654 // 日吉キャンパス　左奥
//#define dest_Lon 139.650603

//#define dest_Lat 35.552365 // 日吉キャンパス　右奥
//#define dest_Lon 139.650505

//#define dest_Lat  35.557534 // 矢上公園　奥
//#define dest_Lon 139.653545


//#define dest_Lat  35557534 // 矢上公園　奥
//#define dest_Lon 139653545

//#define dest_Lat  35.558928 // 日吉公園
//#define dest_Lon 139.656487



// ----------------------------------------------------------
// 0828以降　GOALは小数点なしで書いて！

// ----------------------------------------------------------
#define Threshold 20

/* 自己位置推定 */


// -----------------------------------------------------------------------
// 制御器
// -----------------------------------------------------------------------
#define DELTA_T 0.004 //PID制御の周期
#define Kp 5.0 //比例ゲイン
#define Ki 2.0 //積分ゲイン
#define Kd 3.0 //微分ゲイン

// -----------------------------------------------------------------------
// シークエンス
// -----------------------------------------------------------------------

/*シークエンス遷移用*/
#define PRE2FLY_thre        100      // キャリア格納判定用　（光センサの閾値）
#define FLY2DROP_thre       500      // 放出判定用　　　　　（光センサの閾値）
#define LandingTime_thre 180000      // 着地検知の冗長系：時間でシークエンス移行
#define ReleaseTime_thre  15000      // パラ分離時間の閾値 [ms]
#define EvasionTime_thre  10000      // スタック防止機構 稼働時間の閾値 [ms]
#define GoForward_thre    20000      // 防止機構の稼働時間経過 -> この時間 [ms] まっすぐ進む
#define GoalLEDs_thre     20000      // ゴール後のLED点灯時間
#define Count_0to1_thre      30
#define Count_1to2_thre     100

/*着地検知*/
#define x_angle_thre        2.0      // roll軸角速度変化量の閾値
#define y_angle_thre        2.0      // pitch軸角速度変化量の閾値
#define z_angle_thre        2.0 // yaw軸角速度変化量の閾値
#define x_gyro_thre         250      // roll軸角度変化量の閾値
#define y_gyro_thre         250      // pitch軸角度変化量の閾値
#define z_gyro_thre         250      // yaw軸角速度変化量の閾値
#define x_accel_thre        0.1      // roll軸加速度変化量の閾値
#define y_accel_thre        0.1      // pitch軸加速度変化量の閾値
#define z_accel_thre        0.1      // yaw軸加速度変化量の閾値
#define Series_num_thre     200      // 着地判定のための待機ループ閾値

/* シークエンス確認用LED */
#define LED_num1              1
#define LED_num2              2
#define LED_num3              3

/* シークエンス確認用LED */
#define interval_1_sec     1000      // 1.0 s点滅用
#define interval_2_sec     2000      // 2.0 s点滅用 
#define interval_4_sec     4000      // 4.0 s点滅用

/* スタック */
#define Stuck_interval     5000      // 5.0 s
#define UP                  500      // 0.5 s
#define DOWN               5000      // 5.0 s

/* ゴール検知用 */
#define Goal_interval      1000      // 1.0 s
#define Goal_area            5      // 0901 整数で書いて！　1=000.000001です GPSの最小桁数は下6桁目

// -----------------------------------------------------------------------
// デジタルコンパス
// -----------------------------------------------------------------------
/* 移動平均フィルタ */
// 重み(合計が次数と同じになるように設計)，数字が若い順に新しいデータ，デフォは１
#define Dw0 2.8
#define Dw1 0.15
#define Dw2 0.05

// -----------------------------------------------------------------------
// GPS
// -----------------------------------------------------------------------
#define GPSBAUDRATE 9600


// -----------------------------------------------------------------------
// エンコーダ
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
// ジャイロ加速度センサ
// ----------------------------------------------------------------------
/* ジャイロ*/
#define RESTRICT_PITCH
#define led 13
#define button 2

/* I2C */
const uint16_t I2C_TIMEOUT = 1000;  // エラーチェック用 タイムアウト時間 [ms]

/* カルマンフィルタ */
// 計算重くなるから kalman.cpp で private定義してある

/* 移動平均フィルタ */
// 重み(合計が次数と同じになるように設計)，数字が若い順に新しいデータ，デフォは１
#define Gw0 1.0
#define Gw1 1.0
#define Gw2 1.0

// ----------------------------------------------------------------------
// モータドライバ
// ----------------------------------------------------------------------
#define Driver_PWM 150 // -> 2.7 V


// ----------------------------------------------------------------------
// SD (Openlog)
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// 無線 (MU-2)
// ----------------------------------------------------------------------
#define MU2_BAUDRATE 9600


#endif
