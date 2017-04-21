/**********************************************************************************
 * FILE        :MyControl.cpp
 * Update      :2016/5/29
 * Content     :_20160529_distributed
 * Author      :小澤
 * Memo        : 
 ***********************************************************************************/

 #include "Arduino.h"
#include "MyPID.h"
//#include "main.h"
//#include "MyStruct.h"

static float Diff[2];
static float Integral;

    //sensor_valは，Guidanceで算出された目的地との角度の値が格納される
    //target_valは，したがって0度になってくれれば，目標値にまっすぐ行ってくれるはずである
    //sensor_valの値がプラスの値のとき，左に曲がってほしい
    //おそらく予想だと，左に曲がってほしいときは，返り値がプラスのはずなので，
    //そのときは，その返り値を用いて，右モータの出力を強めればいいはず

//sensor_val はシステムの出力
//target_val は目標値
//Diff[] は目標値と出力の差（偏差e）
//operation = p+d+i は操作量の合計値

//float pid_control(float sensor_val, float target_val){
//  float p = 0;
//  float i = 0;
//  float d = 0;
//  float operation = 0;
//
//  Diff[0] = Diff[1];
//  Diff[1] = sensor_val - target_val;
////  Integral += (Diff[1] - Diff[0])/2.0*DELTA_T;
//  Integral += Diff[1]*DELTA_T;
//
//  p = Kp*Diff[1];
//  i = Ki*Integral;
//  d = Kd*(Diff[1] - Diff[0])/DELTA_T;
//
//  operation = p + i + d;
////  return math_limit(p + i + d,-100,100);
//}
//
////操作量が大きすぎた時の調整(2016/6/20使ってみるかどうか不明)
//float math_limit(float pid_val, float pid_min, float pid_max){
//
//  if(pid_val < pid_min){
//    return pid_min;
//  }else if(pid_val > pid_max){
//    return pid_max;
//  }
//
//  return pid_val;
//}
