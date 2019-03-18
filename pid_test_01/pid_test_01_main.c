
//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

struct _pid{
    float SetSpeed;         //程序想要达到的速度
    float ActualSpeed;      //实际速度
    float err;              //当前误差
    float err_last;         //上一次pid的误差
    float Kp, Ki, Kd;       //比例系数、积分系数、微分系数
    float voltage;          //驱动电压
    float integral;         //累计误差（积分）
}pid;

float readSpeed(){
    return (pid.voltage)/3-100;
}

float PID_realize(float speed){
    pid.SetSpeed=speed;
    pid.err=pid.SetSpeed-pid.ActualSpeed;
    pid.integral+=pid.err;
    pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);   //此处的意思是指代 动力
    pid.err_last=pid.err;
    pid.ActualSpeed=readSpeed();                                      //读取实际的速度
    return pid.ActualSpeed;
}

void PID_init(){
   // printf("pid_value_init begin\n");
    pid.SetSpeed=0;
    pid.ActualSpeed=0;
    pid.err=0;
    pid.err_last=0;
    pid.voltage=0;
    pid.integral=0;
    pid.Kp=0.2;
    pid.Ki=0.3;
    pid.Kd=-0.2;
   // printf("pid_init completed\n");
}

void init_testGPIO(){
    GPIO_setDirectionMode(104,GPIO_DIR_MODE_OUT);
}

//
// Main
//
void main(void)
{
    int i=0;
    int j=0;
    float speed=0;
    int a[10]={3,4,5,6,7,8,9,10,11,12};

    Device_init();
    Device_initGPIO();
    init_testGPIO();
    PID_init();


    for(j=0;j<10;j++){
        for(i=0;i<128;i++){
            speed=PID_realize(a[j]);
        }

        GPIO_writePin(104,1);
        for(i=0;i<speed;i++){
            DEVICE_DELAY_US(1000);
        }
        GPIO_writePin(104,0);
        DEVICE_DELAY_US(10000);
    }
}

//
// End of File
//
