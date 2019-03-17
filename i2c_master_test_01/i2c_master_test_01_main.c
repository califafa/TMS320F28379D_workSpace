//this test use ds3231


//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

#define SLAVE_ADDRESS 0xd0>>1

unsigned char sData=0x00;
unsigned char rData=0;


interrupt void i2cISR(){
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);

}

void initI2C(){

    I2C_disableModule(I2CA_BASE);

    I2C_initMaster(I2CA_BASE,DEVICE_SYSCLK_FREQ,400000,I2C_DUTYCYCLE_50);
    I2C_setConfig(I2CA_BASE,I2C_MASTER_SEND_MODE);
    I2C_setDataCount(I2CA_BASE,2);
    I2C_setBitCount(I2CA_BASE,I2C_BITCOUNT_8);

    I2C_setSlaveAddress(I2CA_BASE,SLAVE_ADDRESS);
    //I2C_setOwnSlaveAddress(I2CA_BASE,SLAVE_ADDRESS);
    I2C_setEmulationMode(I2CA_BASE,I2C_EMULATION_STOP_SCL_LOW);

    I2C_enableModule(I2CA_BASE);

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
    int i=0;
    test_gpioInit();
    GPIO_setPinConfig(GPIO_32_SDAA);
    GPIO_setPadConfig(32,GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(32,GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_105_SCLA);
    GPIO_setPadConfig(105,GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(105,GPIO_QUAL_ASYNC);

    //Interrupt_initModule();
    //Interrupt_initVectorTable();
    //Interrupt_register(INT_I2CA,&i2cISR);
    initI2C();

    while(1){

        //read ds3231 address-0x00's data

        I2C_setDataCount(I2CA_BASE,1);

        I2C_putData(I2CA_BASE,sData);

        I2C_setConfig(I2CA_BASE,I2C_MASTER_SEND_MODE);

        I2C_sendStartCondition(I2CA_BASE);
        I2C_sendStopCondition(I2CA_BASE);

        DEVICE_DELAY_US(100000);

        //I2C_setDataCount(I2CA_BASE,1);
        I2C_setConfig(I2CA_BASE,I2C_MASTER_RECEIVE_MODE);
        I2C_sendStartCondition(I2CA_BASE);
        I2C_sendStopCondition(I2CA_BASE);
        DEVICE_DELAY_US(100);
        rData=I2C_getData(I2CA_BASE);

        GPIO_writePin(104,1);
        for(i=0;i<rData;i++){
            DEVICE_DELAY_US(10);
        }
        GPIO_writePin(104,0);





        DEVICE_DELAY_US(100000);

    }

}

//
// End of File
//
