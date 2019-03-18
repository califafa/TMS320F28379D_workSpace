//this test need link GPIO0 and GPIO1



//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

//
// Main
//
void main(void)
{
    Device_init();
    Device_initGPIO();

    uint32_t readValue=0;
    int j=0;

    EALLOW;
    GpioCtrlRegs.GPADIR.all=0xffffffff; //32bits,output
    GpioCtrlRegs.GPADIR.bit.GPIO1=0;    //input
    EDIS;

    while(1){

        GpioDataRegs.GPADAT.bit.GPIO0=!(GpioDataRegs.GPADAT.bit.GPIO0);

        readValue=GpioDataRegs.GPADAT.bit.GPIO1;

        GpioDataRegs.GPADAT.bit.GPIO2=1;
        for(j=0;j<readValue;j++){
            DEVICE_DELAY_US(100000);    //100ms
        }
        GpioDataRegs.GPADAT.bit.GPIO2=0;

    }

}

//
// End of File
//
