
//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

unsigned char sendChar=0x54;
unsigned char recvChar=0;

//
// initSCIAFIFO - Initialize the SCI FIFO
//
void initSCIAFIFO()
{
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX4);
    SCI_resetChannels(SCIA_BASE);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
}

void test_gpioInit(){
    GPIO_setDirectionMode(104,GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(105,GPIO_DIR_MODE_OUT);
}

//
// Main
//
void main(void)
{



    Device_init();
    Device_initGPIO();
    test_gpioInit();
    // GPIO64 is the SCI Rx pin.
    //
    GPIO_setMasterCore(64, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_64_SCIRXDA);
    GPIO_setDirectionMode(64, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(64, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(64, GPIO_QUAL_ASYNC);

    //
    // GPIO29 is the SCI Tx pin.
    //
    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Enables CPU interrupts
    //
    Interrupt_enableMaster();
    //
    // Initialize SCIA
    //
    //initSCIAFIFO();
    //initSCIALoopback();
   // SCI_disableLoopback(SCIA_BASE);

    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 19200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));

    while(1){
        recvChar=0;
        //SCI_writeCharNonBlocking(SCIA_BASE,sendChar);
        if(sendChar==0x54){
            sendChar=0x45;
        }else{
            sendChar=0x54;
        }

        SCI_writeCharBlockingNonFIFO(SCIA_BASE,sendChar);
        /*
        while(SCI_getRxFIFOStatus(SCIA_BASE) == SCI_FIFO_RX0)
        {
            ;
        }
        */

        DEVICE_DELAY_US(9000);
        //recvChar=~sendChar;
        //if(SCI_isDataAvailableNonFIFO(SCIA_BASE)){

            //recvChar=SCI_readCharNonBlocking(SCIA_BASE);
            //recvChar=SCI_readCharBlockingNonFIFO(SCIA_BASE);
            //recvChar=SCI_readCharBlockingFIFO(SCIA_BASE);
            if(sendChar==0x54){
                recvChar=SCI_readCharNonBlocking(SCIA_BASE);
                //recvChar=SCI_readCharBlockingFIFO(SCIA_BASE);
            }
            if(recvChar==0x54){
                //ESTOP0;
                GPIO_togglePin(104);
            }
        //}

            GPIO_togglePin(105);
    }


}

//
// End of File
//
