/**********************************************************************************
   FILE        :MyGuidance.cpp
   Update      :2016/5/29
   Content     :_20160529_distributed
   Author      :小澤
   Memo        :40-60付近変更（小林）0729 long仕様に変更0828
 ***********************************************************************************/

#include "Arduino.h"
#include "main.h"
#include <math.h>

void Init_MyGuidance() {
  pinMode(PinMotorRight, OUTPUT);
  pinMode(PinMotorLeft, OUTPUT);
}

void  WitchWayToGo(CANSAT* io_cansat) {
  io_cansat->guidance_data.rober_Lat = io_cansat->gps_data.Lat;  //GPSの緯度データを誘導用にする long
  io_cansat->guidance_data.rober_Lon = io_cansat->gps_data.Lon;  //GPSの経度データを誘導用にする long

  io_cansat->guidance_data.delta_Lat = (double)(dest_Lat - io_cansat->guidance_data.rober_Lat) * 0.000001;  //緯度の差
  io_cansat->guidance_data.delta_Lon = (double)(dest_Lon - io_cansat->guidance_data.rober_Lon) * 0.000001;  //経度の差
  io_cansat->guidance_data.distance_to_goal = sqrt(pow(io_cansat->guidance_data.delta_Lat, 2.0) + pow(io_cansat->guidance_data.delta_Lon, 2.0)); //目的地までの距離

  double LON = io_cansat->guidance_data.delta_Lon * cos(io_cansat->compass_data.fvalue*M_PI/180) - io_cansat->guidance_data.delta_Lat * sin(io_cansat->compass_data.fvalue*M_PI/180);
  double LAT = io_cansat->guidance_data.delta_Lon * sin(io_cansat->compass_data.fvalue*M_PI/180) + io_cansat->guidance_data.delta_Lat * cos(io_cansat->compass_data.fvalue*M_PI/180);

  
  float angle = 0;
  if( LAT > 0 ){
    angle = fabs(atan(LON/LAT))*180/M_PI;//単位はdeg
  }else{
    angle = 180-fabs(atan(LON/LAT))*180/M_PI;
  }


  
  if ( LAT > 0   ) { 
    if(fabs(atan(LON / LAT)) < Threshold * M_PI / 180){
      io_cansat->guidance_data.curve = 0;   //直進
      io_cansat->guidance_data.steer = angle;
      Serial.println(F("Go Straight!"));
    }else if(LON>0){
      io_cansat->guidance_data.curve = 1;  //右
      io_cansat->guidance_data.steer = angle;
      Serial.println(F("Turn Right!"));
    }else{
    io_cansat->guidance_data.curve = -1; //左
    io_cansat->guidance_data.steer = angle;
    Serial.println(F("Turn Left!"));     
    }
  } else if (LON > 0) {                     //右
    io_cansat->guidance_data.curve = 1;
    io_cansat->guidance_data.steer = angle;
    Serial.println(F("Turn Right!"));
  } else {                                  //左
    io_cansat->guidance_data.curve = -1;
    io_cansat->guidance_data.steer = angle;
    Serial.println(F("Turn Left!"));
  }
  
//  if ( (LAT > 0) && ((abs(atan(LON / LAT))) < Threshold * M_PI / 180) ) { //直進
//    io_cansat->guidance_data.curve = 0;
//    Serial.println("Go Straight!");
//  } else if (LON > 0) {                     //右
//    io_cansat->guidance_data.curve = 1;
//    Serial.println("Turn Right!");
//  } else {                                  //左
//    io_cansat->guidance_data.curve = -1;
//    Serial.println("Turn Left!");
//  }
}
