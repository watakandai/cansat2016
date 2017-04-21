/**********************************************************************************
 * FILE        :MyControl.h
 * Update      :2016/5/29
 * Content     :_20160529_distributed
 * Author      :小澤
 * Memo        : 
 ***********************************************************************************/

#ifndef _MYPID_H_
#define _MYPID_H_

float pid_control(float sensor_val, float target_val);
float math_limit(float pid_val, float pid_min, float pid_max);

#endif
