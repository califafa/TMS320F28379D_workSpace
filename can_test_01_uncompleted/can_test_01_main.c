
//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"


#define MSG_DATA_LENGTH 4
#define TXCOUNT 100
#define TX_MSG_OBJ_ID 1
#define RX_MSG_OBJ_ID 1

uint16_t sData[MSG_DATA_LENGTH];
uint16_t rData[MSG_DATA_LENGTH];

interrupt void canbISR(){

}

//
// Main
//
void main(void)
{
    Device_init();
    Device_initGPIO();
    int i=0;
    //GPIO_setPinConfig(DEVICE_GPIO_CFG_CANRXA);
    //GPIO_setPinConfig(DEVICE_GPIO_CFG_CANTXA);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANRXB);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANTXB);

    //CAN_initModule(CANA_BASE);
    CAN_initModule(CANB_BASE);

    //CAN_setBitRate(CANA_BASE,DEVICE_SYSCLK_FREQ,500000,16);
    CAN_setBitRate(CANB_BASE,DEVICE_SYSCLK_FREQ,500000,16);

    //CAN_enableInterrupt(CANB_BASE,CAN_INT_IE0|CAN_INT_ERROR|CAN_INT_STATUS);

    Interrupt_initModule();
    Interrupt_initVectorTable();

    EINT;
    ERTM;

    //Interrupt_register(INT_CANB0,&canbISR);

    //Interrupt_enable(INT_CANB0);
    //CAN_enableGlobalInterrupt(CANB_BASE,CAN_GLOBAL_INT_CANINT0);

    //CAN_setupMessageObject(CANA_BASE,TX_MSG_OBJ_ID,0x95555555,CAN_MSG_FRAME_EXT,CAN_MSG_OBJ_TYPE_TX,0,CAN_MSG_OBJ_NO_FLAGS,MSG_DATA_LENGTH);
    CAN_setupMessageObject(CANB_BASE,TX_MSG_OBJ_ID,0x95555555,CAN_MSG_FRAME_EXT,CAN_MSG_OBJ_TYPE_TX,0,CAN_MSG_OBJ_NO_FLAGS,MSG_DATA_LENGTH);
    //CAN_setupMessageObject(CANB_BASE,RX_MSG_OBJ_ID,0x95555555,CAN_MSG_FRAME_EXT,CAN_MSG_OBJ_TYPE_RX,0,CAN_MSG_OBJ_RX_INT_ENABLE,MSG_DATA_LENGTH);

    for(i=0;i<MSG_DATA_LENGTH;i++){
        sData[i]=i;
    }

    //CAN_startModule(CANA_BASE);
    CAN_startModule(CANB_BASE);

    for(i=0;i<(i+1);i++){
        CAN_sendMessage(CANB_BASE,TX_MSG_OBJ_ID,MSG_DATA_LENGTH,sData);
        DEVICE_DELAY_US(10000);

        sData[i%MSG_DATA_LENGTH]=~sData[i%MSG_DATA_LENGTH];

    }
}

//
// End of File
//
