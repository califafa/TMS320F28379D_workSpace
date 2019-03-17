//Debug with GPA.

//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#define BURST 1         //transmit BURST words(16bits) everytime.
#define TRANSFER 128     //transmit TRANSFER times.

//uint16_t is unsigned-int

//#define rData (&(GpioDataRegs.GPADAT.all))


uint16_t sData[128];
uint16_t rData[128];


uint16_t done=0;

#pragma DATA_SECTION(sData,"ramgs0");   // map the TX data to memory
#pragma DATA_SECTION(rData,"ramgs1");   // map the RX data to memory

void initDMA(){
    DMA_initController();
    const void * srcAddr;
    const void * destAddr;
    srcAddr=(const void *)sData;
    destAddr=(const void *)rData;

    DMA_configAddresses(DMA_CH6_BASE,destAddr,srcAddr);
    DMA_configBurst(DMA_CH6_BASE,BURST,1,1);
    DMA_configTransfer(DMA_CH6_BASE,TRANSFER,1,1);
    //DMA_configMode(DMA_CH6_BASE,DMA_TRIGGER_SOFTWARE,DMA_CFG_CONTINUOUS_ENABLE);  //loop
    //DMA_configMode(DMA_CH6_BASE,DMA_TRIGGER_SOFTWARE,DMA_CFG_ONESHOT_DISABLE);      //once , and one trigger one BURST transmitted.
    DMA_configMode(DMA_CH6_BASE,DMA_TRIGGER_SOFTWARE,DMA_CFG_ONESHOT_ENABLE);       //only need a trigger once.
    DMA_setInterruptMode(DMA_CH6_BASE,DMA_INT_AT_END);
    DMA_enableTrigger(DMA_CH6_BASE);
    DMA_enableInterrupt(DMA_CH6_BASE);
}

void test_gpioInit(){
    EALLOW;
    GpioCtrlRegs.GPADIR.all=0xffffffff;
    EDIS;
}

interrupt void dmaIntISR(){
    DMA_stopChannel(DMA_CH6_BASE);
    EALLOW;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7);
    EDIS;
    done=1;
}

//
// Main
//
void main(void)
{

    Device_init();
    int i=0;
    test_gpioInit();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Interrupt_register(INT_DMA_CH6,&dmaIntISR);
    Interrupt_enable(INT_DMA_CH6);
    EINT;
    initDMA();
    SysCtl_selectSecMaster(0,SYSCTL_SEC_MASTER_DMA);
    DMA_startChannel(DMA_CH6_BASE);
    done=0;

    for(i=0;i<128;i++){
        sData[i]=i;
        rData[i]=0;
    }
    EALLOW;

    for(i=0;!done;i++){
        DMA_forceTrigger(DMA_CH6_BASE);
        DEVICE_DELAY_US(1000);
        GpioDataRegs.GPADAT.all=rData[i%128];
    }
    done=0;
    for(i=0;i<128;i++){
        GpioDataRegs.GPADAT.all=rData[i%128];
    }
    EDIS;

}

//
// End of File
//
