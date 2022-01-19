/*
 * ImageSpecial.c
  *  赛道上各种特殊元素的识别
  *  该文件的函数只实现识别是否有该元素的出现以及出现的位置，至于该元素出现了多少次以及对应措施，不在该文件范围内
 *  Created on: 2022年1月17日
 *      Author: yue
 */

#include "ImageSpecial.h"

/*
 *******************************************************************************************
 ** 函数功能: 识别起跑线
 ** 参    数: InflectionL：左拐点
 **           InflectionR：右拐点
 ** 返 回 值: 0：没有识别到起跑线
 **           1：识别到起跑线且车库在车左侧
 **           2：识别到起跑线且车库在车右侧
 ** 作    者: WBN
 ** 注    意：1 . 默认在进行起跑线识别的路段都是直线，即车头已经摆正
 **           2.由于没有实物图做参考，只能先假设一个理想状态：整条起跑线恰好布满整个图像
 ********************************************************************************************
 */
uint8 StartLineFlag(Point InflectionL,Point InflectionR)
{
    /*
     ** 有起跑线的地方就有车库，由于赛道原因，车库有可能出现在车的左边或右边，这里也先对这两种情况进行判断并分别处理；
     ** 以车库在左边为例，从拐点开始作为扫线的原点，从下往上，从左往右扫；
     ** 固定一列，从下往上扫，找到一个黑点，固定该行，从左往右扫，记录连续出现的黑点个数（黑线的宽度），若该宽度大于阈值BLACK_WIDTH，可
     ** 以确定这是一条黑线。继续扫这一列，若发现了足够多的这样的黑线（大于设定的阈值BLACK_NUM），则认为这一行符合斑马线。按照该理论回
     ** 到起点继续行的扫描，若像这样的行数足够多（大于设定的阈值BLACK_TIMES），则认为这一幅图片中存在斑马线，即该路段是起跑线
     **/

    //在这里加入一个判断：检测到左方或右方有拐点时，再进行是否有起跑线的判断
    //并且扫线的位置根据拐点决定。从左下拐点开始：下-上，左-右扫线；从右下拐点开始：下-上，右-左扫线


    int row,cloum;          //行,列
    int Black_width=0;      //固定行，横向扫线是记录每段黑点的个数（即一条黑线的宽度）
    int Black_num=0;        //记录行黑线的数量，作为判断该行是否为斑马线的依据
    int Black_times=0;      //记录满足斑马线的行数，并作为判断该路段是否为斑马线的依据

    if(InflectionL.X!=0&&InflectionL.Y!=0)    //拐点（车库）在左边
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //从左拐点开始固定列，从下往上扫
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //找到了一个黑点
            {
                for(;cloum<MT9V03X_W;cloum++)                                 //固定行，从左往右扫
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //扫到黑点
                    {
                        Black_width++;   //黑线宽度+1
                    }
                    else                                        //扫到白点
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //判断黑线宽度是否满足阈值
                        {
                            Black_num++; //行黑线数量+1
                        }
                        Black_width=0;   //在一次白点判断后重置黑线宽度
                    }
                    if(Black_num>=S_BLACK_NUM)    //若满足斑马线的阈值（这一行）
                    {
                        Black_times++;  //满足斑马线的行数+1
                        break;
                    }
                }
                Black_num=0;    //这一行的扫线结束，重置行黑线数
            }
            if(Black_times>=S_BLACK_TIMES)    //若满足斑马线路段的阈值
            {
                return 1;
            }
        }
        return 0;
    }

    if(InflectionR.X!=0&&InflectionR.Y!=0)    //拐点（车库）在右边
    {
        for(row=InflectionL.X,cloum=InflectionL.Y;row<MT9V03X_H;row++)        //从右拐点开始固定列，从下往上扫
        {
            if(BinaryImage[row][cloum]==IMAGE_BLACK)    //找到了一个黑点
            {
                for(;cloum>0;cloum--)                                         //固定行，从右往左扫
                {
                    if(BinaryImage[row][cloum]==IMAGE_BLACK)    //扫到黑点
                    {
                        Black_width++;   //黑线宽度+1
                    }
                    else                                        //扫到白点
                    {
                        if(Black_width>=S_BLACK_WIDTH)    //判断黑线宽度是否满足阈值
                        {
                            Black_num++; //行黑线数量+1
                        }
                        Black_width=0;   //在一次白点判断后重置黑线宽度
                    }
                    if(Black_num>=S_BLACK_NUM)    //若满足斑马线的阈值（这一行）
                    {
                        Black_times++;  //满足斑马线的行数+1
                        break;
                    }
                }
                Black_num=0;    //这一行的扫线结束，重置行黑线数
            }
            if(Black_times>=S_BLACK_TIMES)    //若满足斑马线路段的阈值
            {
                return 2;
            }
        }
        return 0;
    }

    return 0;
}
/*
 *******************************************************************************************
 ** 函数功能: 识别环岛
 ** 参    数: LeftLine：左线数组
 **           RightLine：右线数组
 **           InflectionL：左拐点
 **           InflectionR：右拐点
 ** 返 回 值: 0：没有识别到环岛
 **           1：识别到环岛且在车身左侧
 **           2：识别到环岛且在车身右侧
 **           3：识别到环岛入口且在车身左侧
 **           4：识别到环岛入口且在车身右侧
 ** 作    者: WBN
 ** 注    意：1 . 默认在进行环岛识别的路段都是直线，即车头已经摆正
 **           2.这个应该是进环岛的函数，出环岛那段应该重写一个，并在进入环岛后再开启出环岛的判断
 ********************************************************************************************
 */
uint8 RoundaboutFlag(int LeftLine,int RightLine,Point InflectionL,Point InflectionR)
{
    /**
     ** 整个赛道会有3个环岛，一个在起跑线左侧，一个在起跑线右侧，由于是循环赛道所以会存在环岛在车左边和在车右边两种情况；
     ** 以左环岛为例，若车还没有进入环岛入口，则图像会有两种情况：一种是图像中有拐点（即完全在环岛外，这种情况可能会出现直角拐弯的误判，先
     ** 不做处理）；另一种是图像中没有拐点（既已经行进到环岛一半但还未到达环岛入口）。若车已经到达环岛入口，则只会有一种情况
     ** */

    /*
     ** 方法一：环岛的识别分为发现环岛以及到达环岛入口两个阶段，该函数会在发现环岛后卡主，直到到达环岛入口再返回识别到环岛；若没发环岛
     **         就直接退出（这样有一个问题就是在发现环岛入口到发现环岛入口这段时间内，车会以原有的角度与速度行驶）
     ** 方法二：环岛的识别任分发现环岛和到达环岛入口两个部分，函数在发现环岛和到达环岛入口作为两个不同的返回值（加上左右共四种情况即四
     **         个返回值），这样将车子的处理问题抛出，该方法可能更稳定也符合一开始的想法
     ** */

    /*
     ** 1.识别到环岛：寻找这两种情况的共通点：一边出现丢线，另一边完整，且丢线的一边必定会再次出现线而非全丢
     ** 1.1 另一种思路：像扫左右边线一样，扫上下边线。。。都是圆好像区别不大？不过下边线可以扫出一条完整的带弧度的线，而左右扫线则可能因为
     **                 摄像头拍摄不全的缘故扫不到最边缘的线
     ** 2.识别到入口：一边出现丢线，另一边完整，且丢线的一边必定只是丢了一个缺口而不会有其他情况
     ** */

    int row,cloum;                      //行,列
    int white_num=0,black_num=0;        //记录扫线过程的白点数量
    int times=0;                    //记录符合环岛特征的列数
    //发现环岛：存在拐点
    if(InflectionL.X!=0&&InflectionL.Y!=0)   //拐点在左边（发现环岛在车身左侧）
    {
        for(row=InflectionL.X,cloum=InflectionL.Y-POINT_MOVE;cloum>0;cloum--)        //外层：从拐点往左扫线
        {
            for(;row<MT9V03X_H;row++)                                                //内层：循环是从下往上
            {
                if(BinaryImage[row][cloum]==IMAGE_WHITE)    //扫到白点
                {
                    white_num++;    //白点数量+1
                }
                else                                        //扫到黑点
                {
                    black_num++;    //黑点数量+1
                }
                if(white_num>=R_WHITE_NUM&&black_num>=R_BLACK_NUM)
                {
                    times++;
                    break;
                }
            }
            if(times>=R_TIMES)
            {
                return 1;
            }
            white_num=0;
            black_num=0;
        }
    }

    //到达环岛入口：无拐点

    return 0;
}

/********************************************************************************************
 ** 函数功能: 识别三岔
 ** 参    数: int startline:用户决定的起始行
 **           int endline:用户决定的结束行（表示对前几段的识别，根据速度不同进行调整）
 **           int *LeftLine：左线
 **           int *RightLine:右线
 **           Point *InflectionL:左边拐点
 **           Point *InflectionR:右边拐点
 **           Point *InflectionC:中间拐点
 ** 返 回 值: 0：没有识别到环岛
 **           1：识别到三岔
 ** 作    者: LJF
 ** 注    意：1 . 目前仅仅是正入三岔的时候的函数，因为三岔前面都会有个弯道所以会出现车身斜的情况，此时的左右拐点并不一定都存在
 **           2.这个是进三岔的函数，出三岔时候应该重写一个，并在进入三岔后再开启出三岔的判断
 *********************************************************************************************/
uint8 ForkIdentify(int startline,int endline,int *LeftLine,int *RightLine,Point *InflectionL,Point *InflectionR,Point *InflectionC)
{
    GetDownInflection(startline, endline, LeftLine, RightLine, InflectionL, InflectionR);//获取左右拐点
    if(InflectionL->X!=0&&InflectionR->X!=0)//当左右拐点存在
    {
        GetForkUpInflection(*InflectionL, *InflectionR, InflectionC);//去搜索上拐点
        if(InflectionC->X!=0)
        {
            //关于三岔的上拐点还会有其他的条件判断，比如不超过多少行什么的，这个要具体才能判断了
            //三岔成立的条件太简单了会存在误判，比如从十字出来的时候就可能会遇到同样的具有三个点
            //这里我想采用元素互斥的原则,给一个非十字的标志
            return 1;//三个拐点存在三岔成立
            /*还需要写根据左右选择标识符来选择往左还是右边从而进行补线*/
        }
    }
    return 0;
}
