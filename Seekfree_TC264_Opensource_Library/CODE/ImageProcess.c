/*
 * ImageProcess.c
 *
 * Created on: 2022年3月29日
 * Author: 30516
 * Effect: Image element processing logic
 */
#include "ImageProcess.h"
#include "zf_gpio.h"
#include "PID.h"
#include "Motor.h"

uint8 bias_startline=95,bias_endline=50;        //动态前瞻
uint8 Fork_flag=0;              //三岔识别的标志变量
uint8 Garage_flag=0;            //车库识别标志变量
uint8 speed_case_1=200,speed_case_2=170,speed_case_3=155,speed_case_4=165,speed_case_5=165,speed_case_6=160,speed_case_7=170;

uint32 SobelResult=0;

void Stop(void)
{
    while(1)
    {
        base_speed=0;
        diff_speed_kp=0;
    }
}

/********************************************************************************************
 ** 函数功能: 对图像的各个元素之间的逻辑处理函数，最终目的是为了得出Bias给中断去控制
 ** 参    数: 无
 ** 返 回 值: 无
 ** 作    者: LJF
 ** 注    意：无
 *********************************************************************************************/
void ImageProcess()
{
    /***************************变量定义****************************/
    static uint8 flag;
    static uint8 case_5,case_0,case_2,case_1,case_4,case_6,case_3;
    Point LeftDownPoint,RightDownPoint;     //左右下拐点
    LeftDownPoint.X=0;LeftDownPoint.Y=0;RightDownPoint.X=0;RightDownPoint.Y=0;
    Point ForkUpPoint;
    ForkUpPoint.X=0;ForkUpPoint.Y=0;
    Point CrossRoadUpLPoint,CrossRoadUpRPoint;
    CrossRoadUpLPoint.X=0;CrossRoadUpLPoint.Y=0;CrossRoadUpRPoint.X=0;CrossRoadUpRPoint.Y=0;
    /*****************************扫线*****************************/
    GetImagBasic(LeftLine,CentreLine,RightLine);
    /*************************搜寻左右下拐点***********************/
    GetDownInflection(110,45,LeftLine,RightLine,&LeftDownPoint,&RightDownPoint);
    /*************************特殊元素判断*************************/
//    CrossLoopEnd_S();
    /****************************状态机***************************/
#if 1
    switch(flag)
    {
        case 0: //识别左环岛
        {
            flag=4; //调试用，跳转到指定状态
            if(case_0<165)  //出库后延时一会再开启下一个元素的识别，防止误判，对应速度180
            {
                case_0++;
                break;
            }
            gpio_set(LED_WHITE, 0);
            if(CircleIslandIdentify_L(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
            {
                gpio_set(LED_WHITE, 1);
                flag=1;         //跳转到状态1
            }
            break;
        }
        case 1: //识别第一个十字回环
        {
            if(case_1<90)   //延时一会再进入十字判断
            {
                CircleIslandOverBegin_L(LeftLine, RightLine);   //防止左环岛过早出状态再次拐入环岛
                if(case_1>10)   //延时加速上坡，给车子调整姿态的时间
                {
                    base_speed=speed_case_1;
                }
                case_1++;
                break;
            }
            gpio_set(LED_GREEN, 0);
            if(CrossLoopEnd_F()==1)
            {
                gpio_set(LED_GREEN, 1);
                base_speed=speed_case_2; //提速上坡进行右环岛
                bias_startline=95;       //出环恢复动态前瞻
                diff_speed_kp-=0.02;     //恢复差速
                flag=2;         //跳转到状态2
            }
            else
            {
                if(CrossLoopBegin_F(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
                {
                    if(case_1==30)  //只进行一次
                    {
                        case_1++;
                        base_speed=150; //分段减速
                    }
                }
                if(CircleIsFlag_3_L()==1)
                {
                    base_speed=140;         //入环降速，为出环做准备
                    diff_speed_kp+=0.02;    //提高一点点差速
                    bias_startline=100;     //入环调整动态前瞻
                }
            }
            break;
        }
        case 2: //识别右环岛
        {
            if(case_2<100)   //延时开启识别
            {
                case_2++;
                break;
            }
            gpio_set(LED_BLUE, 0);
            if(CircleIslandIdentify_R(LeftLine, RightLine, LeftDownPoint, RightDownPoint)==1)
            {
                gpio_set(LED_BLUE, 1);
                flag=3;          //跳转到状态3
            }
            break;
        }
        case 3: //识别左车库
        {
            if(case_3<160)//帧率从50变成100，数的帧数也要翻倍，这里是大S
            {
                case_3++;
                break;
            }
            base_speed=speed_case_3;  //减速进入左车库
            gpio_set(LED_RED, 0);
            if(LostNum_LeftLine>40 && LostNum_RightLine<30)
            {
                Garage_flag=GarageIdentify('L', LeftDownPoint, RightDownPoint);//识别车库
            }
            if(GarageLStatusIdentify(LeftDownPoint, RightDownPoint,Garage_flag)==1)
            {
                gpio_set(LED_RED, 1);
                flag=4;          //跳转到状态4
            }
            break;
        }
        case 4: //识别三岔
        {
            if(case_4<50)    //延迟防止误判
            {
                case_4++;
                break;
            }
            base_speed=speed_case_4;  //提速进入三岔
            gpio_set(LED_YELLOW, 0);
            Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);   //获取三岔状态
            if(ForkFStatusIdentify(LeftDownPoint, RightDownPoint,Fork_flag)==1)
            {
                gpio_set(LED_YELLOW, 1);
                base_speed=speed_case_5; //提速进入第二个十字回环
                flag=5;         //跳转到状态5
            }
            break;
        }
        case 5: //识别第二个十字回环
        {
            if(case_5<110)  //结束三岔后延时一会再开启下一个元素的识别，防止误判
            {
                case_5++;
                break;
            }
            if(case_5==110)
            {
                case_5++;
            }
            gpio_set(P21_4, 0);
            if(CrossLoopEnd_S()==1)
            {
                gpio_set(P21_4, 1);
                base_speed=speed_case_6; //提速进入三岔
                bias_startline=95;       //出环恢复动态前瞻
                flag=6;         //跳转到状态6
            }
            else
            {
               CrossLoopBegin_S(LeftLine, RightLine, LeftDownPoint, RightDownPoint);
               if(CrossLoopIn_S()==1)
               {
                   base_speed=160;     //入环降速，为出环做准备
                   bias_startline=100; //入环调整动态前瞻
               }
            }
            break;
        }
        case 6: //识别第二遍三岔
        {
            if(case_6<90)  //结束十字回环后延时一会再开启下一个元素的识别，防止S弯误判成三岔入口
            {
                case_6++;
                break;
            }
            gpio_set(P21_5, 0);
            Fork_flag=ForkIdentify(LeftLine, RightLine, LeftDownPoint, RightDownPoint);   //获取三岔状态
            if(ForkSStatusIdentify(LeftDownPoint, RightDownPoint,Fork_flag)==1)
            {
                gpio_set(P21_5, 1);
                base_speed=speed_case_7; //降速准备入库
                flag=7;         //跳转到状态7
            }
            break;
        }
        case 7: //识别右车库，入库
        {
            gpio_set(P20_9, 0);
            Garage_flag=GarageIdentify('R', LeftDownPoint, RightDownPoint);//识别车库
            break;
        }
    }
#endif
    /***************************偏差计算**************************/
    if(Fork_flag!=0 || Garage_flag!=0)  //在识别函数里面已经计算了Bias
    {
        Garage_flag=0;Fork_flag=0;
        return;
    }
    else
    {
        Bias=DifferentBias(bias_startline,bias_endline,CentreLine);//无特殊处理时的偏差计算
//        lcd_showfloat(0, 7, Bias, 2, 3);
    }
}
