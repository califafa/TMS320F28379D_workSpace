
//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#include<math.h>

#define log2N 5     //log2(len)
#define len 32      //input & output width

#define Pi 3.141592653589793

typedef struct{
    double real;
    double img;
}C;

C input[len];

C output[len];


C add(C a, C b){
    C c;
    c.real=a.real+b.real;
    c.img=a.img+b.img;
    return c;
}

C sub(C a,C b){
    C c;
    c.real=a.real-b.real;
    c.img=a.img-b.img;
    return c;
}

C mul(C a,C b){
    C c;
    c.real=a.real*b.real-a.img*b.img;
    c.img=a.real*b.img+a.img*b.real;
    return c;
}

void Reverse(){                                                                                       //��ת����
    unsigned int i;
    unsigned char bit;
    for(i=0;i<len;i++){
        unsigned int I=i;
        unsigned int R=0;
        for(bit=0;bit<log2N;bit++){
            R<<=1;
            R|=(I&1);
            I>>=1;
        }
        if(R>i){
            C tmp=input[R];
            input[R]=input[i];
            input[i]=tmp;
        }
    }
}

void ButterflyAlgorithm(){                                                                             //�������㣬��log2N����ÿ����len/(2*i)�飬ÿ����2*i����
    unsigned int i;
    unsigned int j;
    unsigned int k;

    for(i=1;i<=len;i<<=1){                                                                //��log2N��
        for(j=0;j<(len/(2*i));j++){                                                       //ÿ����len/(2*i)��
            for(k=0;k<i;k++){                                                             //ÿ����2*i����ÿ���Ϊ2���֡���һ����Ϊǰi�����ڶ�����Ϊ��i����
                double tmpReal=cos((Pi/i)*k);
                double tmpImg=-1*sin((Pi/i)*k);
                C tmpWn;                                                                               //��ת�ӡ���ÿ�����棬ʵ��Ϊ0-Pi��cos���鲿Ϊ0-Pi��-1*sin����0-Pi��Ϊi���㡣
                tmpWn.real=tmpReal;
                tmpWn.img=tmpImg;

                C tmpXw=mul(input[j*2*i+k+i],tmpWn);

                C tmpTop=add(input[j*2*i+k],tmpXw);

                C tmpBottom=sub(input[j*2*i+k],tmpXw);

                input[j*2*i+k]=tmpTop;
                input[j*2*i+k+i]=tmpBottom;

            }
        }
    }
}

void fft(){                                                                                            //���ٸ���Ҷ�任
    unsigned int i;
    Reverse();
    ButterflyAlgorithm();
    for(i=0;i<len;i++){
        output[i].real=(input[i].real*input[i].real+input[i].img*input[i].img);
    }
}

void test_gpioInit(){
    GPIO_setDirectionMode(104,GPIO_DIR_MODE_OUT);
}

//
// Main
//
void main(void)
{
    Device_init();
    Device_initGPIO();
    double period_num_in_input;
    int i=0;
    int j=0;
    double max=0;
    int maxIndex=0;
    double arr[]={8,9,10,11,12,13,14,15};   //in 32points , num 13 have something wrong.
    test_gpioInit();
    for(j=0;j<8;j++){
        max=0;
        maxIndex=0;
        period_num_in_input=arr[j];
        double freq=period_num_in_input *2*Pi/len;
        for(i=0;i<len;i++){
            input[i].real=cos(i*freq);
            input[i].img=0;
        }
         fft();
         for(i=0;i<len;i++){
            if(output[i].real>max){
                max=output[i].real;
                maxIndex=i;
            }
        }
        GPIO_writePin(104,1);
        for(i=0;i<maxIndex;i++){
            DEVICE_DELAY_US(10);
        }
        GPIO_writePin(104,0);
        DEVICE_DELAY_US(10000);
    }
}

//
// End of File
//
