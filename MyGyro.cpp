/**********************************************************************************
 * FILE        :MyGyro.cpp
 * Update      :2016/7/4
 * Content     :_20160524_integ_seq  ※着地検知関数はこれから記述
 * Author      :小澤
 * Memo        :●カルマンフィルタをかませることにより，ドリフト誤差（動かしていないのに回転しているようなセンサの出力誤差）
                 を補正している．また，路面の外乱に対する堅牢性も確保し精度高めている．
                ●比較用に1次の相補フィルタも実装しているがメモリが余るようであればコメントアウト推奨．
                ●オイラー角表現における特異点(pi/2, -pi/2)はif文で計算式を変更している．
                  余裕があれば状態方程式から制御入力を作成する観点から，
                 特異点の生じないクォータニオン表現に変更するのもあり．
                ●並進加速度成分い3次の移動平均フィルタかませた(2016/07/07)
 ***********************************************************************************/
 
  //includeの順番を変えるべからず 前方宣言に不明によりコンパイルエラーの原因になる
#include "Arduino.h"
#include "main.h"
#include <SoftwareSerial.h>
#include <Wire.h>


// インスタンス呼び出し
Kalman kalmanX;
Kalman kalmanY;

/* IMU Data */
float accX, accY, accZ;    // 加速度
float gyroX, gyroY, gyroZ; // 速度
int16_t tempRaw;
uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data

float gyroXangle, gyroYangle, gyroZangle; // Angle calculate using the gyro only
float compAngleX, compAngleY; // Calculated angle using a complementary filter
float kalAngleX, kalAngleY; // Calculated angle using a Kalman filter

// kalman filter
float roll, pitch;

// ---------------------------------------------------------------------------------------------
// 初期化関数
// ---------------------------------------------------------------------------------------------
void Init_MyGyro() {
  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite2(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite1(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode
  while (i2cRead(0x75, i2cData, 1));
  
  accX = ((i2cData[0] << 8) | i2cData[1]);// + 2000.0;
  accY = ((i2cData[2] << 8) | i2cData[3]);// + 3319.84;
  accZ = ((i2cData[4] << 8) | i2cData[5]);// + 664.48;
  tempRaw = (i2cData[6] << 8) | i2cData[7];
  gyroX = (i2cData[8] << 8) | i2cData[9];
  gyroY = (i2cData[10] << 8) | i2cData[11];
  gyroZ = (i2cData[12] << 8) | i2cData[13];
  
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor")); Serial.print(F("\t"));
    while (1);
  }
  Serial.print(F("Accelerometer + Gyro sensor calibration ...   "));
  delay(100);  //
  Serial.println(F("Calibration completed !"));

#ifdef RESTRICT_PITCH // Eq. 25 and 26
  roll  = atan2(accY, accZ) * RAD_TO_DEG;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  kalmanX.setAngle(roll); // Set starting angle
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;

  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  pinMode(button, INPUT_PULLUP);

  timer = micros();
}

// ---------------------------------------------------------------------------------------------
// メイン関数
// ---------------------------------------------------------------------------------------------
void MyGyro(CANSAT* io_cansat) {
  Wire.end();
  Wire.begin(IMUAddress);
  /*着地検知用に一時刻前の角速度を保存*/
  io_cansat->gyro_data.pre_x_gyro = io_cansat->gyro_data.x_gyro;
  io_cansat->gyro_data.pre_y_gyro = io_cansat->gyro_data.y_gyro;
  io_cansat->gyro_data.pre_z_gyro = io_cansat->gyro_data.z_gyro;

  /*着地検知用に一時刻前の角度を保存*/
  io_cansat->gyro_data.pre_x_angle = io_cansat->gyro_data.x_euler_kal;
  io_cansat->gyro_data.pre_y_angle = io_cansat->gyro_data.y_euler_kal;
  io_cansat->gyro_data.pre_z_angle = io_cansat->gyro_data.z_euler;

  /*着地検知用に一時刻前の加速度を保存*/
  io_cansat->gyro_data.pre_x_accel = io_cansat->gyro_data.x_accel;
  io_cansat->gyro_data.pre_y_accel = io_cansat->gyro_data.y_accel;
  io_cansat->gyro_data.pre_z_accel = io_cansat->gyro_data.z_accel;
  
  /* Update all the values */
//  for (int count=0; count<3 ; count++){ // 加速度の移動平均フィルタ用に連続して3回分保存
//    while (i2cRead(0x3B, i2cData, 14));
//    accX = ((i2cData[0] << 8) | i2cData[1]);// + 2000.0;
//    accY = ((i2cData[2] << 8) | i2cData[3]);// + 3319.84;
//    accZ = ((i2cData[4] << 8) | i2cData[5]);// + 664.48;
//    io_cansat->gyro_data.Xvalue[count]=accX;
//    io_cansat->gyro_data.Yvalue[count]=accY;
//    io_cansat->gyro_data.Zvalue[count]=accZ;
//    if (count == 2){ // ジャイロにはカルマンフィルタかけるから3回取得するうちの最後を取得
//      tempRaw = (i2cData[6] << 8) | i2cData[7];
//      gyroX = (i2cData[8] << 8) | i2cData[9];
//      gyroY = (i2cData[10] << 8) | i2cData[11];
//      gyroZ = (i2cData[12] << 8) | i2cData[13];
//    }
//  }

  while(i2cRead(0x3B, i2cData, 14));
  /* 生データ */
  accX = ((i2cData[0] << 8) | i2cData[1]);// + 2000.0;  // 並進加速度
  accY = ((i2cData[2] << 8) | i2cData[3]);// + 3319.84; // 並進加速度
  accZ = ((i2cData[4] << 8) | i2cData[5]);// + 664.48;  // 並進加速度
  tempRaw = (i2cData[6] << 8) | i2cData[7];             // 気温
  gyroX = (i2cData[8] << 8) | i2cData[9];               // 回転角速度
  gyroY = (i2cData[10] << 8) | i2cData[11];             // 回転角速度
  gyroZ = (i2cData[12] << 8) | i2cData[13];             // 回転角速度
  
  /* 並進加速度に3次の移動平均フィルタ */
  io_cansat->gyro_data.Xvalue[2] = io_cansat->gyro_data.Xvalue[1]; //X axis --------
  io_cansat->gyro_data.Xvalue[1] = io_cansat->gyro_data.Xvalue[0];
  io_cansat->gyro_data.Xvalue[0] = accX;
  io_cansat->gyro_data.Yvalue[2] = io_cansat->gyro_data.Yvalue[1]; //Y axis --------
  io_cansat->gyro_data.Yvalue[1] = io_cansat->gyro_data.Yvalue[0];
  io_cansat->gyro_data.Yvalue[0] = accY;
  io_cansat->gyro_data.Zvalue[2] = io_cansat->gyro_data.Zvalue[1]; //Z axis --------
  io_cansat->gyro_data.Zvalue[1] = io_cansat->gyro_data.Zvalue[0];
  io_cansat->gyro_data.Zvalue[0] = accZ;

  float dt = (float)(micros() - timer) * 0.000001; // Calculate delta time
  timer = micros();

  // ソース: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2  -π to π (rad)の範囲で値を返す -  http://en.wikipedia.org/wiki/Atan2
  //  rad to deg
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  roll  = atan2(accY, accZ) * RAD_TO_DEG;//+++++++++++++++++++++++
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  float gyroXrate = gyroX * 0.00763359; // Convert to deg/s
  float gyroYrate = gyroY * 0.00763359; // Convert to deg/s
  float gyroZrate = gyroZ * 0.00763359; // Convert to deg/s

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
  
  #else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter
    
  if (abs(kalAngleY) > 90) {
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
  }
#endif

  gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  gyroZangle += gyroZrate * dt;
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;

  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;

  if (digitalRead(button) == LOW) {
    gyroZangle = 0;
  }

  // 構造体に格納--------------------------------------------------
  /*角加速度（生データ）*/
  io_cansat->gyro_data.x_accel = accX * 0.0000610351; //FS_SEL_0 16,384 LSB -> g
  io_cansat->gyro_data.y_accel = accY * 0.0000610351;
  io_cansat->gyro_data.z_accel = accZ * 0.0000610351;

  /*角速度（生データ）*/
  io_cansat->gyro_data.x_gyro = gyroXrate; //FS_SEL_0 131 LSB / (°/s)
  io_cansat->gyro_data.y_gyro = gyroYrate;
  io_cansat->gyro_data.z_gyro = gyroZrate;

  /*角度（生データの積分値）*/
  io_cansat->gyro_data.x_euler = roll;
  io_cansat->gyro_data.y_euler = pitch;
  io_cansat->gyro_data.z_euler = gyroZangle;

  /*角加速度（移動平均フィルタ後）*/
  io_cansat->gyro_data.XaccMA =  (Gw0*io_cansat->gyro_data.Xvalue[0]
                              + Gw1*io_cansat->gyro_data.Xvalue[1] + Gw2*io_cansat->gyro_data.Xvalue[2])/(3*16384.0);
  io_cansat->gyro_data.YaccMA =  (Gw0*io_cansat->gyro_data.Yvalue[0]
                              + Gw1*io_cansat->gyro_data.Yvalue[1] + Gw2*io_cansat->gyro_data.Yvalue[2])/(3*16384.0);
  io_cansat->gyro_data.ZaccMA =  (Gw0*io_cansat->gyro_data.Zvalue[0]
                              + Gw1*io_cansat->gyro_data.Zvalue[1] + Gw2*io_cansat->gyro_data.Zvalue[2])/(3*16384.0);
                              
  /* カルマンフィルタ */
  io_cansat->gyro_data.x_euler_kal = kalAngleX;
  io_cansat->gyro_data.y_euler_kal = kalAngleY;

  /* 相補フィルタ */
  io_cansat->gyro_data.x_euler_comp = compAngleX;
  io_cansat->gyro_data.y_euler_comp = compAngleY;

}

// ---------------------------------------------------------------------------------------------
// I2C通信のための write関数
// ---------------------------------------------------------------------------------------------

uint8_t i2cWrite1(uint8_t registerAddress, uint8_t data, bool sendStop) {
  return i2cWrite2(registerAddress, &data, 1, sendStop); // Returns 0 on success
}

uint8_t i2cWrite2(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop) {
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data, length);
  uint8_t rcode = Wire.endTransmission(sendStop); // Returns 0 on success
  if (rcode) {
    Serial.print(F("i2cWrite failed: "));
    Serial.println(rcode);
  }
  return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
}

// ---------------------------------------------------------------------------------------------
// I2C通信のための read関数
// ---------------------------------------------------------------------------------------------
uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  uint8_t rcode = Wire.endTransmission(false); // Don't release the bus
  if (rcode) {
    Serial.print(F("i2cRead failed: "));
    Serial.println(rcode);
  /*-------0831黒須　追加 ハングアップ対策-------*/
    if (rcode == 2 || rcode ==3 || rcode == 4){
      rcode = 0;}
    return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
  }
  /*---------------------------------------------*/
  Wire.requestFrom(IMUAddress, nbytes, (uint8_t)true); // Send a repeated start and then release the bus after reading
  for (uint8_t i = 0; i < nbytes; i++) {
    if (Wire.available())
      data[i] = Wire.read();
    else {
      timeOutTimer = micros();
      while (((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if (Wire.available())
        data[i] = Wire.read();
      else {
        Serial.println(F("i2cRead timeout"));
        return 5; // This error value is not already taken by endTransmission
      }
    }
  }
  return 0; // Success
}
