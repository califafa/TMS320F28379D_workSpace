//this test need link GPIO104 and GPIO105


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

    GPIO_setDirectionMode(104,GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(105,GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(22,GPIO_DIR_MODE_OUT);
    GPIO_writePin(104,0);
    DEVICE_DELAY_US(1000);
    GPIO_writePin(104,1);

    while(1){

        GPIO_togglePin(104);

        readValue=GPIO_readPin(105);    //return 0 or 1

        GPIO_writePin(22,1);
        for(j=0;j<readValue;j++){
            DEVICE_DELAY_US(100000);    //100ms
        }
        GPIO_writePin(22,0);


    }

}

//
// End of File
//
