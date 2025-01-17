/*
 * FuzzyPID.h
 *
 *  Created on: 2022年1月21日
 *  Author: 30516
 *  Effect: 用于模糊PID
 */

#ifndef CODE_FUZZYPID_H_
#define CODE_FUZZYPID_H_

#include "common.h" //数据类型声明

//对数据模糊化的宏定义{NB,NM,NS,ZO,PS,PM,PB},不用数组是因为方便后面规则表的编写
//******************模糊子集******************************
#define NB -3
#define NM -2
#define NS -1
#define ZO  0
#define PS  1
#define PM  2
#define PB  3
//*******************************************************

//隶属值的单位值：FuzzyArray[]*这个=隶属值，该值需要调参使用，(MAX-MIN)/8得到
/*******************物理论域宏定义****************************/
//*******************************************************
#define BiasMembership 60 //偏差
#define BiasCMembership 10//偏差于上一次偏差的差
#define SteerKP 1
#define SteerKd 1
#define MotorKP 1
#define MotorKI 1
//********************************************************

/*************************模糊规则***********************/
uint8 KPFuzzyRule[7][7]={{PB,PB,PM,PM,PS,ZO,ZO},
                         {PB,PB,PM,PS,PS,ZO,NS},
                         {PM,PM,PM,PS,ZO,NS,NS},
                         {PM,PM,PS,ZO,NS,NM,NM},
                         {PS,PS,ZO,NS,NS,NM,NM},
                         {PS,ZO,NS,NM,NM,NM,NB},
                         {ZO,ZO,NM,NM,NM,NB,NB}};//KP参数的模糊规则表
uint8 KIFuzzyRule[7][7]={{NB,NB,NM,NM,NS,ZO,ZO},
                         {NB,NB,NM,NS,NS,ZO,ZO},
                         {NB,NM,NS,NS,ZO,PS,PS},
                         {NM,NM,NS,ZO,PS,PM,PM},
                         {NM,NS,ZO,PS,PS,PM,PB},
                         {ZO,ZO,PS,PS,PM,PB,PB},
                         {ZO,ZO,PS,PM,PM,PB,PB}};//KI参数的模糊规则表
uint8 KDFuzzyRule[7][7]={{PS,NS,NB,NB,NB,NM,PS},
                         {PS,NS,NB,NM,NM,NS,ZO},
                         {ZO,NS,NM,NM,NS,NS,ZO},
                         {ZO,NS,NS,NS,NS,NS,ZO},
                         {ZO,ZO,ZO,ZO,ZO,ZO,ZO},
                         {PB,NS,PS,PS,PS,PS,PB},
                         {PB,PM,PM,PM,PS,PS,PB}};//KD参数的模糊规则表
/**********************************************************/

void ClacMembership(float E,float Membership[2],int Index[2]);//求出偏差的隶属值与隶属度
int  SolutionFuzzy(int IndexE[2],float MSE[2],int IndexEC[2],float MSEC[2],int type);//解模糊

#endif /* CODE_FUZZYPID_H_ */
