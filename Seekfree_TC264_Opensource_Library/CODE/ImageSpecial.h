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

#define BLACK_WIDTH  3   //斑马线黑线宽度阈值 //用于判断该线是否为斑马线黑线
#define BLACK_NUM    8   //斑马线黑线数量阈值 //用于判断该行是否为斑马线
#define BLACK_TIMES  3   //斑马线数量阈值     //用于判断该路段是否为斑马线

uint8 StartLineFlag(Point InflectionL,Point InflectionR);    //起跑线识别
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR,Point *InflectionC);//三岔识别

#endif /* CODE_IMAGESPECIAL_H_ */
