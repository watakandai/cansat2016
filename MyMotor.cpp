/**********************************************************************************
   FILE        :MyMotor.cpp
   Update      :2016/09/02
   Content     :
   Author      :kobayashi, ozawa
   Memo        :ゴール付近ではゆっくり走行するように変更 ozawa
 ***********************************************************************************/

#include "Arduino.h"
#include "main.h"
#include <math.h>


void MotorControl(CANSAT* io_cansat) {
  if (io_cansat->guidance_data.curve == 1) {
    io_cansat->guidance_data.PWM_L = 210;
    if (io_cansat->guidance_data.steer / 180 >= 0.5) {
      io_cansat->guidance_data.PWM_R = 200 * (1 - io_cansat->guidance_data.steer / 360); // 誘導修正 0902
    } else {
      io_cansat->guidance_data.PWM_R = 200 * (1 - io_cansat->guidance_data.steer / 180);
    }
    analogWrite(PinMotorLeft, io_cansat->guidance_data.PWM_L);
    analogWrite(PinMotorRight, io_cansat->guidance_data.PWM_R);
    //    analogWrite(PinMotorRight,200);
    //    analogWrite(PinMotorLeft,200*(1-io_cansat->guidance_data.steer/180));          //小林変更0729
    Serial.println("Turn Right");
  } else if (io_cansat->guidance_data.curve == -1) {
    io_cansat->guidance_data.PWM_R = 210;
    if (io_cansat->guidance_data.steer / 180 >= 0.5) {
      io_cansat->guidance_data.PWM_L = 200 * (1 - io_cansat->guidance_data.steer / 360); // 誘導修正 0902
    } else {
      io_cansat->guidance_data.PWM_L = 200 * (1 - io_cansat->guidance_data.steer / 180);
    }
    analogWrite(PinMotorLeft, io_cansat->guidance_data.PWM_L);
    analogWrite(PinMotorRight, io_cansat->guidance_data.PWM_R);
    //    analogWrite(PinMotorRight,200);
    //    analogWrite(PinMotorLeft,200*(1-io_cansat->guidance_data.steer/180));
    //    analogWrite(PinMotorLeft,200);
    //    analogWrite(PinMotorRight,200*(1-io_cansat->guidance_data.steer/180));
    Serial.println("Turn Left");
  } else {
    io_cansat->guidance_data.PWM_L = 200;
    io_cansat->guidance_data.PWM_R = 200;
    analogWrite(PinMotorLeft, io_cansat->guidance_data.PWM_L);
    analogWrite(PinMotorRight, io_cansat->guidance_data.PWM_R);
    //    analogWrite(PinMotorLeft, 200);
    //    analogWrite(PinMotorRight, 200);
    Serial.println("Go Straight");
  }
  //  Serial.print("dakaku");
  //  Serial.println(io_cansat->guidance_data.steer);
  io_cansat->guidance_data.u = float(io_cansat->guidance_data.curve) * pow(float(io_cansat->guidance_data.PWM_L) / float(io_cansat->guidance_data.PWM_R), -io_cansat->guidance_data.curve);
  Serial.println(io_cansat->guidance_data.u);
}

void Run2Goal(CANSAT* io_cansat) { // ゴール付近でスピードゆるめて正確性向上
  if (fabs(io_cansat->guidance_data.delta_Lat) <= 0.000020 && fabs(io_cansat->guidance_data.delta_Lat) <= 0.000020) {
    if (io_cansat->guidance_data.flag == 0) {
      // ゆっくり減速
      PinMotorLeft, io_cansat->guidance_data.PWM_L = 120;
      PinMotorLeft, io_cansat->guidance_data.PWM_R = 120;
      analogWrite(PinMotorLeft, io_cansat->guidance_data.PWM_L);
      analogWrite(PinMotorRight, io_cansat->guidance_data.PWM_R);
      delay(100);
      PinMotorLeft, io_cansat->guidance_data.PWM_L = 80;
      PinMotorLeft, io_cansat->guidance_data.PWM_R = 80;
      analogWrite(PinMotorLeft, io_cansat->guidance_data.PWM_L);
      analogWrite(PinMotorRight, io_cansat->guidance_data.PWM_R);
      delay(100);
      PinMotorLeft, io_cansat->guidance_data.PWM_L = 0;
      PinMotorLeft, io_cansat->guidance_data.PWM_R = 0;
      analogWrite(PinMotorLeft, io_cansat->guidance_data.PWM_L);
      analogWrite(PinMotorRight, io_cansat->guidance_data.PWM_R);
      io_cansat->guidance_data.flag = 1; // 二度目はない
    }
    // ----------------------
    float gain = 0.6;
    // ----------------------
    if (io_cansat->guidance_data.curve == 1) {
      io_cansat->guidance_data.PWM_L = 210;
      if (io_cansat->guidance_data.steer / 180 >= 0.5) {
        io_cansat->guidance_data.PWM_R = 200 * (1 - io_cansat->guidance_data.steer / 360); // 誘導修正 0902
      } else {
        io_cansat->guidance_data.PWM_R = 200 * (1 - io_cansat->guidance_data.steer / 180);
      }
      analogWrite(PinMotorLeft, int(io_cansat->guidance_data.PWM_L*gain));
      analogWrite(PinMotorRight, int(io_cansat->guidance_data.PWM_R*gain));
      //    analogWrite(PinMotorRight,200);
      //    analogWrite(PinMotorLeft,200*(1-io_cansat->guidance_data.steer/180));          //小林変更0729
      Serial.println("Turn Right");
    } else if (io_cansat->guidance_data.curve == -1) {
      io_cansat->guidance_data.PWM_R = 210;
      if (io_cansat->guidance_data.steer / 180 >= 0.5) {
        io_cansat->guidance_data.PWM_L = 200 * (1 - io_cansat->guidance_data.steer / 360); // 誘導修正 0902
      } else {
        io_cansat->guidance_data.PWM_L = 200 * (1 - io_cansat->guidance_data.steer / 180);
      }
      analogWrite(PinMotorLeft, int(io_cansat->guidance_data.PWM_L*gain));
      analogWrite(PinMotorRight, int(io_cansat->guidance_data.PWM_R*gain));
      //    analogWrite(PinMotorRight,200);
      //    analogWrite(PinMotorLeft,200*(1-io_cansat->guidance_data.steer/180));
      //    analogWrite(PinMotorLeft,200);
      //    analogWrite(PinMotorRight,200*(1-io_cansat->guidance_data.steer/180));
      Serial.println("Turn Left");
    } else {
      io_cansat->guidance_data.PWM_L = 200;
      io_cansat->guidance_data.PWM_R = 200;
      analogWrite(PinMotorLeft, int(io_cansat->guidance_data.PWM_L*gain));
      analogWrite(PinMotorRight, int(io_cansat->guidance_data.PWM_R*gain));
      //    analogWrite(PinMotorLeft, 200);
      //    analogWrite(PinMotorRight, 200);
      Serial.println("Go Straight");
    }
    //  Serial.print("dakaku");
    //  Serial.println(io_cansat->guidance_data.steer);
    io_cansat->guidance_data.u = float(io_cansat->guidance_data.curve) * pow(float(io_cansat->guidance_data.PWM_L) / float(io_cansat->guidance_data.PWM_R), -io_cansat->guidance_data.curve);
    Serial.println(io_cansat->guidance_data.u);
  }
}

