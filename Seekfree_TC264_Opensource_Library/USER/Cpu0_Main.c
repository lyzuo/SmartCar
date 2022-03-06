/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC264D
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-3-23
 ********************************************************************************************************************/

//工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
//工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
//然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
//一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

//对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用enableInterrupts();来开启中断嵌套
//简单点说实际上进入中断后TC系列的硬件自动调用了disableInterrupts();来拒绝响应任何的中断，因此需要我们自己手动调用enableInterrupts();来开启中断的响应。

//头文件引用
#include "headfile.h"       //逐飞的封装库
#include "Binarization.h"   //二值化处理
#include "Steer.h"          //舵机控制
#include "Motor.h"          //电机控制
#include "ImageBasic.h"     //图像的基础处理
#include "ImageSpecial.h"   //图像特殊元素处理
#include "ImageTack.h"      //循迹误差计算
#include "PID.h"

#pragma section all "cpu0_dsram"    //将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

//定义变量
int *LeftLine,*CentreLine,*RightLine;   //左中右三线

int core0_main(void)
{
	get_clk();//获取时钟频率  务必保留
	//用户在此处调用各种初始化函数等
	//***************************变量定义**************************
	int LeftLine[MT9V03X_H]={0}, CentreLine[MT9V03X_H]={0}, RightLine[MT9V03X_H]={0};   //扫线处理左中右三线
	Point LeftDownPoint,RightDownPoint;
	LeftDownPoint.X=0;LeftDownPoint.Y=0;RightDownPoint.X=0;RightDownPoint.Y=0;
	Point ForkUpPoint;
	ForkUpPoint.X=0;ForkUpPoint.Y=0;
	float Bias=0;

	int row;

	int16 left_encoder=0,right_encoder=0;
	SteerPID SteerK;
	MotorPID MotorK;
	//*****************************************************************

	//***************************交互的初始化**************************
	uart_init(UART_0, 115200, UART0_TX_P14_0, UART0_RX_P14_1);      //初始化串口0与电脑上位机通讯
	uart_init (BLUETOOTH_CH9141_UART, BLUETOOTH_CH9141_UART_BAUD, BLUETOOTH_CH9141_UART_TX, BLUETOOTH_CH9141_UART_RX);//初始化蓝牙模块所用的串口
	lcd_init();                                                     //初始化TFT屏幕
	gpio_init(P20_8, GPO, 1, PUSHPULL);                             //初始化LED：设置P20_8为输出
	gpio_init(P20_9, GPO, 1, PUSHPULL);
    gpio_init(P21_4, GPO, 1, PUSHPULL);
    gpio_init(P21_5, GPO, 1, PUSHPULL);
    //*****************************************************************

    //**************************传感器模块初始化**************************
	mt9v03x_init(); //初始化摄像头
	//********************************************************************

	//**************************驱动模块初始化**************************
//	gtm_pwm_init(STEER_PIN, 50, STEER_MID);                         //初始化舵机
	gtm_pwm_init(LEFT_MOTOR_PIN1,17*1000,0);                        //初始化左电机
	gtm_pwm_init(LEFT_MOTOR_PIN2,17*1000,0);
	gtm_pwm_init(RIGHT_MOTOR_PIN1,17*1000,0);                       //初始化右电机
	gtm_pwm_init(RIGHT_MOTOR_PIN2,17*1000,0);
	gpt12_init(LEFT_ENCODER, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);    //初始化左编码器
	gpt12_init(RIGHT_ENCODER, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);   //初始化右编码器
	//********************************************************************

	/**********************PID初始化***********************************************/
	PID_init(&SteerK,&MotorK);
	/**************************************************************/

    //等待所有核心初始化完毕
	IfxCpu_emitEvent(&g_cpuSyncEvent);
	IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
	enableInterrupts();

	while (TRUE)
	{
	    /*初始化参数*/


	    //图像处理模块
	    if(mt9v03x_finish_flag)
	    {
	        ImageBinary();//图像二值化
	        //SPI发送图像到1.8TFT
	        lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);    //二值化后的图像
//	        lcd_displayimage032(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);  //原始灰度图像


	        /*扫线函数测试*/
//	        GetImagBasic(LeftLine,CentreLine,RightLine);
//	        for(int i=MT9V03X_H;i>0;i--)    //LCD上的线从下往上画
//	        {
//	            lcd_drawpoint(CentreLine[i],i,RED); //中红
//	            lcd_showint32(0,0,i,3);
//                lcd_showint32(0,1,RightLine[i],3);
//
//                lcd_showint32(0,5,i,3);
//                lcd_showint32(0,6,LeftLine[i],3);
//                systick_delay_ms(STM0, 500);
//	            lcd_drawpoint(LeftLine[i],i,GREEN);  //左
//                RightLine[i]=RightLine[i]*160/188;
//                lcd_drawpoint(RightLine[i],i,BLUE);//右
//	        }
//
//	        /*斜率函数测试*/
//	        Bias=Regression_Slope(80,40,CentreLine);
//	        lcd_showfloat(0,0,Bias,2,3);
//	        systick_delay_ms(STM0, 1000);
//
	        /*左右下拐点函数测试*/
//	        GetDownInflection(110,40,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint);
//	        lcd_showint32(0,3,LeftDownPoint.Y,3);
//            lcd_showint32(0,4,LeftDownPoint.X,3);
//            lcd_showint32(0,6,RightDownPoint.Y,3);
//            lcd_showint32(0,7,RightDownPoint.X,3);
	        //打印左边
//	        lcd_drawpoint(LeftDownPoint.X,LeftDownPoint.Y,GREEN);
//	        //打印右边
//	        lcd_drawpoint(RightDownPoint.X,RightDownPoint.Y,GREEN);
//            systick_delay_ms(STM0, 800);
//
	        /*三岔上拐点函数测试*/
//	        GetForkUpInflection(LeftDownPoint,RightDownPoint,&ForkUpPoint);
//	        lcd_drawpoint(ForkUpPoint.X*160/188,ForkUpPoint.Y,GREEN);

	        /*三岔识别函数测试*/
//	        ForkIdentify(110,40,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint,&ForkUpPoint);
//	        lcd_drawpoint(RightDownPoint.X,RightDownPoint.Y,GREEN);//描点
//	        lcd_drawpoint(LeftDownPoint.X,LeftDownPoint.Y,GREEN);
//	        lcd_drawpoint(ForkUpPoint.X,ForkUpPoint.Y,GREEN);
//	        lcd_showint32(0,3,LeftDownPoint.Y,3);//打印坐标
//	        lcd_showint32(0,0,LeftDownPoint.X,3);
//	        systick_delay_ms(STM0, 2000);
//	        lcd_showint32(0,3,ForkUpPoint.Y,3);
//            lcd_showint32(0,0,ForkUpPoint.X,3);
//            systick_delay_ms(STM0, 2000);
//	        FillingLine(LeftDownPoint,ForkUpPoint);//补线画线
//	        lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);
//	        systick_delay_ms(STM0, 800);
//	        FillingLine(RightDownPoint,ForkUpPoint);
//            lcd_displayimage032(BinaryImage[0],MT9V03X_W,MT9V03X_H);
//            systick_delay_ms(STM0, 800);

//
            /*进入环岛前的判断测试*/
//            CircleIslandBegin(LeftLine,RightLine,LeftDownPoint,RightDownPoint);    //调用检测环岛入口的函数


	        gpio_toggle(P20_8);//翻转IO：LED
            mt9v03x_finish_flag = 0;//在图像使用完毕后务必清除标志位，否则不会开始采集下一幅图像
	    }

	    /*电机驱动测试*/
//	    SteerCtrl(STEER_MID);
//	    MotorCtrl(1000,1000);

	    /*编码器测试*/
	    MotorEncoder(&left_encoder,&right_encoder);
	    Speed_PI_Left(left_encoder,1000,MotorK);
	    Speed_PI_Right(right_encoder,1000,MotorK);
	    printf("left_encoder=%d,right_encoder=%d",left_encoder,right_encoder);
	}
}

#pragma section all restore


