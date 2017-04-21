/**********************************************************************************
 * FILE        :MyStruct.h
 * Update      :2016/07/11
 * Content     :_20160711_StuckEscaping&Goal
 * Author      :小澤
 * Memo        :0828杉原GPS変更
 ***********************************************************************************/

#ifndef MYSTRUCT_H_
#define MYSTRUCT_H_

//----------------------------------------------------------------
// STATE cansatの状態を表す列挙型 (追加20160523)
//----------------------------------------------------------------
typedef enum _STATE_ {
  PREPARING = 0,   // 準備状態（電源オン→GPS衛星補足までの間）
  FLYING = 1,     // 地上期（運搬→ロケット収納→ロケット発射）
  DROPPING = 2,    // 降下中
  LANDING = 3,     // ジャイロ加速度センサによる着地検知
  RUNNING = 4,    // 走行中
  IDLING = 5,     // アイドリング
  STUCK = 6,      // スタック中
  GOAL = 7,       // ゴール
} STATE;

//----------------------------------------------------------------
// GPS
//----------------------------------------------------------------
typedef struct _GPS_{
  float  Time;
  float  LatDo;
  float  LatHun;
  long   LatDoReal;
  long  LatHunReal; //0828 f->l
  long  Lat;        //0828 f->l
  float  LonDo;
  float  LonHun;
  long   LonDoReal;
  long  LonHunReal; //0828 f->l
  long  Lon;        //0828 f->l
  short  SatNum;
  double Alt;
  double PosAccuracy;

//  String SendDataString;
//  String SendCommandString;
}GPS;

//----------------------------------------------------------------
// デジタルコンパス
//----------------------------------------------------------------
typedef struct _COMPASS_ {
  float fvalue;
  float pvalue[3] = {0}; // Initializing by 0
}COMPASS;

//----------------------------------------------------------------
// 光センサ
//----------------------------------------------------------------
typedef struct _LIGHT_ {
  int light_value;
} LIGHT;


//----------------------------------------------------------------
// ジャイロ（回転）+加速度（並進）
//----------------------------------------------------------------
typedef struct _GYRO_ {
  float x_accel;        // acclerometer in the x axis
  float y_accel;        // acclerometer in the y axis
  float z_accel;        // acclerometer in the z axis
  float x_gyro;         // angular velocity in the x axis
  float y_gyro;         // angular velocity in the y axis
  float z_gyro;         // angular velocity in the z axis
  float x_euler;        // euler angle in the x axis
  float y_euler;        // euler angle in the y axis
  float z_euler;        // euler angle in the z axis

  /*着地検知用に一時刻前データを保存*/
  float pre_x_gyro;      // roll
  float pre_y_gyro;      // pitch
  float pre_z_gyro;      // yaw

  float pre_x_angle;      // roll
  float pre_y_angle;      // pitch
  float pre_z_angle;      // yaw

  float pre_x_accel;      // roll
  float pre_y_accel;      // pitch
  float pre_z_accel;      // yaw
    
  // Kalman Filter
  float x_euler_kal;    // Kalman filtered angular velocity in the x axis
  float y_euler_kal;    // Kalman filtered angular velocity in the y axis
  
  // 1st Complimentary Filter 
  float x_euler_comp;   // Complimentary filtered angular velocity in the x axis
  float y_euler_comp;   // Complimentary filtered angular velocity in the y axis

  // Moving average filter variables
  float XaccMA = 0;
  float YaccMA = 0;
  float ZaccMA = 0;
  float Xvalue[3] = {0}; // initializing by 0
  float Yvalue[3] = {0}; // initializing by 0
  float Zvalue[3] = {0}; // initializing by 0
} GYRO;

//----------------------------------------------------------------
// SD（OpenLog）
//----------------------------------------------------------------
typedef struct _OPENLOG_ {
  String light_log;
  String compass_log;
  String gps_log;
  String gyro_log;
  String send_log;
  String all_log;
  String Date;
  String MicroSec;
  String MicroSec_log;
  String guidance_log;
  String state_log;
  int count = 0;
} OPENLOG;


//----------------------------------------------------------------
// エンコーダー
//----------------------------------------------------------------
typedef struct _ENCODER_ {
  int flg_pre_r;
  int count_r;
  int flg_pre_l;
  int count_l;
  int count;
  double roll_right; //回転数の保管
  double roll_left;
} ENCODER;

//----------------------------------------------------------------
// 制御器
//----------------------------------------------------------------
typedef struct _CONTROL_ {
  
} CONTROL;

//----------------------------------------------------------------
// 距離・角度の算出
//----------------------------------------------------------------
typedef struct _DESTINATION_ {
  
} DESTINATION;

//----------------------------------------------------------------
// 自己位置推定
//----------------------------------------------------------------
typedef struct _LOCALIZATION_ {
  
} LOCALIZATION;

//----------------------------------------------------------------
// 無線（MU2）
//----------------------------------------------------------------
typedef struct _SENDDATA_{
  String SendDataString;
  String SendCommandString;
  bool ack;                        // 20160502 地上局に届いたか
  unsigned long sendTime;          // 送った時間
  unsigned long resTime;           // 帰ってきた時間
  int count = 0;
}SENDDATA;

//----------------------------------------------------------------
// 誘導（ガイダンス）
//----------------------------------------------------------------
typedef struct _GUIDANCE_{
  long rober_Lat;   //本体の緯度座標  0828 f->l
  long rober_Lon;   //本体の経度 0828 f->l
  double delta_Lat;
  double delta_Lon;
  int flag = 0;
  float degree_azimuth_north;   //ローバと北方向との角度のずれ
  float degree_pre_dest;   //ローバから見たゴールの方向
  float degree_dest;   //ローバから見たゴールの方向
  float degree_difference_rad;  //
  float degree_difference_deg;
  float distance_to_dest;   //ゴールまでの距離
  float distance_to_goal;
  int curve;   //舵角
  float steer;//舵角          //小林追加0729
  float dest_Lon_transferred; //added(2016/07/02)
  float dest_Lat_transferred; //added(2016/07/02)
  float degree_to_dest; //added(2016/07/02)
  int PWM_L;
  int PWM_R;
  float u; // control input u
 } GUIDANCE;


//----------------------------------------------------------------
// シークエンス
//----------------------------------------------------------------
typedef struct _SEQUENCE_{
  unsigned long Series_num1 = 0;                   // 着地判定のための待機ループ 0 -> Series_num_thre
  unsigned long Series_num2 = 0;                   // 着地判定のための待機ループ 0 -> Series_num_thre
  unsigned long Series_num3 = 0;                   // 着地判定のための待機ループ 0 -> Series_num_thre
  unsigned long Count_0to1 = 0;
  unsigned long Count_1to2 = 0;
  int ledState[3];                  // 各LEDの状態(LOW or HIGH)を記憶する変数
  int LED1_flag = 0;
  int LED2_flag = 0;
  int LED3_flag = 0;
  double PreviousBlinkingTime[3];       // 各LEDにおいて状態が変化した時の時間を保存する変数
  unsigned long WelcomeOnBoardTime;     // GPS補足完了 -> CanSat始動時間
  unsigned long DropStartTime = 0;      // 分離開始時間
  unsigned long ReleaseStartTime = 0;   // 分離開始時間
  unsigned long RunningStartTime = 0;   // 走行開始時間
  unsigned long EvasionStartTime = 0;   // スタック防止機構 稼働開始時間
  unsigned long GoalTime;           // ゴール時間
  bool Stuck_flag = 0;              // スタック脱出機構稼働フラグ 0 -> 1
  bool Goal_flag = 0;               // ゴール検知フラグ 0 -> 1
  bool GPGGA_flag = 1;              // GPGGA 取得確認フラグ 0 -> 1
//  float Pre1sGpsLat;                // 5s前のGPS Lat
//  float Pre1sGpsLon;                // 5s前のGPS Lat
//  long PreviousGoalJudgeTime = 0;   // １s前のGPS保存時刻
  float Pre20sGpsLat;                // 10s前のGPS Lat
  float Pre20sGpsLon;                // 10s前のGPS Lon
  unsigned long PreGpsPollingTime = 0;  // 10s前のGPS保存時刻
  float PreviousTime;               // スタック防止機構用
  int flag = 0;                         // 初回判定フラグ
  int LR_flag = 0;                     
  int NEXT_flag = 0;
}SEQUENCE;

//----------------------------------------------------------------
// CanSat
//----------------------------------------------------------------
typedef struct _CANSAT_ {
  ENCODER encoder_data;
  STATE state_data;
  LIGHT light_data;
  COMPASS compass_data;
  GPS gps_data;
  SEQUENCE sequence_data;
  OPENLOG openlog_data;
  CONTROL control_data;
  DESTINATION destination_data;
  LOCALIZATION loc_data;
  SENDDATA send_data;
  GYRO gyro_data;
  GUIDANCE guidance_data;
} CANSAT;

#endif
