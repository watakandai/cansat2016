/**********************************************************************************
 * FILE        :MyMotorDriver.cpp
 * Update      :2016/4/22
 * Content     :_20160523_integ_seq
 * Author      :黒須
 * Memo        : 
 ***********************************************************************************/

#include "Arduino.h"
#include "main.h"


void Init_MyMotorDriver(){
  pinMode(Driver_pin_0, OUTPUT);
  pinMode(Driver_pin_1, OUTPUT);
}

void MyMotor_forward(){
      digitalWrite(Driver_pin_0, HIGH);
      digitalWrite(Driver_pin_1, LOW);
      analogWrite(Power_pin, Driver_PWM);
}

void MyMotor_back(){
      digitalWrite(Driver_pin_0, LOW);
      digitalWrite(Driver_pin_1, HIGH);
      analogWrite(Power_pin, Driver_PWM);
}

void MyMotor_stop(){
      digitalWrite(Driver_pin_0, LOW);
      digitalWrite(Driver_pin_1, LOW);
      analogWrite(Power_pin, Driver_PWM);
}


