/*
 * Forkroads.c
 * Created on: 2022��5��25��
 * Author: 30516
 * Effect: �������·����ص�Դ����
 */

#include "ImageSpecial.h"
#include "PID.h"
#include <stdlib.h> //abs������fabs��math.h

/*********************************************************************************
 ** ��������: ���������¹յ���Ѱ�������Ϲյ�
 ** ��    ��: Point InflectionL: ��߹յ�
 **           Point InflectionR: �ұ߹յ�
 **           Point *UpInflectionC: �м��Ϲյ�
 ** �� �� ֵ: ��
 ** ˵    ��: ��������������޸ģ�д�����ҹյ����棬���߻�Ҫ��һ��Ԫ�����͵Ĳ����������������Ҳ�ͬ�Ĺյ㣬����ֻ�������
 ** ��    ��: LJF
 **********************************************************************************/
void GetForkUpInflection(Point DownInflectionL,Point DownInflectionR,Point *UpInflectionC)
{
    int starline,i,cloumnL,cloumnR;
    UpInflectionC->X=0;UpInflectionC->Y=0;//�Ϲյ�����
    UpInflectionC->X=(DownInflectionL.X+DownInflectionR.X)/2;//V���Ϲյ��������Ϊ���ҹյ��ֵ����Ҫ�޸ģ���һ������������
    starline=(DownInflectionL.Y+DownInflectionR.Y)/2;//��ʼ��Ϊ���ҹյ��еľ�ֵ
    //���������ҵ��Ǹ�����ĵ㼴Ϊ�Ϲյ�
    for(i=starline;i>1;i--)
    {
        //ͼ��������[��][��]
        if(BinaryImage[i][UpInflectionC->X]==IMAGE_WHITE && BinaryImage[i-1][UpInflectionC->X]==IMAGE_BLACK)
        {
            for(cloumnL=UpInflectionC->X;cloumnL>10;cloumnL--)
            {
                if(BinaryImage[i-1][cloumnL]==IMAGE_WHITE)
                    break;
                if(cloumnL==11)//�����ʼ���о�С����11����ô�򲻻�return����ֱ�ӵ�����ĸ�ֵ
                    return;//�������˶�û���ҵ��׵ļ������������˳��ж�
            }
            for(cloumnR=UpInflectionC->X;cloumnR<MT9V03X_W-10;cloumnR++)
            {
                if(BinaryImage[i-1][cloumnR]==IMAGE_WHITE)
                    break;
                if(cloumnR==MT9V03X_W-11)
                    return;//�������˶�û���ҵ��׵ļ������������˳��ж�
            }
            UpInflectionC->Y=i;//Y����������
            return;
        }
    }
}

/********************************************************************************************
 ** ��������: ʶ������
 ** ��    ��: int startline:�û���������ʼ��
 **           int endline:�û������Ľ����У���ʾ��ǰ���ε�ʶ�𣬸����ٶȲ�ͬ���е�����
 **           int *LeftLine������
 **           int *RightLine:����
 **           Point *InflectionL:��߹յ�
 **           Point *InflectionR:�ұ߹յ�
 **           Point *InflectionC:�м�յ�
 ** �� �� ֵ:  0��û��ʶ�𵽻���
 **           1����������
 ** ��    ��: LJF
 ** ע    �⣺1 . Ŀǰ����������������ʱ��ĺ�������Ϊ����ǰ�涼���и�������Ի���ֳ���б���������ʱ�����ҹյ㲢��һ��������
 **           2.����ǽ������ĺ�����������ʱ��Ӧ����дһ�������ڽ����������ٿ������������ж�
 *********************************************************************************************/
uint8 ForkIdentify(int *LeftLine,int *RightLine,Point DownInflectionL,Point DownInflectionR)
{
    Point UpInflectionC;
    if(DownInflectionL.X!=0 && DownInflectionL.Y>60 && DownInflectionR.X!=0 && DownInflectionR.Y>60)//�����ҹյ����,�����ҹյ㲻��̫����ֶ������
    {
          //ȡ��������ҹյ��������жϣ�������������
        if(abs((DownInflectionL.Y-DownInflectionR.Y))<40)//���������յ������С��30���Ž����ж�
        {
            GetForkUpInflection(DownInflectionL, DownInflectionR, &UpInflectionC);//ȥ�����Ϲյ�
            if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
            {
                FillingLine('R',DownInflectionR,UpInflectionC);//���������˾��ڷ���֮ǰ����
                Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//��Ϊ������������������һ�ξ��룬���´��̫�࣬���Ի��ǰ���ԭ���ķ���
                return 1;//�����յ����������������������
            }
        }
        else
            return 0;
    }
    else if(DownInflectionL.X==0 && DownInflectionR.X==0)//��������¹յ㲻���ڲ�������һ�γ��־Ͷ��ߵĻ��Ļ�,���Ǿ�ȥ���治�������ϵĹյ�
    {
        Point ImageDownPointL,ImageDownPointR;//�Ի�������½Ǻ����½���Ϊ���Ҳ��ߵĵ�
        ImageDownPointL.X=0,ImageDownPointL.Y=MT9V03X_H,ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=MT9V03X_H;
        GetForkUpInflection(ImageDownPointL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0 && UpInflectionC.Y>40)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//���������˾��ڷ���֮ǰ����
            //�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            //��һ����Խ���������ܽӽ���������Ĺսǣ������ֶ��Ѳ������߼��������bias����
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine)*1.5;
            return 1;//�������붪ʧ���ҹյ���һ֡
        }
    }
    //��յ�x[0,70),������������̫�ұߣ����⼫�����Ӱ����
    else if(LostNum_RightLine>=60 && DownInflectionL.X!=0)
    {
        Point ImageDownPointR;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        ImageDownPointR.X=MT9V03X_W-1,ImageDownPointR.Y=DownInflectionL.Y;
        GetForkUpInflection(DownInflectionL, ImageDownPointR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',ImageDownPointR,UpInflectionC);//���������˾��ڷ���֮ǰ����
            Bias=DifferentBias(ImageDownPointR.Y,UpInflectionC.Y,CentreLine);//�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            return 1;//������б������
        }
    }
    //�ҹյ�x[0,70)
    else if(LostNum_LeftLine>=60 && DownInflectionR.X!=0)
    {
        Point ImageDownPointL;//����յ�ԳƵĵ�ȥ���ߺ��ҹյ�
        ImageDownPointL.X=5,ImageDownPointL.Y=DownInflectionR.Y;
        GetForkUpInflection(ImageDownPointL, DownInflectionR, &UpInflectionC);
        if(UpInflectionC.Y!=0)//ֱ�ӷ���Y���ɣ��ӿ��ٶȣ���ΪXĬ�Ͼͻḳֵ��
        {
            FillingLine('R',DownInflectionR,UpInflectionC);//���������˾��ڷ���֮ǰ����
            Bias=DifferentBias(DownInflectionR.Y,UpInflectionC.Y,CentreLine);//�ڴ˴��Ͷ�ƫ����м��㣬�Ϳ��Ա������һ�������߱����ߵ������⣬ͬʱ�ⲿʹ��һ����־����ʶ����֮����һ���򲻽��������Զ����ǰհƫ�����
            return 1;//������б������
        }
    }
    return 0;
}

/********************************************************************************************
 ** ��������: ����״̬��ת�жϺ���
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0��������δ����
 **           1�������ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 ForkFStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 NowFlag)
{
    static uint8 StatusChange,num1,num3,numspecial;//����ʶ��������ʳ״̬������������״̬�Ƿ���ת

    if(numspecial<200)//��ֹ�ܾö�û�г��ֽ�����ڵ�״̬����ʱȥ�жϳ���
    {
        numspecial++;
    }
    else if(StatusChange<1)//�ж�״̬��û�жȹ����״̬����û����ǿ������
    {
        StatusChange=2;
    }

    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
            if(NowFlag==1)
            {
                StatusChange=1;//ֻҪ��ʼʶ����������˵���Ѿ�����ڽ׶���
            }
            break;
        }
        //��;״̬
        case 1:
        {
            if(num1<120)  //���㹻����ʱ���ó��ߵ�����������
            {
                num1++;
                break;
            }
            else if(NowFlag==0)
            {
                StatusChange=2;//�����м����̬֮����ת��������
            }
            break;
        }
        //����״̬
        case 2:
        {
            if(NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        //ȷ���Ѿ��������ˣ����������ھͳ������ˣ�ʹ�ó�������ʵ��ɨ�߳���
        case 3:
        {
            if(num3<35)  //���㹻����ʱ���ó��߳�������
            {
                num3++;
                break;
            }
            else
            {
                return 1;
            }
        }
        default:break;
    }
    return 0;
}

/********************************************************************************************
 ** ��������: ����״̬��ת�жϺ���
 ** ��    ��: Point InflectionL�����¹յ�
 **           Point InflectionR�����¹յ�
 ** �� �� ֵ:  0��������δ����
 **           1�������ѽ���
 ** ��    ��: LJF
 *********************************************************************************************/
uint8 ForkSStatusIdentify(Point DownInflectionL,Point DownInflectionR,uint8 NowFlag)
{
    static uint8 StatusChange,num1,num3,numspecial;//����ʶ��������ʳ״̬������������״̬�Ƿ���ת

    if(numspecial<255)//��ֹ�ܾö�û�г��ֽ�����ڵ�״̬����ʱȥ�жϳ���
    {
        numspecial++;
    }
    else if(StatusChange<1)//�ж�״̬��û�жȹ����״̬����û����ǿ������
    {
        StatusChange=2;
    }

    switch(StatusChange)
    {
        //���״̬
        case 0:
        {
            if(NowFlag==1)
            {
                SteerK.P=18.25;//��СKP��������
                StatusChange=1; //ֻҪ��ʼʶ����������˵���Ѿ�����ڽ׶���
            }
            break;
        }
        //��;״̬
        case 1:
        {
            if(num1<100)  //���㹻����ʱ���ó��ߵ�����������
            {
                if(num1==50)
                {
                    SteerK.D=3;//����ֱ��֮��D��С��ֹ��
                    base_speed+=15; //�����������٣�ȷ������������������Żᴥ��
                }
                num1++;
                break;
            }
            if(NowFlag==0)
            {
                StatusChange=2; //�����м����̬֮����ת��������
                base_speed-=5; //�����ڼ���
            }
            break;
        }
        //����״̬
        case 2:
        {
            if(NowFlag==1)
            {
                StatusChange=3;
            }
            break;
        }
        //ȷ���Ѿ��������ˣ����������ھͳ������ˣ�ʹ�ó�������ʵ��ɨ�߳���
        case 3:
        {
            if(num3<50)
            {
                num3++;
                break;
            }
            else
            {
                SteerK.P=19.25;//��ԭKP
                return 1;
            }
        }
        default:break;
    }
    return 0;
}
