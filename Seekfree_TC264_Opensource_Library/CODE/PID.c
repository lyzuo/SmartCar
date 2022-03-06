/*
 * PID.c
 *
 *  Created on: 2021年12月10日
 *      Author: yue
 */
#include "PID.h"

/********************************************************************************************
 ** 函数功能: 两个PID参数的赋值初始化
 ** 参    数: SteerPID *SteerK
 **           MotorPID *MotorK
 ** 返 回 值: 无
 ** 作    者: LJF
 *********************************************************************************************/
void PID_init(SteerPID *SteerK,MotorPID *MotorK)
{
    SteerK->P=0;SteerK->D=0;
    MotorK->P=0;MotorK->I=0;
}

/*
 *******************************************************************************************
 ** 函数功能: 根据偏差来求舵机PWM
 ** 参    数: float SlopeBias:    最小回归方程得出来的偏差
 **           SteerPID K:         进行PID的舵机PID参数
 ** 返 回 值: 给舵机的PWM
 ** 作    者: LJF
 ** 注    意：偏差是传进来还是传的是中线进来再根据那个函数求偏差？
 **           返回出去的PWM可能很小看看是在这里对PWM进行缩放还是把它归进去参数?
 **           Bias的正负是这里处理还是传进来之前，这个问题跟第一个问题有关联？
 ********************************************************************************************
 */
int Steer_Position_PID(float SlopeBias,SteerPID K)//舵机位置式PID控制，采用分段式PID控制
{
    static float LastSlopeBias;
    int PWM;
    PWM=K.P*SlopeBias+K.D*(SlopeBias-LastSlopeBias);
    LastSlopeBias=SlopeBias;
    return PWM;
}

/*
 *******************************************************************************************
 ** 函数功能: 电机增量式速度PI控制器，根据编码器的值来不断矫正电机的速度   //左电机
 ** 参    数: left_encoder：左电机编码器的值
 **           left_target ：左电机目标速度
 **           K           : 电机PID参数
 ** 返 回 值: 给电机的PWM
 ** 作    者: WBN
 ** 注    意：使用增量式PI控制的优点：
 **           1.算式中没有累加的过程，控制量只与近两次采样值有关，不容易产生大的误差
 **           2.输出的是增量，即变化量，可以有更好的容错
 ********************************************************************************************
 */
int16 Speed_PI_Left(int16 left_encoder,int16 left_target,MotorPID K)
{
    static int16 Bias,Last_Bias,PWM;    //当前偏差，上一次偏差，输出的PWM

    Bias=left_target-left_encoder;           //求出当前偏差，期望值-当前值
    PWM+=K.P*(Bias-Last_Bias)+K.I*Bias;      //增量式PI，并把结果直接叠加在上一次的PWM上
    Last_Bias=Bias;                          //为下一次PID保存这一次偏差

    return PWM;         //返回可以直接赋值给电机的PWM
}

/*
 *******************************************************************************************
 ** 函数功能: 电机增量式速度PI控制器，根据编码器的值来不断矫正电机的速度   //右电机
 ** 参    数: left_encoder：左电机编码器的值
 **           left_target ：左电机目标速度
 **           K           : 电机PID参数
 ** 返 回 值: 给电机的PWM
 ** 作    者: WBN
 ** 注    意：使用增量式PI控制的优点：
 **           1.算式中没有累加的过程，控制量只与近两次采样值有关，不容易产生大的误差
 **           2.输出的是增量，即变化量，可以有更好的容错
 ********************************************************************************************
 */
int16 Speed_PI_Right(int16 right_encoder,int16 right_target,MotorPID K)
{
    static int16 Bias,Last_Bias,PWM;    //当前偏差，上一次偏差，输出的PWM

    Bias=right_target-right_encoder;         //求出当前偏差，期望值-当前值
    PWM+=K.P*(Bias-Last_Bias)+K.I*Bias;      //增量式PI，并把结果直接叠加在上一次的PWM上
    Last_Bias=Bias;                          //为下一次PID保存这一次偏差

    return PWM;         //返回可以直接赋值给电机的PWM
}


