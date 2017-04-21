/**********************************************************************************
 * FILE        :MyOpenLog.cpp
 * Update      :
 * Content     :_20160523_integ_seq
 * Author      :
 * Memo        :
 ***********************************************************************************/

#include <SoftwareSerial.h>
#include "Arduino.h"
#include "main.h"



SoftwareSerial OpenLog(OpenLog_Tx_pin, OpenLog_Rx_pin);

void Init_MyOpenLog(CANSAT* io_cansat, String Date){
  Serial2.begin(9600);
  delay(1000); //Wait a second for OpenLog to init
  Serial2.println(F("Run OpenLog...."));
  Serial.println(F("Run OpenLog ... "));
  Serial2.println(io_cansat->openlog_data.Date);
  Serial2.println(F("Current time [ms],  compass, light, gps_lat, gps_lon, satnum, PosAccuracy,  Control state, PWM_R, PWM_L, Control input, State, Xgyro, Ygyro, Zgyro"));

  /* Initializing visualizer */
  Serial.print(F("Date : ")); Serial.println(Date);
  Serial.println(F("Running !"));

}


void MyOpenLog(CANSAT* io_cansat){
  Serial2.end();
  Serial2.begin(9600);
  /* 保存時刻(MicroSec)の保存 */
  io_cansat->openlog_data.MicroSec = millis();

  //Send Data
  //-------------データの作成（Stringへの変換）-----------------------
  String MicroSec_log = (String) io_cansat->openlog_data.MicroSec;
  String compass_log = (String) io_cansat->compass_data.fvalue;
  String light_log = (String) io_cansat->light_data.light_value;
  String guidance_log = (String) io_cansat->guidance_data.curve;
  String state_log = (String) io_cansat->state_data;
  String gps_log = String(io_cansat->gps_data.Lat) + "," + String(io_cansat->gps_data.Lon) + "," + (String) io_cansat->gps_data.SatNum + "," +  (String) io_cansat->gps_data.PosAccuracy; //20160428 String変換時4桁に
  String PWM_log = String(io_cansat->guidance_data.PWM_R) + "," + String(io_cansat->guidance_data.PWM_L);                  //20160428 String変換時4桁に
  String u_log = String(io_cansat->guidance_data.u, 3);
  String gyro_log = String(io_cansat->gyro_data.x_gyro, 5) + ", " + String(io_cansat->gyro_data.y_gyro, 5) + ", " + String(io_cansat->gyro_data.z_gyro, 5);
  io_cansat->openlog_data.all_log += MicroSec_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += compass_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += light_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += gps_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += guidance_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += PWM_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += u_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += state_log;
  io_cansat->openlog_data.all_log += ",";
  io_cansat->openlog_data.all_log += gyro_log;

  //-------------データの送信-----------------------
//  if(io_cansat->openlog_data.count > 30){
    Serial2.println(io_cansat->openlog_data.all_log);
    Serial2.print(F("\r\n"));//改行コード
//    io_cansat->openlog_data.count = 0;
//  }else{
//    io_cansat->openlog_data.count = io_cansat->openlog_data.count + 1;
//  }
  Serial.println(io_cansat->openlog_data.all_log);
//  Serial.println(io_cansat->openlog_data.count);
}
