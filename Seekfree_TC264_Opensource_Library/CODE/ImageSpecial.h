/*
 * ImageSpecial.h
 *
 *  Created on: 2022年1月17日
 *      Author: yue
 */

#ifndef CODE_IMAGESPECIAL_H_
#define CODE_IMAGESPECIAL_H_

#include "Binarization.h"       //二值化之后的图像数组
#include "SEEKFREE_MT9V03X.h"   //为了要uint8这种定义,二值化算法中的某些数学计算,摄像头图像的全局变量
#include "ImageBasic.h"         //获取图像基本处理之后的数据

//起跑线识别
#define S_BLACK_WIDTH  3    //斑马线黑线宽度阈值 //用于判断该线是否为斑马线黑线
#define S_BLACK_NUM    8    //斑马线黑线数量阈值 //用于判断该行是否为斑马线
#define S_BLACK_TIMES  3    //斑马线数量阈值     //用于判断该路段是否为斑马线
//环岛识别
#define C_LOST1 5           //不丢线一边的丢线数阈值
#define C_LOST2 20          //丢线一边的丢线数阈值
//环岛判断flag
extern uint8 Flag_CircleBegin;   //发现环岛
extern uint8 Flag_CircleIn;      //环岛入口

uint8 StartLineFlag(int *LeftLine,int *RightLine);      //起跑线识别
uint8 CircleIsland_Begin(int LeftLine,int RightLine,Point InflectionL,Point InflectionR);
uint8 CircleIsland_Begin(int LeftLine,int RightLine,Point InflectionL,Point InflectionR);
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR,Point *InflectionC);//三岔识别

#endif /* CODE_IMAGESPECIAL_H_ */
