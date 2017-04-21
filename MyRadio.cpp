/**********************************************************************************
 * FILE        :MyRadio.cpp
 * Update      :2016/7/28
 * Content     :_20160523_integ_seq
 * Author      :杉原 送るデータ変更
 * Memo        :PackData:データを詰め込む、MU2_send:コマンドを作りMU-2に送信する
 ***********************************************************************************/

#include <stdio.h>
#include <string.h>
#include "Arduino.h"
#include "main.h"

//SoftwareSerial MU2Serial(MU2_Tx_pin, MU2_Rx_pin);

String head_ack = "head";
String end_ack = "end";
void Init_MyRadio(){
  Serial3.begin(MU2_BAUDRATE);
//  Serial3.begin(19200);
//  Serial3.print("@BR96/W\r\n"); // 一度だけ設定
  Serial3.print("@CH07\r\n"); //20160707 sugi CHを指定
  
  /* Initializing visualization */
  Serial.println(F("MU-2 Initializing. Please check your reciever. "));  
}
//20160502 Openlogに保存するデータを無線で送る
void MU2_Send(CANSAT* io_cansat){
  Serial3.end();
  Serial3.begin(MU2_BAUDRATE);
        /* MU-2に送るコマンドを生成する。SendCommandStringは実際にMU-2に送るコマンド、
     SendDataStringは無線で送りたいデータ。*/
   
//  io_cansat->send_data.sendTime =millis();  //20160502 送信時刻
  //-------------データの作成（Stringへの変換）-----------------------
//  io_cansat->openlog_data.MicroSec_log = (String) io_cansat->openlog_data.MicroSec;
//  io_cansat->openlog_data.compass_log = (String) io_cansat->compass_data.fvalue;
//  io_cansat->openlog_data.light_log = (String) io_cansat->light_data.light_value;
//  io_cansat->openlog_data.guidance_log = (String) io_cansat->guidance_data.curve;
//  io_cansat->openlog_data.state_log = (String) io_cansat->state_data;
//  io_cansat->openlog_data.gps_log = String(io_cansat->gps_data.Lat ,4) + "," + String(io_cansat->gps_data.Lon ,4) + "," + (String) io_cansat->gps_data.SatNum + "," +  (String) io_cansat->gps_data.PosAccuracy; //20160428 String変換時4桁に
//  io_cansat->openlog_data.gyro_log = String(io_cansat->gyro_data.x_euler_kal ,4) + "," + String(io_cansat->gyro_data.y_euler_kal ,4);                  //20160428 String変換時4桁に
//  //io_cansat->openlog_data.all_log = io_openlog->io_cansat->openlog_data.compass_log + "," + io_cansat->openlog_data.light_log + "," + io_cansat->openlog_data.gps_log + "," + io_cansat->openlog_data.gyro_log ; 
//  //SendDataStringにデータを詰める
    io_cansat->send_data.SendDataString = head_ack + "," + io_cansat->openlog_data.all_log + "," + end_ack; 
  
  //------------- データの送信-------------------

    io_cansat->send_data.SendCommandString = "@DT";    
    if (io_cansat->send_data.SendDataString.length() < 16){// データサイズは2桁の16進数で指定しなければならないので、1桁の時は0で埋める
      io_cansat->send_data.SendCommandString += String("0");
      io_cansat->send_data.SendCommandString += String(io_cansat->send_data.SendDataString.length(), HEX);
    } else {io_cansat->send_data.SendCommandString += String(io_cansat->send_data.SendDataString.length(), HEX);}

    io_cansat->send_data.SendCommandString += io_cansat->send_data.SendDataString;  //データの中身
    io_cansat->send_data.SendCommandString += String("/A");  //20160516 ACK追加
    io_cansat->send_data.SendCommandString += String("\r\n");  //〆の改行コード

//    if(io_cansat->send_data.count > 30){
      Serial3.print(io_cansat->send_data.SendCommandString);
//      io_cansat->send_data.count = 0;
//    }else{
//      io_cansat->send_data.count = io_cansat->send_data.count + 1;
//    }
//    Serial.print(io_cansat->send_data.SendCommandString);
}

void CommandSendTest(CANSAT* io_cansat){   //デバッグ用　MU-2がコマンドを受け取ったか確かめる
    char receiveBuf[7];
    int count = 0;
    do {
      if ( Serial3.available() ) {
        receiveBuf[count] = Serial3.read();
        //Serial.print(receiveBuf[count]);
        count++;
      }else break;

      if (count > 6){
        receiveBuf[6]='\0';

        //20160516　 受信した内容によって場合分け
        if(strncmp(receiveBuf,"*DR",3)==0){ //20160502 受信応答が来たか記録
            //o_send->ack = 1;
            //o_send->resTime =millis();  //20160502 受信時刻
            Serial.print(F("ack\r\n"));
            break;
        }
        else if(strncmp(receiveBuf,"*DT",3)==0){ //20160516 送信レスポンスなら一回捨てる  
            Serial.print(F("responce\r\n")); 
            digitalWrite(LED_2_pin, HIGH); //S
            count=0;
        }
        else{
         // o_send->ack = 0;
         Serial.print(F("empty\r\n"));
          break;
        }
      }
      //if (count > 250)break;
    } while (receiveBuf[count - 1] != 0x0A); //0x0A means 'return'   
      Serial.print(receiveBuf);
}
